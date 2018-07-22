/*  Copyright (C) 2014-2018 FastoGT. All right reserved.
    This file is part of sniffer.
    sniffer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    sniffer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with sniffer.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "utils.h"

#include <netinet/if_ether.h>

#include <string.h>

#include <array>

#include <common/macros.h>
#include <common/time.h>

#include "pcap_packages/radiotap_header.h"

#define SIZE_OF_MAC_ADDRESS ETH_ALEN
#define BROADCAST_MAC \
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }

namespace {

typedef unsigned char mac_address_t[SIZE_OF_MAC_ADDRESS];

const std::array<mac_address_t, 1> kFilteredMacs = {{BROADCAST_MAC}};

bool need_to_skipped_mac(mac_address_t mac) {
  for (size_t i = 0; i < kFilteredMacs.size(); ++i) {
    if (memcmp(kFilteredMacs[i], mac, SIZE_OF_MAC_ADDRESS) == 0) {
      return true;
    }
  }

  return false;
}
}

namespace sniffer {

PARSE_RESULT MakeEntry(const u_char* packet, const pcap_pkthdr* header, Entry* ent) {
  if (!packet || !header || !ent) {
    return PARSE_INVALID_INPUT;
  }

  bpf_u_int32 packet_len = header->caplen;
  if (packet_len < sizeof(struct radiotap_header)) {
    return PARSE_INVALID_RADIOTAP_SIZE;
  }

  struct radiotap_header* radio = (struct radiotap_header*)packet;
  packet += sizeof(struct radiotap_header);
  packet_len -= sizeof(struct radiotap_header);
  if (packet_len < sizeof(struct frame_control)) {
    return PARSE_INVALID_FRAMECONTROL_SIZE;
  }

  struct frame_control* fc = (struct frame_control*)(packet);
  mac_address_t mac = {0};
  if (fc->type == TYPE_MNGMT || fc->type == TYPE_DATA) {
    if (packet_len < sizeof(struct ieee80211header)) {
      DNOTREACHED();
      return PARSE_INVALID_PACKET;
    }

    struct ieee80211header* beac = (struct ieee80211header*)packet;
    memcpy(mac, beac->addr2, sizeof(mac));
  } else if (fc->type == TYPE_CNTRL) {
    if (fc->subtype == SUBTYPE_CNTRL_ControlWrapper || fc->subtype == SUBTYPE_CNTRL_CTS ||
        fc->subtype == SUBTYPE_CNTRL_ACK) {
      return PARSE_CNTRL_PACKET;
    }

    size_t offset_second_addr = sizeof(struct frame_control) + sizeof(uint16_t) + sizeof(mac);
    if (packet_len < offset_second_addr) {
      DNOTREACHED();
      return PARSE_INVALID_PACKET;
    }

    const u_char* second_addr = packet + offset_second_addr;
    memcpy(mac, second_addr, sizeof(mac));
  } else {
    WARNING_LOG() << "Not handled frame control type: " << static_cast<int>(fc->type);
    return PARSE_INVALID_PACKET;
  }

  if (need_to_skipped_mac(mac)) {
    // skipped_count++;
    return PARSE_SKIPPED_PACKET;
  }

  std::string source_mac = ether_ntoa((struct ether_addr*)&mac);
  common::time64_t ts_cap = common::time::timeval2mstime(&header->ts);
  *ent = Entry(source_mac, ts_cap, radio->wt_ssi_signal);  // timestamp in msec
  return PARSE_OK;
}
}

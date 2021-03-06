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

#pragma once

#include <common/libev/tcp/tcp_client.h>  // for TcpClient

#include "protocol/protocol.h"

namespace sniffer {
namespace daemon_client {

class DaemonClient : public common::libev::tcp::TcpClient {
 public:
  typedef common::libev::tcp::TcpClient base_class;
  DaemonClient(common::libev::IoLoop* server, const common::net::socket_info& info);
  virtual ~DaemonClient();

  bool IsVerified() const;
  void SetVerified(bool verif);

  const char* ClassName() const override;

 private:
  bool is_verified_;
};

typedef protocol::ProtocolClient<DaemonClient> ProtocoledDaemonClient;

}
}

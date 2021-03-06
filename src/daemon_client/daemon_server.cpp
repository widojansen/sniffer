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

#include "daemon_client/daemon_server.h"

#include "daemon_client/daemon_client.h"

namespace sniffer {
namespace daemon_client {

DaemonServer::DaemonServer(const common::net::HostAndPort& host, common::libev::IoLoopObserver* observer)
    : base_class(host, true, observer) {}

DaemonServer::~DaemonServer() {}

common::libev::tcp::TcpClient* DaemonServer::CreateClient(const common::net::socket_info& info) {
  return new DaemonClient(this, info);
}

}
}

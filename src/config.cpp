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

#include "config.h"

#include <string.h>  // for strcmp

#include <common/logger.h>  // for COMPACT_LOG_WARNING, WARNING_LOG

#include "inih/ini.h"

#define CHANNEL_COMMANDS_IN_NAME "COMMANDS_IN"
#define CHANNEL_COMMANDS_OUT_NAME "COMMANDS_OUT"
#define CHANNEL_CLIENTS_STATE_NAME "CLIENTS_STATE"

#define CONFIG_SERVER_OPTIONS "server"
#define CONFIG_SERVER_OPTIONS_ID_FIELD "id"
#define CONFIG_SERVER_OPTIONS_DB_HOSTS_FIELD "db_hosts"

#define DEFAULT_ID_FIELD_VALUE "localhost"
#define DEFAULT_DB_HOSTS_FIELD_VALUE "127.0.0.1"

/*
  [server]
  id=localhost
  db_hosts=127.0.0.1
*/

#define MATCH_FIELD(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

namespace sniffer {
namespace {
int ini_handler_fasto(void* user_data, const char* section, const char* name, const char* value) {
  Config* pconfig = reinterpret_cast<Config*>(user_data);
  if (MATCH_FIELD(CONFIG_SERVER_OPTIONS, CONFIG_SERVER_OPTIONS_ID_FIELD)) {
    pconfig->server.id = value;
    return 1;
  } else if (MATCH_FIELD(CONFIG_SERVER_OPTIONS, CONFIG_SERVER_OPTIONS_DB_HOSTS_FIELD)) {
    pconfig->server.db_hosts = value;
    return 1;
  } else {
    return 0; /* unknown section/name, error */
  }
}
}  // namespace

ServerSettings::ServerSettings() : id(DEFAULT_ID_FIELD_VALUE), db_hosts(DEFAULT_DB_HOSTS_FIELD_VALUE) {}

Config::Config() : server() {}

common::Error load_config_file(const common::file_system::ascii_file_string_path& config_path, Config* options) {
  if (!options || !config_path.IsValid()) {
    return common::make_error_inval();
  }

  std::string path = config_path.GetPath();
  ini_parse(path.c_str(), ini_handler_fasto, options);
  return common::Error();
}

common::Error save_config_file(const common::file_system::ascii_file_string_path& config_path, Config* options) {
  if (!options || !config_path.IsValid()) {
    return common::make_error_inval();
  }

  return common::Error();
}
}

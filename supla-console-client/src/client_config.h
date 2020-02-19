/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef CLIENT_CONFIG_H_
#define CLIENT_CONFIG_H_

#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include "globals.h"
#include "notification.h"
#include "supla-client-lib/log.h"
#include "supla-client-lib/tools.h"
#include "yaml/yaml.h"

class client_config {
 private:
  std::string supla_host;
  uint16_t supla_port;
  uint32_t supla_accessid;
  std::string supla_password;
  std::string supla_email;
  uint16_t supla_protocol_version;

  std::string pushover_token;
  std::string pushover_user;
  std::string pushover_device;

 public:
  client_config();
  virtual ~client_config();
  bool load(const char *config_file);

  std::string getSuplaHost();
  std::string getSuplaPassword();
  const uint16_t getSuplaPort();
  const uint32_t getSuplaAccessId();
  std::string getSuplaEmail();
  const uint16_t getSuplaProtocolVersion();
};

#endif /* CLIENT_CONFIG_H_ */

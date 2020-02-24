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

#include "client_config.h"

#include <string>

notifications* ntfns = new notifications();

client_config::client_config() {
  this->supla_host = "localhost";
  this->supla_port = 2016;
  this->supla_accessid = 1;
  this->supla_password = "";
  this->supla_email = "";
  this->supla_protocol_version = 10;

  this->pushover_token = "";
  this->pushover_user = "";
  this->pushover_device = "";
}

client_config::~client_config() {}

bool client_config::load(const char* config_file) {
  try {
    if (!st_file_exists(config_file)) {
      std::cout << "configuration file missing [parameter -config]"
                << std::endl;
      exit(2);
    }

    Yaml::Node root;
    Yaml::Parse(root, config_file);

    if (!root["supla"].IsNone()) {
      this->supla_host = root["supla"]["host"].As<std::string>("localhost");
      this->supla_port = root["supla"]["port"].As<uint16_t>(2016);
      this->supla_email = root["supla"]["email"].As<std::string>("");
      this->supla_protocol_version =
          root["supla"]["protocol_version"].As<uint16_t>(10);
    }

    if (!root["pushover"].IsNone()) {
      this->pushover_token = root["pushover"]["token"].As<std::string>("");
      this->pushover_user = root["pushover"]["user"].As<std::string>("");
      this->pushover_device = root["pushover"]["device"].As<std::string>("");
      this->pushover_title = root["pushover"]["title"].As<std::string>("SUPLA");
    }

    if (!root["notifications"].IsNone()) {
      for (auto itN = root["notifications"].Begin();
           itN != root["notifications"].End(); itN++) {
        Yaml::Node& command = (*itN).second;

        enum_trigger trigger;
        std::string str_trigger = command["trigger"].As<std::string>("none");

        if (str_trigger.compare("onchange") == 0)
          trigger = onchange;
        else if (str_trigger.compare("ontime") == 0)
          trigger = ontime;
        else if (str_trigger.compare("onconnection") == 0)
          trigger = onconnection;
        else
          trigger = none;

        enum_reset reset;

        std::string str_reset = command["reset"].As<std::string>("auto");
        if (str_reset.compare("auto") == 0)
          reset = r_automatic;
        else
          reset = r_none;

        this->pushover_title = command["title"].As<std::string>(pushover_title);
        this->pushover_device =
            command["device"].As<std::string>(pushover_device);

        ntfns->add_notifiction(
            trigger, command["time"].As<std::string>(""),
            command["condition"].As<std::string>(""), pushover_device,
            pushover_title,
            command["message"].As<std::string>("sample message"),
            pushover_token, pushover_user, reset);
      }
    }

    return true;

  } catch (std::exception& exception) {
    std::cout << exception.what() << std::endl;
    return false;
  }
}

std::string client_config::getSuplaHost() { return this->supla_host; }
std::string client_config::getSuplaPassword() { return this->supla_password; }
const uint16_t client_config::getSuplaPort() { return this->supla_port; }
const uint32_t client_config::getSuplaAccessId() {
  return this->supla_accessid;
}
const uint16_t client_config::getSuplaProtocolVersion() {
  return this->supla_protocol_version;
}

std::string client_config::getSuplaEmail() { return this->supla_email; }

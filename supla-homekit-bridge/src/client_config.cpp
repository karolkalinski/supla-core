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

client_config::client_config() {
  this->mqtt_host = "localhost";
  this->mqtt_port = 1883;
  this->mqtt_username = "";
  this->mqtt_password = "";
  this->mqtt_commands = "";
  this->mqtt_states = "";
  this->mqtt_client_name = "supla_mqtt_client";
  this->mqtt_protocol_version = 5;
  this->mqtt_publish_events = true;

  this->supla_host = "localhost";
  this->supla_port = 2016;
  this->supla_locationid = 1;
  this->supla_password = "";
  this->supla_email = "";
  this->supla_protocol_version = 10;
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

    if (!root["mqtt"].IsNone()) {
      this->mqtt_host = root["mqtt"]["host"].As<std::string>("localhost");
      this->mqtt_port = root["mqtt"]["port"].As<uint16_t>(1883);
      this->mqtt_username = root["mqtt"]["username"].As<std::string>("");
      this->mqtt_password = root["mqtt"]["password"].As<std::string>("");
      this->mqtt_commands =
          root["mqtt"]["commands_file_path"].As<std::string>("command.yaml");
      this->mqtt_states =
          root["mqtt"]["states_file_path"].As<std::string>("state.yaml");
      this->mqtt_client_name =
          root["mqtt"]["client_name"].As<std::string>("supla_mqtt_client");
      this->mqtt_protocol_version =
          root["mqtt"]["protocol_version"].As<uint16_t>(5);
      this->mqtt_publish_events =
          root["mqtt"]["publish_supla_events"].As<bool>(true);
    }

    if (!root["supla"].IsNone()) {
      this->supla_host = root["supla"]["host"].As<std::string>("localhost");
      this->supla_password = root["supla"]["password"].As<std::string>("");
      this->supla_locationid = root["supla"]["access_id"].As<uint32_t>(2016);
      this->supla_port = root["supla"]["port"].As<uint16_t>(2016);
      this->supla_email = root["supla"]["email"].As<std::string>("");
      this->supla_protocol_version =
          root["supla"]["protocol_version"].As<uint16_t>(10);
    }

    return true;
  } catch (std::exception& exception) {
    std::cout << exception.what() << std::endl;
    return false;
  }
}

bool client_config::getMqttPublishEvents() { return this->mqtt_publish_events; }

std::string client_config::getMqttCommands() { return this->mqtt_commands; }

std::string client_config::getMqttStates() { return this->mqtt_states; }

std::string client_config::getMqttClientName() {
  return this->mqtt_client_name;
}
std::string client_config::getMqttHost() { return this->mqtt_host; }
std::string client_config::getMqttUsername() { return this->mqtt_username; }
std::string client_config::getMqttPassword() { return this->mqtt_password; }
int client_config::getMqttPort() { return this->mqtt_port; }
const uint16_t client_config::getMqttProtocolVersion() {
  return this->mqtt_protocol_version;
}
std::string client_config::getSuplaHost() { return this->supla_host; }
std::string client_config::getSuplaPassword() { return this->supla_password; }
const uint16_t client_config::getSuplaPort() { return this->supla_port; }
const uint32_t client_config::getSuplaLocationId() {
  return this->supla_locationid;
}
const uint16_t client_config::getSuplaProtocolVersion() {
  return this->supla_protocol_version;
}

std::string client_config::getSuplaEmail() { return this->supla_email; }

/*
 * channel.cpp
 *
 *  Created on: 18 lut 2020
 *      Author: beku
 */

#include "channel.h"

channel::channel(int channel_id, int channel_function, std::string caption,
                 char value[SUPLA_CHANNELVALUE_SIZE],
                 char sub_value[SUPLA_CHANNELVALUE_SIZE], bool online,
                 unsigned _supla_int_t flags) {
  this->channel_id = channel_id;
  this->channel_function = channel_function;
  this->caption = caption;
  this->online = online;
  this->flags = flags;
  this->state = NULL;

  memcpy(this->value, value, SUPLA_CHANNELVALUE_SIZE);
  memcpy(this->sub_value, sub_value, SUPLA_CHANNELVALUE_SIZE);
}

channel::~channel() {
  if (this->state != NULL) {
    free(this->state);
  }
}

unsigned _supla_int_t channel::getFlags(void) { return this->flags; }

TDSC_ChannelState* channel::getState(void) { return this->state; }

void channel::add_notification_on_change(void* value) {
  bool found = false;

  for (auto p : notification_list) {
    if (p == value) {
      found = true;
      break;
    }
  }

  if (!found) {
    supla_log(LOG_DEBUG, "adding notification to channels's %d list",
              this->channel_id);
    notification_list.push_back(value);
  };
}

void channel::add_notification_on_connection(void* value) {
  bool found = false;

  for (auto p : connection_change_list) {
    if (p == value) {
      found = true;
      break;
    }
  }

  if (!found) {
    supla_log(LOG_DEBUG, "adding notification connection to channels's %d list",
              this->channel_id);
    connection_change_list.push_back(value);
  }
}

void channel::notify(void) {
  for (auto p : notification_list) {
    supla_log(LOG_DEBUG, "pushing onchange notification on channel %d",
              this->channel_id);

    notification_notify(p);
  }
}

bool channel::value_changed(char first[SUPLA_CHANNELVALUE_SIZE],
                            char second[SUPLA_CHANNELVALUE_SIZE]) {
  for (int i = 0; i < SUPLA_CHANNELVALUE_SIZE; i++) {
    if (first[i] != second[i]) return true;
  }

  return false;
}

void channel::setOnline(bool value) {
  bool hasChanged = this->online != value;

  this->online = value;

  if (hasChanged) {
    for (auto p : connection_change_list) {
      supla_log(LOG_DEBUG, "pushing onchange notification on channel %d",
                this->channel_id);

      notification_notify(p);
    }
  }
}

bool channel::getOnline(void) { return this->online; }

void channel::setValue(char value[SUPLA_CHANNELVALUE_SIZE]) {
  bool hasChanged = value_changed(this->value, value);

  if (!hasChanged) return;

  memcpy(this->value, value, SUPLA_CHANNELVALUE_SIZE);

  if (hasChanged) notify();
}
void channel::setSubValue(char sub_value[SUPLA_CHANNELVALUE_SIZE]) {
  bool hasChanged = value_changed(this->sub_value, sub_value);
  if (!hasChanged) return;

  memcpy(this->sub_value, sub_value, SUPLA_CHANNELVALUE_SIZE);

  if (hasChanged) notify();
}
void channel::setCaption(std::string value) { this->caption = value; }

void channel::setFunction(int value) { this->channel_function = value; }

void channel::getValue(char value[SUPLA_CHANNELVALUE_SIZE]) {
  memcpy(value, this->value, SUPLA_CHANNELVALUE_SIZE);
}
void channel::getSubValue(char value[SUPLA_CHANNELVALUE_SIZE]) {
  memcpy(value, this->sub_value, SUPLA_CHANNELVALUE_SIZE);
}
std::string channel::getCaption(void) { return this->caption; }

int channel::getFunction(void) { return this->channel_function; }
int channel::getChannelId(void) { return this->channel_id; }

void channel::setState(TDSC_ChannelState* state) {
  if (state == NULL) return;

  if (this->state != NULL) {
    free(this->state);
  }

  this->state = (TDSC_ChannelState*)malloc(sizeof(TDSC_ChannelState));

  this->state->BatteryHealth = state->BatteryHealth;
  this->state->BatteryLevel = state->BatteryLevel;
  this->state->BatteryPowered = state->BatteryPowered;
  this->state->BridgeNodeOnline = state->BridgeNodeOnline;
  this->state->ChannelID = state->ChannelID;
  this->state->ChannelNumber = state->ChannelNumber;
  this->state->ConnectionUptime = state->ConnectionUptime;
  this->state->IPv4 = state->IPv4;
  this->state->Fields = state->Fields;
  this->state->LastConnectionResetCause = state->LastConnectionResetCause;
  this->state->Uptime = state->Uptime;
  this->state->WiFiRSSI = state->WiFiRSSI;
  this->state->WiFiSignalStrength = state->WiFiSignalStrength;
  this->state->defaultIconField = state->defaultIconField;

  notify();
}

std::string channel::getStringValue(int index) {
  switch (this->channel_function) {
    case SUPLA_CHANNELFNC_POWERSWITCH:
    case SUPLA_CHANNELFNC_LIGHTSWITCH:
    case SUPLA_CHANNELFNC_NOLIQUIDSENSOR:
    case SUPLA_CHANNELFNC_MAILSENSOR:
    case SUPLA_CHANNELFNC_CONTROLLINGTHEDOORLOCK:
    case SUPLA_CHANNELFNC_CONTROLLINGTHEGARAGEDOOR:
    case SUPLA_CHANNELFNC_CONTROLLINGTHEGATE:
    case SUPLA_CHANNELFNC_CONTROLLINGTHEGATEWAYLOCK:
    case SUPLA_CHANNELFNC_OPENINGSENSOR_DOOR:
    case SUPLA_CHANNELFNC_OPENINGSENSOR_GARAGEDOOR:
    case SUPLA_CHANNELFNC_OPENINGSENSOR_GATE:
    case SUPLA_CHANNELFNC_OPENINGSENSOR_GATEWAY:
    case SUPLA_CHANNELFNC_OPENINGSENSOR_ROLLERSHUTTER:
    case SUPLA_CHANNELFNC_OPENINGSENSOR_WINDOW: {
      switch (index) {
        case 0:
          return std::to_string(this->value[0]); /* relay data */
        case 1:
          return std::to_string(this->sub_value[0]); /* sensor 1 data */
        case 2:
          return std::to_string(this->sub_value[1]); /* sensor 2 data */
        default:
          return std::to_string(this->value[0]);
      }
    } break;
    case SUPLA_CHANNELFNC_THERMOMETER: {
      double temp;
      memcpy(&temp, this->value, sizeof(double));

      std::stringstream ss;
      ss << std::fixed << std::setprecision(2) << temp;
      std::string mystring = ss.str();
      return ss.str();
    }
    case SUPLA_CHANNELFNC_HUMIDITYANDTEMPERATURE: {
      double temp;
      double hum;

      int n;

      memcpy(&n, this->value, 4);
      temp = n / 1000.00;

      memcpy(&n, &this->value[4], 4);
      hum = n / 1000.00;

      if (index == 0) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << temp;
        std::string mystring = ss.str();
        return ss.str();
      } else {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << hum;
        std::string mystring = ss.str();
        return ss.str();
      }
    }
    case SUPLA_CHANNELFNC_CONTROLLINGTHEROLLERSHUTTER: {
      return std::to_string(this->value[0]); /* percentage */
    }
  };

  return "0";
}

channels::channels() { arr = safe_array_init(); }

channels::~channels() {
  channel* chnl;

  for (int i = 0; i < safe_array_count(arr); i++) {
    chnl = (channel*)safe_array_get(arr, i);

    if (chnl) delete chnl;
  }

  safe_array_free(arr);
}

channel* channels::find_channel(int channel_id) {
  safe_array_lock(arr);

  channel* result = NULL;

  for (int i = 0; i < safe_array_count(arr); i++) {
    channel* temp = (channel*)safe_array_get(arr, i);
    if (temp != NULL && temp->getChannelId() == channel_id) {
      result = temp;
      break;
    }
  }

  safe_array_unlock(arr);
  return result;
}

void* channels::get_channels(void) { return this->arr; }

channel* channels::add_channel(int channel_id, int channel_function,
                               std::string caption,
                               char value[SUPLA_CHANNELVALUE_SIZE],
                               char sub_value[SUPLA_CHANNELVALUE_SIZE],
                               bool online, unsigned _supla_int_t flags) {
  safe_array_lock(arr);

  channel* result = find_channel(channel_id);

  if (result == NULL) {
    result = new channel(channel_id, channel_function, caption, value,
                         sub_value, online, flags);

    if (result != NULL && safe_array_add(arr, result) == -1) {
      delete result;
      result = NULL;
    };
  };

  safe_array_unlock(arr);
  return result;
}

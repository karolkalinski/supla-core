/*
 * channel.cpp
 *
 *  Created on: 18 lut 2020
 *      Author: beku
 */

#include "channel.h"

channel::channel(int channel_id, int channel_function, std::string caption){
	this->channel_id = channel_id;
	this->channel_function = channel_function;
	this->caption = caption;
}

channel::~channel() {
  for (auto notif : notification_list) {
    delete notif;
  }

  notification_list.clear();
  std::vector<notification*>().swap(notification_list);
}

void channel::addNotification(notification* value) {
	
  bool found = false;
  for (auto notif : notification_list){
    if (notif == value)
	{
		found = true;
		break;
	}
  }
  if (!found)
	notification_list.push_back(value);
}

void channel::notify(void) {
  for (auto notif: notification_list) {
    notif->notify();
  }
}

void channel::setValue(char value[SUPLA_CHANNELVALUE_SIZE]) {
  
  bool hasChanged = false;
  if (strcmp(value, this->value) != 0)
    hasChanged = true;
  
  memcpy(this->value, value, SUPLA_CHANNELVALUE_SIZE);
  
  if (hasChanged) notify();
}
void channel::setSubValue(char value[SUPLA_CHANNELVALUE_SIZE]) {
	memcpy(this->sub_value, value, SUPLA_CHANNELVALUE_SIZE);
}
void channel::setCaption(std::string value) { this->caption = value; }

void channel::setFunction(int value) { this->channel_function = value; }

void channel::getValue(char value[SUPLA_CHANNELVALUE_SIZE]) {
	memcpy(value, this->value, SUPLA_CHANNELVALUE_SIZE);
}
void channel::getSubValue(char value[SUPLA_CHANNELVALUE_SIZE]) {
	memcpy(value, this->sub_value, SUPLA_CHANNELVALUE_SIZE);
}
std::string channel::getCaption(void) {return this->caption; }

int channel::getFunction(void) { return this->channel_function; }
int channel::getChannelId(void) { return this->channel_id; }

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
			return std::to_string(this->value[0]);
		} break;
		case SUPLA_CHANNELFNC_THERMOMETER:
		{
			double temp;
			memcpy(&temp, this->value, sizeof(double));
			return std::to_string(temp);
		}
		case SUPLA_CHANNELFNC_HUMIDITYANDTEMPERATURE:
		{
			double temp;
			double hum;

			int n;

     	    memcpy(&n, this->value, 4);
			temp = n / 1000.00;

			memcpy(&n, &this->value[4], 4);
			hum = n / 1000.00;


			if (index == 0) return std::to_string(temp);
			else
				return std::to_string(hum);
		}
	};

	return "0";
}

channels::channels() {
	arr = safe_array_init();
}

channels::~channels() {
	channel* chnl;

	for (int i = 0; i< safe_array_count(arr); i++) {

		chnl = (channel*)safe_array_get(arr, i);

		if (chnl) delete chnl;
	}

	safe_array_free(arr);
}

channel* channels::find_channel(int channel_id) {
	safe_array_lock(arr);

	channel* result = NULL;

	for (int i = 0; i< safe_array_count(arr); i++) {

		channel* temp = (channel*)safe_array_get(arr, i);
		if (temp != NULL && temp->getChannelId() == channel_id)
		{
			result = temp;
			break;
		}
	}

	safe_array_unlock(arr);
	return result;
}

channel* channels::add_channel(int channel_id, int channel_function, std::string caption) {
	safe_array_lock(arr);

	channel* result = find_channel(channel_id);

	if (result == NULL) {

		result = new channel(channel_id, channel_function, caption);

		if (result != NULL && safe_array_add(arr, result) == -1) {
		      delete result;
		      result = NULL;
		};
	};

	safe_array_unlock(arr);
	return result;
}

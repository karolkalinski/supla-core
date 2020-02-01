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

#include "client_device.h"
#include "globals.h"
#include "PHKAccessory.h"

void* lck = lck_init();
void* lck_value = lck_init();

void identify_callback_routine_execution(void *ptr, void *aid) {
	lck_lock(lck);

	client_device_channel* channel = channels->find_channel(*((int *) aid) - 1);

	if (channel)
		supla_log(LOG_DEBUG, "Channel found");
	else
		supla_log(LOG_DEBUG, "Channel not found");

	lck_unlock(lck);
}

void set_value_callback_routine_execution(void *ptr, void *aid) {

	lck_lock(lck_value);

	client_device_channel* channel = channels->find_channel(*((_supla_int64_t *) aid)-1);

	if (channel) {
		switch (channel->getFunc()) {
		case SUPLA_CHANNELFNC_POWERSWITCH:
		case SUPLA_CHANNELFNC_LIGHTSWITCH: {
			boolCharacteristic* c = ((boolCharacteristic *) ptr);

			if (c->getValue())
				supla_client_open(sclient, channel->getId(), 0, 1);
			else
				supla_client_open(sclient, channel->getId(), 0, 0);
		}
			break;
		};
	}

	lck_unlock(lck_value);
}

client_device_channel::~client_device_channel() {

}

client_device_channel::client_device_channel(int Id, int Number, int Type,
		int Func, int Param1, int Param2, int Param3, char *TextParam1,
		char *TextParam2, char *TextParam3, bool Hidden, bool Online,
		char *Caption, int accessoryId) :
		supla_device_channel(Id, Number, Type, Func, Param1, Param2, Param3,
				TextParam1, TextParam2, TextParam3, Hidden) {
	this->Online = Online;
	this->Caption =
			Caption ? strndup(Caption, SUPLA_CHANNEL_CAPTION_MAXSIZE) : NULL;
	this->isHK = false;

	memset(this->Sub_value, 0, SUPLA_CHANNELVALUE_SIZE);

	this->firmware_revision = "1.0";
	this->manufacturer = "unknown";
	this->model = "unknown";
	this->name = std::string(Caption);
	this->serial_number = "0000000000";
}



int client_device_channel::getAccessoryId(void) { return this->getId() + 1; }


int client_device_channel::getId(void) { return supla_device_channel::getId(); }

void client_device_channel::setHKValue(char value[SUPLA_CHANNELVALUE_SIZE]) {
	this->setValue(value);

	jsoncons::json describe;
	jsoncons::json characteristics = jsoncons::json::make_array();
	bool wasDescribed = false;

	switch (this->getFunc()) {
	case SUPLA_CHANNELFNC_HUMIDITYANDTEMPERATURE: {

		accessory* accessory = accessories->getAccessoryById(this->getAccessoryId());

		if (!accessory)
			break;

		service* service = accessory->getServiceByType(
				serviceType_temperatureSensor);

		if (!service)
			break;

		floatCharacteristic* temperature =
				(floatCharacteristic*) service->getCharacteristicByType(
						charType_currentTemperature);

		if (!temperature)
			break;

		boolCharacteristic* active = (boolCharacteristic*)service->getCharacteristicByType(charType_sensorActive);

		if (active)
		{
			active->setValue(this->Online);
			characteristics.push_back(active->describeValue());
		}

		service = accessory->getServiceByType(serviceType_humiditySensor);

		if (!service)
					break;

		floatCharacteristic* humidity =
				(floatCharacteristic*)service->getCharacteristicByType(charType_currentHumidity);

		active = (boolCharacteristic*)service->getCharacteristicByType(charType_sensorActive);

		if (active)
		{
			active->setValue(this->Online);
			characteristics.push_back(active->describeValue());
		}

		if (!humidity)
			break;

		supla_channel_temphum* tempHum = this->getTempHum();
		if (tempHum) {
		  double temp = tempHum->getTemperature();
		  double hum = tempHum->getHumidity();
		  delete tempHum;

		  temperature->setValue(temp);
          characteristics.push_back(temperature->describeValue());
          humidity->setValue(hum);
          characteristics.push_back(humidity->describeValue());
		  wasDescribed = true;
        }

	} break;
	case SUPLA_CHANNELFNC_THERMOMETER: {
		accessory* accessory = accessories->getAccessoryById(this->getAccessoryId());

		if (!accessory)
			break;

		service* service = accessory->getServiceByType(
				serviceType_temperatureSensor);

		if (!service)
			break;

		floatCharacteristic* temperature =
				(floatCharacteristic*) service->getCharacteristicByType(
						charType_currentTemperature);

		if (!temperature)
			break;

		boolCharacteristic* active = (boolCharacteristic*)service->getCharacteristicByType(charType_sensorActive);

		if (active)
    	{
			active->setValue(this->Online);
			characteristics.push_back(active->describeValue());
		}


		double temp;
		this->getDouble(&temp);
		temperature->setValue(temp);

		characteristics.push_back(temperature->describeValue());

		wasDescribed = true;
	}
		break;
	case SUPLA_CHANNELFNC_LIGHTSWITCH: {
		accessory* accessory = accessories->getAccessoryById(this->getAccessoryId());
		if (!accessory)
			break;

		service* service = accessory->getServiceByType(serviceType_lightBulb);
		if (!service)
			break;

		boolCharacteristic* on =
				(boolCharacteristic*) service->getCharacteristicByType(
						charType_on);

		if (!on)
			break;

		char value;
		this->getChar(&value);
		on->setValue(value > 0);

		characteristics.push_back(on->describeValue());
		wasDescribed = true;
	}
		break;
	case SUPLA_CHANNELFNC_POWERSWITCH: {
			accessory* accessory = accessories->getAccessoryById(this->getAccessoryId());
			if (!accessory)
				break;

			service* service = accessory->getServiceByType(serviceType_switch);
			if (!service)
				break;

			boolCharacteristic* on =
					(boolCharacteristic*) service->getCharacteristicByType(
							charType_on);

			if (!on)
				break;

			char value;
			this->getChar(&value);
			on->setValue(value > 0);

			characteristics.push_back(on->describeValue());
			wasDescribed = true;
		}
			break;
	};

	if (wasDescribed) {
		std::string broadcastTemp;
		describe["characteristics"] = characteristics;
		describe.dump(broadcastTemp);

		char* cstr = strdup(broadcastTemp.c_str());

		broadcastInfo * info = new broadcastInfo;
		info->sender = NULL;
		info->desc = cstr;

		pthread_t thread;
		pthread_create(&thread, NULL, announce, info);
	}
}

std::string client_device_channel::getSerialNumber(void) {
	return this->serial_number;
}
std::string client_device_channel::getName(void) {
	return this->name;
}
std::string client_device_channel::getModel(void) {
	return this->model;
}
std::string client_device_channel::getManufacturer(void) {
	return this->manufacturer;
}
std::string client_device_channel::getFirmwareRevision(void) {
	return this->firmware_revision;
}

char *client_device_channel::getCaption(void) {
	return Caption;
}

bool client_device_channel::isHidden(void) {
	return !this->isHK;
}

void client_device_channel::setSubValue(
		char sub_value[SUPLA_CHANNELVALUE_SIZE]) {
	memcpy(this->Sub_value, sub_value, SUPLA_CHANNELVALUE_SIZE);
}

void client_device_channel::getSubValue(
		char sub_value[SUPLA_CHANNELVALUE_SIZE]) {
	memcpy(sub_value, this->Sub_value, SUPLA_CHANNELVALUE_SIZE);
}

void client_device_channel::setOnline(bool value) {
	this->Online = value;




}
bool client_device_channel::getOnline() {
	return this->Online;
}

client_device_channel* client_device_channels::add_channel(int Id, int Number,
		int Type, int Func, int Param1, int Param2, int Param3,
		char *TextParam1, char *TextParam2, char *TextParam3, bool Hidden,
		bool Online, char *Caption) {

	safe_array_lock(arr);

	client_device_channel *channel = find_channel(Id);

	if (channel == 0) {
		channel = new client_device_channel(Id, Number, Type, Func, Param1,
				Param2, Param3, TextParam1, TextParam2, TextParam3, Hidden,
				Online, Caption, ++current_accessory_id);

		if (channel != NULL && safe_array_add(arr, channel) == -1) {
			delete channel;
			channel = NULL;
		} else {


			accessories->add_accessory_for_supla_channel(channel,
						identify_callback_routine_execution,
						set_value_callback_routine_execution);
		}
	} else {
		channel->setOnline(Online);
	}

	safe_array_unlock(arr);

	return channel;
}

void client_device_channels::set_channel_sub_value(int ChannelID,
		char sub_value[SUPLA_CHANNELVALUE_SIZE]) {
	if (ChannelID == 0)
		return;

	safe_array_lock(arr);

	client_device_channel *channel = find_channel(ChannelID);

	if (channel) {
		channel->setSubValue(sub_value);
	}
	safe_array_unlock(arr);
}

client_device_channel *client_device_channels::find_channel(int ChannelId) {
	return (client_device_channel *) safe_array_findcnd(arr, arr_findcmp,
			&ChannelId);
}

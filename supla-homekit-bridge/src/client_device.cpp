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
/* callback homekit->supla notify */
void identify_callback_routine_execution(void *ptr, void *aid) {
	lck_lock(lck);

	client_device_channel* channel = channels->find_channel(*((int *) aid) - 1);

	if (channel)
		supla_log(LOG_DEBUG, "Channel found");
	else
		supla_log(LOG_DEBUG, "Channel not found");

	lck_unlock(lck);
}

/* callback homekit->supla change value*/
void set_value_callback_routine_execution(void *ptr, void *aid) {

    /* one change on time */
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
			case SUPLA_CHANNELFNC_CONTROLLINGTHEGARAGEDOOR:
			{
				/* is not really mather that homekit sends open/close. in supla there is an relay to run*/
				supla_client_open(sclient, channel->getId(), 0, 1);
			} break;
			case SUPLA_CHANNELFNC_CONTROLLINGTHEDOORLOCK:
			{
				uint8Characteristic* c = ((uint8Characteristic*) ptr);
				supla_client_open(sclient, channel->getId(), 0, c->getValue());
					
			} break;
			case SUPLA_CHANNELFNC_CONTROLLINGTHEROLLERSHUTTER:
			{
				characteristic* c = ((characteristic*) ptr);
				
				if (c->getType() == charType_holdPosition) {
				  supla_client_open(sclient, channel->getId(), 0, 0);
				} else if (c->getType() == charType_targetPosition) {
				  supla_client_open(sclient, channel->getId(), 0, ((uint8Characteristic*)c)->getValue() + 10);	
				}
			} break;
		};
	}
	
	lck_unlock(lck_value);
}

client_device_channel::~client_device_channel() {
  if (this->Caption) {
	  free(this->Caption);
	  this->Caption = NULL;
  }
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
	
	this->isHK = true;

	memset(this->Sub_value, 0, SUPLA_CHANNELVALUE_SIZE);

    /* TODO lbek  read those data from channel info ... */
	this->firmware_revision = "1.0";
	this->manufacturer = "unknown";
	this->model = "unknown";
	this->name = std::string(Caption);
	this->serial_number = "0000000000";
	 
}


/* bridge has 0 id so to avoid duplicate it's added 1 */
int client_device_channel::getAccessoryId(void) { return this->getId() + 1; }
int client_device_channel::getId(void) { return supla_device_channel::getId(); }

void client_device_channel::setHKValue(
    char value[SUPLA_CHANNELVALUE_SIZE], char sub_value[SUPLA_CHANNELVALUE_SIZE],
	bool online) {
	
	/* check change type */
    int positionState = 0;
	char current_value[SUPLA_CHANNELVALUE_SIZE];
	this->getValue(current_value);
		
	switch (this->getFunc()) {
	  case SUPLA_CHANNELFNC_CONTROLLINGTHEGARAGEDOOR:
	  {
		 if (current_value[0] == 0 && value[0] == 1) /* zatrzymanie -> start */
		 {
			if (this->sub_value[0] == 0) /* zamykanie */
			   positionState = 3;
			 else if (this->sub_value[0] == 1) /* otwieranie */
			   positionState = 2;
		 } else if (current_value[0] == 1 && value[0] == 0) /* ruch -> stop */	 
		 {
			if (this->sub_value[0] == 0) /* otwarte */
			  positionState = 3;
			else if (this->sub_value[0] == 1) /* zamkniete */ 
			  positionState = 2;
		 } else if (current_value[0] ==  0 && value[0] == 0) /* zmiana wartosci czujnika*/
		 {
			if (this->sub_value[0] == 0) /* otwarte */
			  positionState = 0;
			else if (this->sub_value[0] == 1) /* zamkniete */ 
			  positionState = 1;
		 }
	  } break;
	  case SUPLA_CHANNELFNC_CONTROLLINGTHEROLLERSHUTTER:
	  {
		  if (value[0] > current_value[0])
			positionState = 1;
		  else if (values[0] < current_value[0])
			positionState = 0;
		  else positionState = 2;
	  }
	}
	
	this->setValue(value);
    this->setSubValue(sub_value);
	this->setOnline(online);

    accessory* accessory = accessories->getAccessoryById(this->getAccessoryId());
	if (!accessory) return;

	jsoncons::json describe;
	jsoncons::json characteristics = jsoncons::json::make_array();
	bool wasDescribed = false;

	switch (this->getFunc()) {
		case SUPLA_CHANNELFNC_HUMIDITYANDTEMPERATURE: {
		
		service* service = accessory->getServiceByType(
		  serviceType_temperatureSensor);

		if (!service) return;

		floatCharacteristic* temperature =
				(floatCharacteristic*) service->getCharacteristicByType(
						charType_currentTemperature);

		if (!temperature) return;

		boolCharacteristic* active = (boolCharacteristic*)service->getCharacteristicByType(charType_sensorActive);

		if (active)
		{
			active->setValue(this->Online);
			characteristics.push_back(active->describeValue());
		}

		service = accessory->getServiceByType(serviceType_humiditySensor);

		if (!service) return;

		floatCharacteristic* humidity =
				(floatCharacteristic*)service->getCharacteristicByType(charType_currentHumidity);

		active = (boolCharacteristic*)service->getCharacteristicByType(charType_sensorActive);

		if (active)
		{
			active->setValue(this->Online);
			characteristics.push_back(active->describeValue());
		}

		if (!humidity) return;

		supla_channel_temphum* tempHum = this->getTempHum();
		
		if (tempHum) {
		  double temp = tempHum->getTemperature();
		  double hum = tempHum->getHumidity();
		  delete tempHum;

		  temperature->setValue(temp);
          humidity->setValue(hum);
          
		  characteristics.push_back(temperature->describeValue());
          characteristics.push_back(humidity->describeValue());
		  wasDescribed = true;
        }

	} break;
	case SUPLA_CHANNELFNC_THERMOMETER: {
		service* service = accessory->getServiceByType(
				serviceType_temperatureSensor);

		if (!service) return;

		floatCharacteristic* temperature =
				(floatCharacteristic*) service->getCharacteristicByType(
						charType_currentTemperature);

		if (!temperature) return;

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
	case SUPLA_CHANNELFNC_LIGHTSWITCH:
    case SUPLA_CHANNELFNC_POWERSWITCH:
	{
		service* = NULL;
		
		if (this->getFunc() == SUPLA_CHANNELFNC_LIGHTSWITCH)
		  service = accessory->getServiceByType(serviceType_lightBulb);
		else 
		  service = accessory->getServiceByType(serviceType_switch);
	  
		if (!service) return;
		
		boolCharacteristic* on =
				(boolCharacteristic*) service->getCharacteristicByType(
						charType_on);

		if (!on) return;

		char value;
		this->getChar(&value);
		
		on->setValue(value > 0);

		characteristics.push_back(on->describeValue());
		wasDescribed = true;
	}	break;
    case SUPLA_CHANNELFNC_CONTROLLINGTHEDOORLOCK:
	{
		service* service = accessory->getServiceByType(serviceType_lockMechanism);
		if (!service) return;
		
		uint8Characteristic* targetLockState = (uint8Characteristic*)service->getCharacteristicByType(charType_targetDoorState);
		if (!targetDoorState) break;

		uint8Characteristic* currentLockState = (uint8Characteristic*)service->getCharacteristicByType(charType_currentDoorState);
		if (!currentDoorState) break;
		
		char val[SUPLA_CHANNELVALUE_SIZE];
		this->getValue(val);
		
		currentLockState->setValue(this->sub_value[0]);
		targetDoorState->setValue(val[0]);
		
		characteristics.push_back(targetLockState->describeValue());
		characteristics.push_back(currentLockState->describeValue());
		wasDescribed = true;
	} break;
	case SUPLA_CHANNELFNC_CONTROLLINGTHEROLLERSHUTTER:
	{
		service* service = accessory->getServiceByType(serviceType_windowCovering);
		if (!service) return;
		
		boolCharacteristic* obstruction = (boolCharacteristic*)service->getCharacteristicByType(charType_obstruction);

		if (!obstruction) return;
		obstruction->setValue(!this->getOnline());
		
		uint8Characteristic* currentPosition = (uint8Characteristic*)service->getCharacteristicByType(charType_currentPosition);
		
		if (!currentPosition) return;
		
		char cv[SUPLA_CHANNELVALUE_SIZE];
        this->getChar(cv);
        char sub_value[SUPLA_CHANNELVALUE_SIZE];
        this->getSubValue(sub_value);
        char shut = cv[0];
				
		currentPosition->setValue(100 - shut);
		
		uint8Characteristic* targetPosition = (uint8Characteristic*)service->getCharacteristicByType(charType_targetosition);
		
		if (!targetPosition) return;
		
		targetPosition->setValue(100 - shut);
		
		uint8Characteristic* ps = (uint8Characteristic*)service->getCharacteristicByType(charType_positionState);
		
		if (!ps) return;
		
		ps->setValue(positionState);
		
		characteristics->push_back(currentPosition);
		characteristics->push_back(targetPosition);
		characteristics->push_back(positionState);
		characteristics->push_back(obstruction);
		wasDescribed = true;
		
	} break;
	case SUPLA_CHANNELFNC_CONTROLLINGTHEGARAGEDOOR:
	{
		
		service* service = accessory->getServiceByType(serviceType_garageDoorOpener);

		if (!service) break;

		boolCharacteristic* obstruction = (boolCharacteristic*)service->getCharacteristicByType(charType_obstruction);

		if (!obstruction) break;
		obstruction->setValue(!this->getOnline());

		uint8Characteristic* targetDoorState = (uint8Characteristic*)service->getCharacteristicByType(charType_targetDoorState);
		if (!targetDoorState) break;

		uint8Characteristic* currentDoorState = (uint8Characteristic*)service->getCharacteristicByType(charType_currentDoorState);
		if (!currentDoorState) break;

	
		currentDoorState->setValue(positionState);
		targetDoorState->setValue(this->Sub_value[0]);

		characteristics.push_back(targetDoorState->describeValue());
		characteristics.push_back(currentDoorState->describeValue());
		characteristics.push_back(obstruction->describeValue());
		wasDescribed = true;
	} break;
	};

	if (wasDescribed) {
		std::string broadcastTemp;
		describe["characteristics"] = characteristics;
		describe.dump(broadcastTemp);

		broadcastInfo * info = new broadcastInfo;
		info->sender = NULL;
		info->desc = strdup(broadcastTemp.c_str());

		pthread_t* thread = (pthread_t*)malloc(sizeof(pthread_t));

		pthread_create(thread, NULL, announce, info);
		pthread_detach(*thread);

		free(thread);
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

void client_device_channel::setFunc(int value) {
	this->Func = value;
}

bool client_device_channel::getOnline() {
	return this->Online;
}

client_device_channels::~client_device_channels(){
	lck_free(lck_value);
	  lck_free(lck);
}

client_device_channels::client_device_channels() {
	this->initialized = false;
}

bool client_device_channels::getInitialized(void) {
	return this->initialized;
}
void client_device_channels::setInitialized(bool value) {
	this->initialized = value;
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

			if (!channel->isHidden())
			accessories->add_accessory_for_supla_channel(channel,
						identify_callback_routine_execution,
						set_value_callback_routine_execution);
		}
	} else {
		channel->setOnline(Online);
		channel->setFunc(Func);
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



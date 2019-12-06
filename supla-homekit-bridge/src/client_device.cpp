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

client_device_channel::client_device_channel(
    int Id, int Number, int Type, int Func, int Param1, int Param2, int Param3,
    char *TextParam1, char *TextParam2, char *TextParam3, bool Hidden,
    bool Online, char *Caption)
    : supla_device_channel(Id, Number, Type, Func, Param1, Param2, Param3,
                           TextParam1, TextParam2, TextParam3, Hidden) {
  this->Online = Online;
  this->Caption =
      Caption ? strndup(Caption, SUPLA_CHANNEL_CAPTION_MAXSIZE) : NULL;
  
  this->accessoryId = -1;
  memset(this->Sub_value, 0, SUPLA_CHANNELVALUE_SIZE);
}

client_device_channel::~client_device_channel() {
  if (this->Caption) {
    free(this->Caption);
    this->Caption = NULL;
  }
}

int client_device_channel::getAccessoryId(void) { return this->accessoryId; }

void client_device_channel::notifyHomekit(void) {
    
   Accessory *a = AccessorySet::getInstance().accessoryAtIndex(this->getAccessoryId());
   
   if (a == NULL) return;
   
   switch (this->getFunc()) {
	  case SUPLA_CHANNELFNC_CONTROLLINGTHEDOORLOCK: {
	  
	    char value[SUPLA_CHANNELVALUE_SIZE];
        char sub_value[SUPLA_CHANNELVALUE_SIZE];
        channel->getSubValue(sub_value); /* sensors */
        channel->getChar(value); /* relay */
        bool hi = value[0] > 0;
		
		intCharacteristics* c = a->characteristicAtType(charType_currentPosition);
		
		char current_position = 0;
		
		if (c != NULL) {
		
			if (sub_value[0] > 0)
			  current_position = 100;
			
			c->setValue(value);
			c->notify();
		};
		
		c = a->characteristicAtType(charType_positionState);
		
		if (c != NULL) {
		  
		  char value = 2;

          if (hi) {
			
			if (current_position = 0) {
				value = 1; 
			} else {
			    value = 0;
			};
			  
		  };
           			  
		  c->setValue(value);
		  c->notify();
		};
		
		c = a->characteristicAtType(charType_targetPosition);
		
		if (c!= NULL) {
		  c->setValue(std::toString(hi));
		  c->notify();
		}
	    
	  }; break;
	  
	  case SUPLA_CHANNELFNC_CONTROLLINGTHEGARAGEDOOR: {
	  
	    char value[SUPLA_CHANNELVALUE_SIZE];
        char sub_value[SUPLA_CHANNELVALUE_SIZE];
        channel->getSubValue(sub_value); /* sensors */
        channel->getChar(value); /* relay */
        bool hi = value[0] > 0;
		bool isOnline = channel->getOnline();
		
		intCharacteristics* c = a->characteristicAtType(charType_currentDoorState);
		
		if (c != NULL) {
			
			char value = 4;
			if (sub_value[0] > 0) {
			  value = 0;
			  if (hi) {
				  value = 2;
			  }
			  
			} else {
			  value = 1;
			  if (hi) {
			    value = 3;  
			  }; 			  
			};
			
			c->setValue(value);
			c->notify();
		};
		
		c = a->characteristicAtType(charType_targetDoorState);
		
		if (c != NULL) {
		  c->setValue(std::toString(hi));
		  c->notify();
		};
		
		c = a->characteristicAtType(charType_obstruction);
		
		if (c != NULL) {
		  c->setValue(std::toString(!isOnline));
		  c->notify();
		};
		
	  } break;
	  case SUPLA_CHANNELFNC_THERMOMETER:
      case SUPLA_CHANNELFNC_HUMIDITY:
      case SUPLA_CHANNELFNC_HUMIDITYANDTEMPERATURE: {
		  
        supla_channel_temphum* tempHum = channel->getTempHum();
        if (tempHum) {
          double temp = tempHum->getTemperature();
          
		  floatCharacteristics* c = a->characteristicAtType(charType_currentTemperature); 
		  
		  if (c != NULL) {
			c->setValue(std::to_string(temp));
			c->notify();
		  }
		  
		  if (tempHum->isTempAndHumidity()) {
			double hum = tempHum->getHumidity();

			c = a->characteristicAtType(charType_currentTemperature); 

			if (c != NULL) {
			  c->setValue(std::to_string(hum));
			  c->notify();
		    }
    	  }
		  
		  delete tempHum;
        }
     } break;
	 case SUPLA_CHANNELFNC_POWERSWITCH: 
     case SUPLA_CHANNELFNC_LIGHTSWITCH: {
		
		boolCharacteristics* c = a->characteristicAtType(charType_on); 
		
		if (c != NULL) {
		  char cv[SUPLA_CHANNELVALUE_SIZE];
          this->getChar(cv);
          bool hi = cv[0] > 0;	
		  c->setValue(cv[0]);
		  c->notify();
		}
	 } break;
	 
   }	   
}

char *client_device_channel::getCaption(void) { return Caption; }

void client_device_channel::setSubValue(
    char sub_value[SUPLA_CHANNELVALUE_SIZE]) {
  memcpy(this->Sub_value, sub_value, SUPLA_CHANNELVALUE_SIZE);
}

void client_device_channel::getSubValue(
    char sub_value[SUPLA_CHANNELVALUE_SIZE]) {
  memcpy(sub_value, this->Sub_value, SUPLA_CHANNELVALUE_SIZE);
}

void client_device_channel::setOnline(bool value) { this->Online = value; }
bool client_device_channel::getOnline() { return this->Online; }

client_device_channel* client_device_channels::add_channel(
    int Id, int Number, int Type, int Func, int Param1, int Param2, int Param3,
    char *TextParam1, char *TextParam2, char *TextParam3, bool Hidden,
    bool Online, char *Caption) {
  
  safe_array_lock(arr);

  client_device_channel *channel = find_channel(Id);
  if (channel == 0) {
    if (Type == 0) {
      /* enable support for proto version < 10 */
      switch (Func) {
        case SUPLA_CHANNELFNC_THERMOMETER:
          Type = SUPLA_CHANNELTYPE_THERMOMETERDS18B20;
          break;
        case SUPLA_CHANNELFNC_DISTANCESENSOR:
          Type = SUPLA_CHANNELTYPE_DISTANCESENSOR;
          break;
        case SUPLA_CHANNELFNC_DIMMER:
          Type = SUPLA_CHANNELTYPE_DIMMER;
          break;
        case SUPLA_CHANNELFNC_DIMMERANDRGBLIGHTING:
          Type = SUPLA_CHANNELTYPE_DIMMERANDRGBLED;
          break;
        case SUPLA_CHANNELFNC_THERMOSTAT:
          Type = SUPLA_CHANNELTYPE_THERMOSTAT;
          break;
        case SUPLA_CHANNELFNC_THERMOSTAT_HEATPOL_HOMEPLUS:
          Type = SUPLA_CHANNELTYPE_THERMOSTAT_HEATPOL_HOMEPLUS;
          break;
        case SUPLA_CHANNELFNC_RGBLIGHTING:
          Type = SUPLA_CHANNELTYPE_RGBLEDCONTROLLER;
          break;
        case SUPLA_CHANNELFNC_GAS_METER:
        case SUPLA_CHANNELFNC_WATER_METER:
          Type = SUPLA_CHANNELTYPE_IMPULSE_COUNTER;
          break;
        case SUPLA_CHANNELFNC_ELECTRICITY_METER:
          Type = SUPLA_CHANNELTYPE_ELECTRICITY_METER;
          break;
        case SUPLA_CHANNELFNC_HUMIDITY:
        case SUPLA_CHANNELFNC_HUMIDITYANDTEMPERATURE:
          Type = SUPLA_CHANNELTYPE_DHT11;
          break;
        case SUPLA_CHANNELFNC_WEATHER_STATION:
          Type = SUPLA_CHANNELTYPE_WEATHER_STATION;
          break;
        case SUPLA_CHANNELFNC_WEIGHTSENSOR:
          Type = SUPLA_CHANNELTYPE_WEIGHTSENSOR;
          break;
        case SUPLA_CHANNELFNC_RAINSENSOR:
          Type = SUPLA_CHANNELTYPE_RAINSENSOR;
          break;
        case SUPLA_CHANNELFNC_PRESSURESENSOR:
          Type = SUPLA_CHANNELTYPE_PRESSURESENSOR;
          break;
        case SUPLA_CHANNELFNC_WINDSENSOR:
          Type = SUPLA_CHANNELTYPE_WINDSENSOR;
          break;
        case SUPLA_CHANNELFNC_MAILSENSOR:
          Type = SUPLA_CHANNELTYPE_SENSORNC;
          break;
        case SUPLA_CHANNELFNC_LIGHTSWITCH:
        case SUPLA_CHANNELFNC_POWERSWITCH:
          Type = SUPLA_CHANNELTYPE_RELAY;
          break;
      }
    }

    *channel = new client_device_channel(
        Id, Number, Type, Func, Param1, Param2, Param3, TextParam1, TextParam2,
        TextParam3, Hidden, Online, Caption);

    if (channel != NULL && safe_array_add(arr, channel) == -1) {
      delete channel;
      channel = NULL;
	} else {
		
	  /*add homekit accessory */
	}
  } else {
    channel->setOnline(Online);
  }

  safe_array_unlock(arr);
  
  return channel;
}

void client_device_channels::set_channel_sub_value(
    int ChannelID, char sub_value[SUPLA_CHANNELVALUE_SIZE]) {
  if (ChannelID == 0) return;

  safe_array_lock(arr);

  client_device_channel *channel = find_channel(ChannelID);

  if (channel) {
    channel->setSubValue(sub_value);
  }
  safe_array_unlock(arr);
}

client_device_channel *client_device_channels::find_channel(int ChannelId) {
  return (client_device_channel *)safe_array_findcnd(arr, arr_findcmp,
															&ChannelId);
}

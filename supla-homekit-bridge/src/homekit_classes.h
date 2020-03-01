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

#ifndef HOMEKIT_CLASSES_H
#define HOMEKIT_CLASSES_H

 
#include "json/jsoncons/json.hpp"
#include "client_device.h"

#include <math.h>
#include <mutex>

#define HOMEKIT_STRING_MAX_LENGTH 256
#define HOMEKIT_STRING_DEFAULT_LENGTH 64

typedef enum {
    deviceType_other = 0,
    deviceType_bridge = 2,
    deviceType_fan = 3,
    deviceType_garageDoorOpener = 4,
    deviceType_lightBulb = 5,
    deviceType_doorLock = 6,
    deviceType_outlet = 7,
    deviceType_switch = 8,
    deviceType_thermostat = 9,
    deviceType_sensor = 10,
    deviceType_alarmSystem = 11,
    deviceType_door = 12,
    deviceType_window = 13,
    deviceType_windowCover = 14,
    deviceType_programmableSwitch = 15,
} deviceType;
typedef enum {
    charType_adminOnlyAccess    = 0x1,
    charType_audioChannels      = 0x2,
    charType_audioCodexName     = 0x3,
    charType_audioCodexParameter= 0x4,
    charType_audioFeedback      = 0x5,
    charType_audioPropAttr      = 0x6,
    charType_audioValAttr       = 0x7,
    charType_brightness         = 0x8,
    charType_cameraNightVision  = 0x9,
    charType_cameraPan          = 0xA,
    charType_cameraTilt         = 0xB,
    charType_cameraZoom         = 0xC,
    charType_coolingThreshold   = 0xD,
    charType_currentDoorState   = 0xE,
    charType_currentHeatCoolMode= 0xF,
    charType_currentHumidity    = 0x10,
    charType_currentTemperature = 0x11,
    charType_heatingThreshold   = 0x12,
    charType_hue                = 0x13,
    charType_identify           = 0x14,
    charType_inputVolume        = 0x15,
    charType_ipCameraStart      = 0x16,
    charType_ipCameraStop       = 0x17,
    charType_lockControlPoint   = 0x19,
    charType_lockAutoTimeout    = 0x1A,
    charType_lockLastAction     = 0x1C,
    charType_lockCurrentState   = 0x1D,
    charType_lockTargetState    = 0x1E,
    charType_logs               = 0x1F,
    charType_manufactuer        = 0x20,
    charType_modelName          = 0x21,
    charType_motionDetect       = 0x22,
    charType_serviceName        = 0x23,
    charType_obstruction        = 0x24,
    charType_on                 = 0x25,
    charType_outletInUse        = 0x26,
    charType_outputVolume       = 0x27,
    charType_rotationDirection  = 0x28,
    charType_rotationSpeed      = 0x29,
    charType_rtcpExtProp        = 0x2A,
    charType_rtcpVideoPayload   = 0x2B,
    charType_rtcpAudioPayload   = 0x2C,
    charType_rtcpAudioClock     = 0x2D,
    charType_rtcpProtocol       = 0x2E,
    charType_saturation         = 0x2F,
    charType_serialNumber       = 0x30,
    charType_srtpCyptoSuite     = 0x31,
    charType_targetDoorState    = 0x32,
    charType_targetHeatCoolMode = 0x33,
    charType_targetHumidity     = 0x34,
    charType_targetTemperature  = 0x35,
    charType_temperatureUnit    = 0x36,
    charType_version            = 0x37,
    charType_videoCodexName     = 0x38,
    charType_videoCodexPara     = 0x39,
    charType_videoMirror        = 0x3A,
    charType_videoPropAttr      = 0x3B,
    charType_videoRotation      = 0x3C,
    charType_videoValAttr       = 0x3D,
    
    charType_firmwareRevision   = 0x52,
    charType_hardwareRevision   = 0x53,
    charType_softwareRevision   = 0x54,
    
    charType_reachable          = 0x63,
    
    charType_airParticulateDensity = 0x64,
    charType_airParticulateSize = 0x65,
    charType_airQuality         = 0x95,
    charType_carbonDioxideDetected = 0x92,
    charType_carbonMonoxideDetected = 0x69,
    charType_carbonDioxideLevel = 0x93,
    charType_carbonMonoxideLevel = 0x90,
    charType_carbonDioxidePeakLevel = 0x94,
    charType_carbonMonoxidePeakLevel = 0x91,
    charType_smokeDetected      = 0x76,
    
    charType_alarmCurrentState  = 0x66,
    charType_alarmTargetState   = 0x67,
    charType_batteryLevel       = 0x68,
    charType_contactSensorState = 0x6A,
    charType_holdPosition       = 0x6F,
    charType_leakDetected       = 0x70,
    charType_occupancyDetected  = 0x71,
    
    charType_currentAmbientLightLevel = 0x6B,
    charType_currentHorizontalTiltAngle = 0x6C,
    charType_targetHorizontalTiltAngle = 0x7B,
    charType_currentPosition    = 0x6D,
    charType_targetPosition     = 0x7C,
    charType_currentVerticalTiltAngle = 0x6E,
    charType_targetVerticalTiltAngle = 0x7D,
    
    charType_positionState      = 0x72,
    charType_programmableSwitchEvent = 0x73,
    charType_programmableSwitchOutputState = 0x74,
    
    charType_sensorActive       = 0x75,
    charType_sensorFault        = 0x77,
    charType_sensorJammed       = 0x78,
    charType_sensorLowBattery   = 0x79,
    charType_sensorTampered     = 0x7A,
    charType_sensorChargingState= 0x8F,

    serviceType_accessoryInfo      = 0x3E,
    serviceType_camera             = 0x3F,
    serviceType_fan                = 0x40,
    serviceType_garageDoorOpener   = 0x41,
    serviceType_lightBulb          = 0x43,
    serviceType_lockManagement     = 0x44,
    serviceType_lockMechanism      = 0x45,
    serviceType_microphone         = 0x46,
    serviceType_outlet             = 0x47,
    serviceType_speaker            = 0x48,
    serviceType_switch             = 0x49,
    serviceType_thermostat         = 0x4A,
    
    serviceType_alarmSystem        = 0x7E,
    serviceType_bridgingState      = 0x62,
    serviceType_carbonMonoxideSensor = 0x7F,
    serviceType_contactSensor      = 0x80,
    serviceType_door               = 0x81,
    serviceType_humiditySensor     = 0x82,
    serviceType_leakSensor         = 0x83,
    serviceType_lightSensor        = 0x84,
    serviceType_motionSensor       = 0x85,
    serviceType_occupancySensor    = 0x86,
    serviceType_smokeSensor        = 0x87,
    serviceType_programmableSwitch_stateful = 0x88,
    serviceType_programmableSwitch_stateless = 0x89,
    serviceType_temperatureSensor  = 0x8A,
    serviceType_window             = 0x8B,
    serviceType_windowCover        = 0x8C,
    serviceType_airQualitySensor   = 0x8C,
    serviceType_securityAlarm      = 0x8E,
    serviceType_charging           = 0x8F,
    
    serviceType_battery            = 0x96,
    serviceType_carbonDioxideSensor= 0x97,
	serviceType_protocolInformation= 0xA2
    
} charType;
typedef enum {
    unit_none = 0,
    unit_celsius,
    unit_percentage,
    unit_arcDegree,
	unit_lux,
	unit_seconds
} unitType;

enum {
    permission_read = 1,
    permission_write = 1 << 1,
    permission_notify = 1 << 2  //Notify = Accessory will notice the controller
};

class characteristic {
private:
   void initJsonCharacteristic(void);
protected:
   pthread_mutex_t mtx;
 
   jsoncons::ojson json_characteristic;
   int aid;
   int iid;
   charType type;
   int permission;
   unitType unit;

   set_value_callback_routine set_value_callback;
public:
    characteristic(_supla_int64_t aid, int iid, charType type, int permission, unitType unit)
	: aid(aid), iid(iid), type(type), permission(permission), unit(unit)
	{  
	   pthread_mutex_init(&mtx, NULL);
	   initJsonCharacteristic(); 
	}
	virtual ~characteristic() {
	   pthread_mutex_destroy(&mtx);
	}

	bool writable(void);
    bool notifiable(void);
	charType getType(void);
	int getIid(void);
	jsoncons::ojson describe(void);
	jsoncons::ojson describeValue(void);

	virtual void setValue(std::string value) = 0;

	void setCallback(set_value_callback_routine callback) {
		this->set_value_callback = callback;
	}
	double showDecimals(const double& x, const int& numDecimals);
	void notifySupla(void);
	void getAccessoryId(void);
};

class floatCharacteristic: public characteristic {
private:
   float value;
   const float minValue;
   const float maxValue;
   const float step;
   const unitType unit;
public:
   floatCharacteristic(int aid,
     int iid, charType type, int permission, float minVal,
     float maxVal, float step, unitType unit)
   : characteristic(aid, iid, type, permission, unit), minValue(minVal),
   maxValue(maxVal), step(step), unit(unit)
   { 
		json_characteristic["format"] = "float";
		json_characteristic["minValue"] = minValue;
		json_characteristic["maxValue"] = maxValue;
		json_characteristic["minStep"] = showDecimals(step, 1);
   }

   void setValue(float value);
   virtual void setValue(std::string str) {

   }
};

class intCharacteristic: public characteristic {
private:
  const int minValue;
  const int maxValue;
  const int step;
  const unitType unit;
  int value;

public:
  intCharacteristic(int aid, int iid, charType type, int permission, int minVal,
	int maxVal, int step, unitType unit, int value)
	: characteristic(aid, iid, type, permission, unit), minValue(minVal), maxValue(maxVal),
	step(step), unit(unit), value(value)
	{ 
	   json_characteristic["format"] = "int";
	   json_characteristic["minValue"] = minValue;
	   json_characteristic["maxValue"] = maxValue;
	   json_characteristic["minStep"] = step;
	}
    
  void setValue(int value);
  int getValue(void);
  virtual void setValue(std::string str) {

     }
};

class uint8Characteristic: public characteristic {
private:
  const int minValue;
  const int maxValue;
  const int step;
  const unitType unit;
  int value;

  jsoncons::json valid_values;

public:
  uint8Characteristic(int aid, int iid, charType type, int permission, int minVal,
	int maxVal, int step, unitType unit, int value)
	: characteristic(aid, iid, type, permission, unit), minValue(minVal), maxValue(maxVal),
	step(step), unit(unit), value(value)
	{
	   json_characteristic["format"] = "uint8";
	   valid_values = jsoncons::json::make_array();

	   json_characteristic["minValue"] = minValue;
	   json_characteristic["maxValue"] = maxValue;
	   json_characteristic["minStep"] = step;
	}

  void setValidValue(uint8_t value);
  void setValue(uint8_t value);
  uint8_t getValue(void);
  virtual void setValue(std::string str) {
	  uint8_t newValue = atoi(str.c_str());

	  if (value != newValue) {
	    setValue(newValue);
	    if (set_value_callback)
	  	  set_value_callback(this, &this->aid);
	  }

   }
};


class stringCharacteristic: public characteristic {
private:
  const unsigned short maxLength;
  std::string value;

public:
    stringCharacteristic(int aid, int iid, charType type, int permission, std::string value,
	  unsigned short maxLen = HOMEKIT_STRING_DEFAULT_LENGTH)
	  : characteristic(aid, iid, type, permission, unit_none), maxLength(maxLen), value(value)
	  { 
    	setValue(value);
	    json_characteristic["format"] = "string"; 
		if (maxLength != HOMEKIT_STRING_DEFAULT_LENGTH)
		  json_characteristic["maxLen"] = maxLength;
	  }
	
    virtual void setValue(std::string str) {
    	pthread_mutex_lock(&mtx);
    	  this->value = value;
    	  if (permission & permission_read) {
    	    json_characteristic["value"] = value;
    	  }
    	  pthread_mutex_unlock(&mtx);
       }
};

class boolCharacteristic: public characteristic {
private:
    bool value;
public:
    boolCharacteristic(int aid, int iid, charType type, int permission, bool value)
	: characteristic(aid, iid, type, permission, unit_none), value(value)
	{
    	setValue(value);
    	json_characteristic["format"] = "bool";
	}
	
	void setValue(bool value);
	bool getValue(void);

	virtual void setValue(std::string str) {
	  bool newValue = (strncmp("true", str.c_str(), 4)==0)||(strncmp("1", str.c_str(), 1)==0);
	  if (value != newValue) {
		  setValue(newValue);
		  	  if (set_value_callback)
		  		  set_value_callback(this, &this->aid);
	  }

	}
}; 

class identityCharacteristic: public characteristic {
private:
    identify_callback_routine callback;
    bool value;
public:
    identityCharacteristic(int aid, int iid, charType type, int permission,
    		identify_callback_routine callback, bool value)
	: characteristic(aid, iid, type, permission, unit_none), callback(callback), value(value)
	{

    	json_characteristic["format"] = "bool";
	}

	virtual void setValue(std::string str) {
    	if (callback){
				callback(this, &this->aid);
			}
	   }
};

class service {
private:
	uint16_t iid;
  bool primary;
  bool hidden;
  uint8_t uuid;
  void* characteristics;
  void characteristics_clean(void);
  static char characteristics_delcnd(void *ptr);
  static char characteristics_findcmptype(void *ptr, void *type);
  static char characteristics_findcmpid(void *ptr, void *id);
public:
  service(uint16_t iid, uint8_t uuid);
  ~service();
  
  characteristic* getCharacteristicByType(charType type);
  characteristic* getCharacteristicById(int iid);
  void add_characteristic(characteristic* c);
  
  jsoncons::ojson describe(void);
  void describe_characteristics(jsoncons::ojson& array);

  uint8_t getType();
  uint16_t getId();

  void setPrimary(bool value);
};

class accessory {
private:
  int id;
  int currentUUID;

  void* services;
  void services_clean(void);
  static char services_delcnd(void *ptr);
  static char services_findcmpid(void *ptr, void *id);
  static char services_findcmptype(void *ptr, void *type);

public:
  accessory(int id);
  ~accessory();
  service* getServiceById(uint16_t id);
  service* getServiceByType(uint8_t type);
  characteristic* getCharacteristicById(uint16_t id);
  void add_service(service* s);

  jsoncons::ojson describe(void);
  void describe_characteristics(jsoncons::ojson& array);

  uint8_t getType();

  int getId();
  int getNextUUID(void);
};


#endif

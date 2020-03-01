/*
 * homekit_configuration.h
 *
 *  Created on: 29 lut 2020
 *      Author: beku
 */

#ifndef HOMEKIT_CONFIGURATION_H_
#define HOMEKIT_CONFIGURATION_H_

#include <string>

class Configuration {
private:
	std::string deviceName;
	std::string devicePassword;
	std::string deviceIdentity;
	std::string manufacturerName;
	std::string deviceUUID;
    std::string controllerRecordsAddress;

 public:
  static Configuration& Instance() {
    static Configuration myInstance;
    return myInstance;
  }

  Configuration(Configuration const&) = delete;             // Copy construct
  Configuration(Configuration&&) = delete;                  // Move construct
  Configuration& operator=(Configuration const&) = delete;  // Copy assign
  Configuration& operator=(Configuration &&) = delete;      // Move assign

  std::string getDeviceName(void) { return this->deviceName; }
  std::string getDevicePassword(void) { return this->devicePassword; }
  std::string getDeviceIdentity(void) { return this->deviceIdentity; }
  std::string getManufacturerName(void) { return this->manufacturerName; }
  std::string getDeviceUUID(void) { return this->deviceUUID; }
  std::string getControllerRecordsAddress(void) {return this->controllerRecordsAddress;}

  void setDeviceName(std::string value) { this->deviceName = value;}
  void setDevicePassword(std::string value) {this->devicePassword = value;}
  void setDeviceIdentity(std::string value) {this->deviceIdentity = value;}
  void setManufacturerName(std::string value) {this->manufacturerName = value;}
  void setDeviceUUID(std::string value) { this->deviceUUID = value; }
  void setControllersRecordsAddress(std::string value) {
	  this->controllerRecordsAddress = value;
  }
 protected:
  Configuration() {
	  this->deviceName = "Supla HomeKit Bridge";
	  this->devicePassword = "123-44-321";
	  this->deviceIdentity = "11:10:34:23:51:12";
	  this->manufacturerName = "Beku";
	  this->deviceUUID = "62F47751-8F26-46BF-9552-8F4238E67D60";
	  this->controllerRecordsAddress =  "./PHK_controller";
  }
  ~Configuration() {}

};

#endif /* HOMEKIT_CONFIGURATION_H_ */

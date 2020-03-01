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
 
#include "homekit_classes.h"
#include "supla-client-lib/safearray.h"
#include <string>

void characteristic::notifySupla(void) {
	if (set_value_callback) {
		set_value_callback(this, &this->aid);
	}
}

double characteristic::showDecimals(const double& x, const int& numDecimals)
 {
       int y=x;
       double z=x-y;
       double m=pow(10,numDecimals);
       double q=z*m;
       double r=round(q);

       return static_cast<double>(y)+(1.0/m)*r;
   }


void characteristic::initJsonCharacteristic() {
  


  json_characteristic["iid"] = iid;

  char hexString[4*sizeof(int)+1];
  sprintf(hexString,"%X", this->type);
  std::string tpt(hexString);

  json_characteristic["type"] =  tpt;
  json_characteristic["format"] = "generic";
     
  jsoncons::json perms = jsoncons::json::make_array();
  if (permission & permission_read) perms.push_back("pr");
  if (permission & permission_write) perms.push_back("pw");
  if (permission & permission_notify) perms.push_back("ev");
  
  json_characteristic["perms"] = perms;
  if (permission & permission_read)
    {
  	  if (permission & permission_notify)
  		  json_characteristic["ev"] = true;
  	  else
  		  json_characteristic["ev"] = false;
    };

  
  if (unit != unit_none) {
    switch (unit) {
	   case unit_celsius: json_characteristic["unit"] = "celsius"; break;
       case unit_percentage: json_characteristic["unit"] = "percentage"; break;
       case unit_arcDegree: json_characteristic["unit"] = "arcdegress"; break;
	   case unit_lux: json_characteristic["unit"] = "lux"; break;
	   case unit_seconds: json_characteristic["unit"] = "seconds"; break;
	   case unit_none:
	   default: break;
	}
  }





}	

void characteristic::setValue(std::string value) {

}

jsoncons::ojson characteristic::describeValue(void) {
	jsoncons::ojson result;

	try
	{
		result["aid"] = this->aid;
		result["iid"] = this->iid;
		result["value"] = (jsoncons::ojson)this->json_characteristic["value"];
	} catch (std::exception& ) {

	}
	return result;
}

jsoncons::ojson characteristic::describe(void) {
  return this->json_characteristic;
}

charType characteristic::getType(void) { return this->type; }
int characteristic::getIid(void) { return this->iid; }

bool characteristic::writable(void) { 
  return (this->permission & permission_write);
}




bool characteristic::notifiable(void) {
  return (this->permission & permission_notify);
}

bool boolCharacteristic::getValue(void) { return this->value; }

void boolCharacteristic::setValue(bool value) {
  pthread_mutex_lock(&mtx);
  this->value = value;
  if (permission & permission_read) {
    json_characteristic["value"] = value;
  } 
  pthread_mutex_unlock(&mtx);
}

void floatCharacteristic::setValue(float value) {
  pthread_mutex_lock(&mtx);
  this->value = value;
  if (permission & permission_read) {
    json_characteristic["value"] = showDecimals(value, 1);
  }
  pthread_mutex_unlock(&mtx);
}

int intCharacteristic::getValue(void) {
	return this->value;
}

void intCharacteristic::setValue(int value) {
  pthread_mutex_lock(&mtx);
  this->value = value;
  if (permission & permission_read) {
    json_characteristic["value"] = value;
  }
  pthread_mutex_unlock(&mtx);
}

uint8_t uint8Characteristic::getValue(void) {
	return this->value;
}

void uint8Characteristic::setValidValue(uint8_t value) {
	this->valid_values.push_back(value);
	json_characteristic["valid_values"] = this->valid_values;
}

void uint8Characteristic::setValue(uint8_t value) {
  pthread_mutex_lock(&mtx);
  this->value = value;
  if (permission & permission_read) {
    json_characteristic["value"] = value;
  }
  pthread_mutex_unlock(&mtx);
}

accessory::accessory(int id) {
	this->id = id;
	this->services = safe_array_init();
	/* six first reserved to accessory info characteristics*/
	this->currentUUID = 7;
}
char accessory::services_delcnd(void *ptr) {
  delete (service *)ptr;
  return 1;
}
void accessory::services_clean(void) {
	safe_array_lock(services);
	safe_array_clean(services, services_delcnd);
	safe_array_unlock(services);
}

accessory::~accessory(void) {
	services_clean();
	safe_array_free(services);
}

int accessory::getId() { return this->id; }

char accessory::services_findcmpid(void *ptr, void* id) {
  return ((service *)ptr)->getId() == *((uint16_t*)id) ? 1 : 0;
}

char accessory::services_findcmptype(void *ptr, void *type) {
  return ((service *)ptr)->getType() == *((uint8_t*)type) ? 1 : 0;
}

service* accessory::getServiceById(uint16_t id) {
	return (service*)safe_array_findcnd(services, services_findcmpid, &id);
}

characteristic* accessory::getCharacteristicById(uint16_t id) {

	safe_array_lock(services);

	for (int a = 0; a < safe_array_count(services); a++) {
		service *c =
			   static_cast<service*>(safe_array_get(services, a));
		if (c) {
			characteristic* result = c->getCharacteristicById(id);
			if (result)
				{
				safe_array_unlock(services);
				return result;
		}
		}
	}

	safe_array_unlock(services);
	return NULL;
}

service* accessory::getServiceByType(uint8_t type) {
	return (service*)safe_array_findcnd(services, services_findcmptype, &type);
}

void accessory::add_service(service* service) {

	safe_array_lock(services);

	  if (getServiceById(service->getId()) == NULL) {
	    if (service != NULL && safe_array_add(services, service) == -1) {
	      delete service;
	      service = NULL;
	    }
	  }

	 safe_array_unlock(services);
}

int accessory::getNextUUID() { return ++this->currentUUID; }

void accessory::describe_characteristics(jsoncons::ojson& array) {

  for (int a = 0; a < safe_array_count(services); a++) {
	 service *c =
	   static_cast<service*>(safe_array_get(services, a));

	   if (c) {
		 c->describe_characteristics(array);
	   }
  };
}

jsoncons::ojson accessory::describe(void) {

  jsoncons::ojson result;
  jsoncons::json j_services = jsoncons::json::make_array();

  result["aid"] = this->id;

  for (int a = 0; a < safe_array_count(services); a++) {
	 service *c =
	   static_cast<service*>(safe_array_get(services, a));

	   if (c) {
		 j_services.push_back(c->describe());
	   }
  };

  if (j_services.size() > 0)
	result["services"] = j_services;

  return result;
}

service::service(uint16_t iid, uint8_t uuid) {
  this->characteristics = safe_array_init();
  this->uuid = uuid;
  this->iid = iid;
  this->hidden = false;
  this->primary = false;
}


void service::setPrimary(bool value) {
	this->primary = value;
}

char service::characteristics_delcnd(void *ptr) {
  delete (characteristic *)ptr;
  return 1;
}

void service::characteristics_clean(void) {
  safe_array_lock(characteristics);
  safe_array_clean(characteristics, characteristics_delcnd);
  safe_array_unlock(characteristics);
}

service::~service(void) {
  characteristics_clean();
  safe_array_free(characteristics);
}

uint8_t service::getType() { return this->uuid; }
uint16_t service::getId() { return this->iid; }

char service::characteristics_findcmpid(void *ptr, void *id) {
  return ((characteristic *)ptr)->getIid() == *((int *)id) ? 1 : 0;
}

char service::characteristics_findcmptype(void *ptr, void *type) {
  return ((characteristic *)ptr)->getType() == *((charType *)type) ? 1 : 0;
}

characteristic* service::getCharacteristicByType(charType type) {
  return (characteristic *)safe_array_findcnd(characteristics, 
    characteristics_findcmptype, &type); 
}
characteristic* service::getCharacteristicById(int iid) {
  return (characteristic *)safe_array_findcnd(characteristics, 
    characteristics_findcmpid, &iid); 
}



void service::add_characteristic(characteristic* c) {
  
  safe_array_lock(characteristics);

  if (getCharacteristicById(c->getIid()) == NULL) {
    if (c != NULL && safe_array_add(characteristics, c) == -1) {
      delete c;
      c = NULL;
    }
  }
  
  safe_array_unlock(characteristics);
}

void service::describe_characteristics(jsoncons::ojson& array) {
  safe_array_lock(characteristics);

  for (int a = 0; a < safe_array_count(characteristics); a++) {
    characteristic *c =
        static_cast<characteristic*>(safe_array_get(characteristics, a));

	if (c && c->notifiable()) {
	  array.push_back(c->describeValue());
	}
  };

  safe_array_unlock(characteristics);
}

jsoncons::ojson service::describe(void) {
  
  safe_array_lock(characteristics);
  
  jsoncons::ojson result;
  
  char temp[8];
  snprintf(temp, 8, "%X", this->uuid);

  std::string tpt(temp, 2);
  result["iid"] = this->iid;
  result["type"] = tpt;
  result["primary"] = this->primary;
  result["hidden"] = this->hidden;

  jsoncons::json chars = jsoncons::json::make_array();
  
  for (int a = 0; a < safe_array_count(characteristics); a++) {
    characteristic *c =
        static_cast<characteristic*>(safe_array_get(characteristics, a));
    
	if (c) {
	  chars.push_back(c->describe());
	}
  };
  
  if (chars.size() > 0)
    result["characteristics"] = chars;
    


  safe_array_unlock(characteristics);
  
  return result;
}

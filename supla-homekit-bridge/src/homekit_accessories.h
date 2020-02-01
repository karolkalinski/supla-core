#ifndef HOMEKIT_ACCESSORIES_H
#define HOMEKIT_ACCESSORIES_H

#include "supla-client-lib/safearray.h"
#include "homekit_classes.h"
#include "client_device.h"



class homekit_accessories {
  private:
    void* arr;

    accessory* add_accessory_gateway_lock(int accessoryId, service* info); /* elektrozaczep -> switch ? */

    accessory* add_accessory_gate(int accessoryId, service* info); /* brama wjazdowa -> garage door opener */
	accessory* add_accessory_garage_door(int accessoryId, service* info); /* drzwi garażowe -> garage_door_opener */
	accessory* add_accessory_thermometer(accessory* accessory); /* temperatur sensor */
	accessory* add_accessory_humidity(accessory* accessory); /* humidity sensor */
	accessory* add_accessory_door_lock(int accessoryId, service* info); /* zamek w drzwiach  -> lock mechanizm */
	accessory* add_accessory_rollershutter(int accessoryId, service* info); /* window covering */
	accessory* add_accessory_power_switch(accessory* accessory, set_value_callback_routine callback); /* switch */
	accessory* add_accessory_light_switch(accessory* accessory, set_value_callback_routine callback); /* lightswitch */
	accessory* add_accessory_dimmer(int accessoryId, service* info); /* lightswitch */
	accessory* add_accessory_rgb_lightning(int accessoryId, service* info); /* lightswitch */
	accessory* add_accessory_dimmer_and_rgb_lightning(int accessoryId, service* info); /* lightswitch */
    accessory* add_accessory_sensornonc(int accessoryId, service* info); /* occupancy sensor */
	accessory* add_accessory_thermostat(int accessoryId, service* info); /* thermostat */
	accessory* add_accessory_bridge();
	
	accessory* new_accessory(int accessoryId, service* information_service);
	service* get_accessory_information_service(
		_supla_int64_t aid,
	    std::string firmware_revision, std::string manufacturer, 
		std::string model, std::string name, std::string serial_number, 
		identify_callback_routine identity_callback_routine);
  
   	void add_accessory_to_array(accessory* accessory);

  public:
    homekit_accessories();
	~homekit_accessories();
	
	void add_accessory_for_supla_channel(
	  client_device_channel* channel, 
	  identify_callback_routine identify_callback,
	  set_value_callback_routine value_callback);

	accessory* getAccessoryById(_supla_int64_t id);

	jsoncons::ojson describe(void);
};

#endif

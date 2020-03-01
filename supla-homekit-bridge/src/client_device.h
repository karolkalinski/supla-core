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

#ifndef CLIENT_DEVICE_H
#define CLIENT_DEVICE_H

#include <string.h>
#include "jsoncons/json.hpp"
#include "supla-client-lib/log.h"
#include "supla-client-lib/devicechannel.h"
#include "supla-client-lib/safearray.h"
#include "supla-client-lib/supla-client.h"
#include "supla-client-lib/lck.h"


typedef void (*identify_callback_routine)(void *ptr, void *user_param);
typedef void (*set_value_callback_routine)(void *ptr, void *user_param);

class client_device_channel : public supla_device_channel {
 private:
   char *Caption;
   char Sub_value[8];
   bool isHK;
   bool Online;

   std::string serial_number;
   std::string name;
   std::string model;
   std::string manufacturer;
   std::string firmware_revision;

 public:

  client_device_channel(int Id, int Number, int Type, int Func, int Param1,
                        int Param2, int Param3, char *TextParam1,
                        char *TextParam2, char *TextParam3, bool Hidden,
                        bool Online, char *Caption, int accessoryId);
   ~client_device_channel();
  
  bool getOnline();
  void setOnline(bool value);
  void setFunc(int value);

  void setSubValue(char sub_value[SUPLA_CHANNELVALUE_SIZE]);
  void getSubValue(char sub_value[SUPLA_CHANNELVALUE_SIZE]);
  char* getCaption(void);
  void notifyHomekit(void);

  void setHKValue(char value[SUPLA_CHANNELVALUE_SIZE]);
  bool isHidden(void);
  int getId(void);
  int getAccessoryId(void);
  std::string getSerialNumber(void);
  std::string getName(void);
  std::string getModel(void);
  std::string getManufacturer(void);
  std::string getFirmwareRevision(void);
};

class client_device_channels : public supla_device_channels {
 private:
	 int current_accessory_id = 0;
	 bool initialized;
 public:
   client_device_channels();
   ~client_device_channels();

   client_device_channel *add_channel(
                   int Id, int Number, int Type, int Func, 
                   int Param1,
                   int Param2, int Param3, char *TextParam1, char *TextParam2,
                   char *TextParam3, bool Hidden, bool Online, char *Caption);

   void set_channel_sub_value(int ChannelID,
                             char sub_value[SUPLA_CHANNELVALUE_SIZE]);

   client_device_channel *find_channel(int ChannelId);
   jsoncons::json describe(void);
   bool getInitialized(void);
   void setInitialized(bool value);
};

#endif

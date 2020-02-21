/*
 * channel.h
 *
 *  Created on: 18 lut 2020
 *      Author: beku
 */

#ifndef CHANNEL_H_
#define CHANNEL_H_

#include <cstring>
#include <string>
#include <vector>

#include "notification_loop.h"
#include "supla-client-lib/proto.h"
#include "supla-client-lib/safearray.h"
class channel {
 private:
  int channel_id;
  int channel_function;
  bool online;

  std::string caption;
  char value[SUPLA_CHANNELVALUE_SIZE];
  char sub_value[SUPLA_CHANNELVALUE_SIZE];
  std::vector<void*> notification_list;
  std::vector<void*> connection_change_list;
  bool value_changed(char first[SUPLA_CHANNELVALUE_SIZE],
                     char second[SUPLA_CHANNELVALUE_SIZE]);

 public:
  channel(int channel_id, int channel_function, std::string caption);
  virtual ~channel();

  void setValue(char value[SUPLA_CHANNELVALUE_SIZE]);
  void setSubValue(char value[SUPLA_CHANNELVALUE_SIZE]);
  void setCaption(std::string value);
  void setFunction(int value);
  void notify(void);

  void getValue(char value[SUPLA_CHANNELVALUE_SIZE]);
  void getSubValue(char value[SUPLA_CHANNELVALUE_SIZE]);
  std::string getCaption(void);
  int getFunction(void);
  int getChannelId(void);
  std::string getStringValue(int index);
  
  void add_notification_on_change(void* value);
  void add_notification_on_connection(void* value);
};

class channels {
 private:
  void* arr;

 public:
  channels();
  virtual ~channels();

  channel* add_channel(int channel_id, int channel_function,
                       std::string caption);
  channel* find_channel(int channel_id);
};

extern channels* chnls;

#endif /* CHANNEL_H_ */

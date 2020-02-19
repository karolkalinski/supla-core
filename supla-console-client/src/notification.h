/*
 * notification.h
 *
 *  Created on: 18 lut 2020
 *      Author: beku
 */

#ifndef NOTIFICATION_H_
#define NOTIFICATION_H_

#include <string>
#include <vector>
 
#include "supla-client-lib/safearray.h"
#include "supla-client-lib/log.h"
#include "supla-client-lib/lck.h"
#include "crontab_parser.h"
#include "ccronexpr.h"

#include "channel.h"

#include <time.h>
#include <ctime>

enum enum_trigger { none, onchange, ontime };

typedef struct {
  int channelid;
  int index;
  bool wasIndexed;
} channel_index;

class notification {
private:
   enum_trigger trigger;
   std::string time;
   std::string condition;
   std::string device;
   std::string title;
   std::string message;
   std::string token;
   std::string user;
   time_t next;
   bool isChannelsSet;
   void* lck;
   std::vector<channel_index> channels;
   std::string notificationCmd;
   std::string buildNotificationCommand();
   bool isConditionSet(void);
public:
	notification();
	virtual ~notification();

	void setTrigger(enum_trigger value);
	void setTime(std::string time);
	void setCondition(std::string value);
	void setDevice(std::string value);
	void setTitle(std::string value);
	void setMessage(std::string value);
	void setToken(std::string value);
	void setUser(std::string value);

	enum_trigger getTrigger(void);
	std::string getTime(void);
	std::string getCondtion(void);
	std::string getDevice(void);
	std::string getTitle(void);
	std::string getMessage(void);
	bool setNextTime(time_t value);
    
	void notify(void);
	void setChannels(void);
	void set_channel_trigger(void);
	void notify_on_time_trigger(void);
};

class notifications {
private:
	void* arr;
	std::string user;
	std::string token;
public:
	notifications();
	virtual ~notifications();
	
	void add_notifiction(enum_trigger trigger, std::string time,
	  std::string condition, std::string device,
	  std::string title, std::string message, 
	  std::string token, std::string user);

	void setUser(std::string value);
	void setToken(std::string value);
	void handle();
};

extern notifications* ntfns;

#endif /* NOTIFICATION_H_ */

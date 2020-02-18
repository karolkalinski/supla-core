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
#include "channel.h"
#include "supla-client-lib/safearray.h"
#include "supla-client-lib/log.h"
#include "crontab_parser.h"
#include "ccronexpr.h"
#include <time.h>
#include <ctime>

enum enum_trigger { none, onchange, ontime };

class notification {
private:
   enum_trigger trigger;
   std::string time;
   std::string condition;
   std::string device;
   std::string title;
   std::string message;
   time_t next;

   std::string notificationCmd;

   std::string buildNotificationCommand(std::string token, std::string user);

public:
	notification();
	virtual ~notification();

	void setTrigger(enum_trigger value);
	void setTime(std::string time);
	void setCondition(std::string value);
	void setDevice(std::string value);
	void setTitle(std::string value);
	void setMessage(std::string value);

	enum_trigger getTrigger(void);
	std::string getTime(void);
	std::string getCondtion(void);
	std::string getDevice(void);
	std::string getTitle(void);
	std::string getMessage(void);
    bool setNextTime(time_t value);
    void sendNotification(std::string token, std::string user);
	bool isConditionSet(void);
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
	  std::string title, std::string message);

	void setUser(std::string value);
	void setToken(std::string value);

	void handle();
};

extern notifications* ntfns;

#endif /* NOTIFICATION_H_ */

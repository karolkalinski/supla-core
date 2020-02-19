/*
 * notification.cpp
 *
 *  Created on: 18 lut 2020
 *      Author: beku
 */

#include "notification.h"

struct channel_index {
  int channelid;
  uint8_t index;
}

notification::notification() {
	// TODO Auto-generated constructor stub
  this->condition = "";
  this->device = "";
  this->message = "";
  this->time = "";
  this->title = "";
  this->trigger = none;
  this->next =  std::time(0);
  this->notificationCmd = "";
  this->isChannelsSet = false;
  lck = lck_init();
}

notification::~notification() {
	lck_free(lck);
}

bool notification::setNextTime(time_t value) {
	 double sec = difftime(value, next);
	 this->next = value;
	 supla_log(LOG_DEBUG, "%s", ctime(&this->next));

	 return sec > 0;

}

std::string notification::buildNotificationCommand() {
	if (this->notificationCmd.length() == 0) {

		notificationCmd.append("curl -d ");
		notificationCmd.append("\"token=");
		notificationCmd.append(this->token);
		notificationCmd.append("&user=");
		notificationCmd.append(this->user);
		notificationCmd.append("&message=");
		notificationCmd.append(this->message);
		notificationCmd.append("\" ");
		notificationCmd.append("-H \"Content-Type: application/x-www-form-urlencoded\" ");
		notificationCmd.append("-X POST https://api.pushover.net/1/messages.json >  /dev/null 2>&1");
	}

	return this->notificationCmd;
}


void notification::notify(void) {
   lck_lock(lck);

   if (!isConditionSet()) return;
  
   std::string command = buildNotificationCommand();

   int commandResult = system(command.c_str());
   if (commandResult != 0) {
      supla_log(LOG_WARNING, "%s", command.c_str());
      supla_log(LOG_WARNING, "The command above failed with exist status %d",
        commandResult);
  }
  
  lck_unlock(lck);   
}
void notification::setChannelTrigger(void) {
  
  if (!isChannelsSet) return;
  
  for (auto channel_struct: this->channels) {
	  channel* ch = chnls->find_channel(channel_struct.channelid);
	  if (ch) ch->addNotification(this);
  }
}

void notification::setChannels(void) {
  if (isChannelsSet) return;
  
  if (this->condition.length() == 0) return false;
  
  std::string temp = this->condition;
  
  std::size_t start = temp.find("%channel_"); // 0
  while (start != std::string::npos) {
	  std::size_t end = temp.find("%", start + 1); // 11

	  std::string channelId = temp.substr(start + 9, end - start - 9);

	  auto vect = split(channelid, '_');
      
	  if (vect.size() > 1)
		channels.push_back({std::stoi(vect[0]), 0, false}); 	
      else 
		channels.push_back({std::stoi(vect[0]), std::stoi(vect[1]), true);
	  
	 
	  temp = temp.substr(end + 1);
	  start = temp.find("%channel_"); // 0
  }
  
  isChannelsSet = true;
}

bool notification::isConditionSet(void) {
  
  if (this->channels.size() == 0) return;
  
  for (auto channel_struct : this->channels) {
	  channel* chnl = chnls->find_channel(channel_struct.channelid);

	  if (!chnl)
      {
		supla_log(LOG_DEBUG, "channel %d not found", channel_struct.channelId);
		return 0;
	  }

	  std::string val = chnl->getStringValue(channel_struct.index);

	  if (channel_struct.wasIndexed)
		  replace_string_in_place(&temp, "%channel_" + 
	         std::to_string(channel_struct.channelid) + "_" + 
			 std::to_string(channel_struct.index) + "%", val);
      else
		  replace_string_in_place(&temp, "%channel_" + 
			std::to_string(channel_struct.channelid) +"%", val);
  };

  std::string command;
  command.append("echo ");
  command.append("\"");
  command.append(temp);
  command.append("\" ");
  command.append("| bc");

  std::string commandResult = get_command_output(command);
  if (commandResult.compare("0\n") == 0) return false;
  if (commandResult.compare("1\n") == 0) return true;

  if (commandResult != "") {
      supla_log(LOG_WARNING, "%s", temp.c_str());
      supla_log(LOG_WARNING, "The command above failed with exist status %d",
                commandResult);
  }

  return false;
}

void setToken(std::string value) {
  this->token = value;
}
void setUser(std::string value) {
  this->user = value;
}

void notification::notify_on_time_trigger(void) {
  if (this->time.length() == 0 ) continue;

  cron_expr expr;
  const char* err = NULL;
  memset(&expr, 0, sizeof(expr));
  cron_parse_expr(this->time.c_str(), &expr, &err);
  if (err) {
		supla_log(LOG_DEBUG, "error parsing crontab value %s", err);
	    continue;
  }
  time_t cur = time(NULL);
  time_t next = cron_next(&expr, cur);
  if (setNextTime(next)) notify();
}

void notification::setTrigger(enum_trigger value) {
	this->trigger = value;
}
	void notification::setTime(std::string time) {
		this->time = time;
	}
	void notification::setCondition(std::string value) {
		this->condition = value;
	}
	void notification::setDevice(std::string value) {
		this->device = value;
	}
	void notification::setTitle(std::string value) {
		this->title = value;
	}
	void notification::setMessage(std::string value) {
		this->message = value;
	}
	enum_trigger notification::getTrigger(void) {
		return this->trigger;
	}
	std::string notification::getTime(void) {
		return this->time;
	}
	std::string notification::getCondtion(void) {
		return this->condition;
	}
	std::string notification::getDevice(void) {
		return this->device;
	}
	std::string notification::getTitle(void) {
		return this->title;
	}
	std::string notification::getMessage(void) {
		return this->message;
	}

notifications::notifications() {
	arr = safe_array_init();
}

notifications::~notifications() {

	safe_array_lock(arr);
	for (int i = 0; i < safe_array_count(arr); i++) {
		notification* ntf = (notification*)safe_array_get(arr, i);
		if (ntf) delete ntf;
	}
	
	safe_array_unlock(arr);
	safe_array_free(arr);
}



void notifications::add_notifiction(
  enum_trigger trigger, std::string time,
  std::string condition, std::string device,
  std::string title, std::string message, std::string token, std::string user) {

  safe_array_lock(arr);

  notification* nt = new notification();

  nt->setCondition(condition);
  nt->setDevice(device);
  nt->setMessage(message);
  nt->setTime(time);
  nt->setTitle(title);
  nt->setTrigger(trigger);
  nt->setUser(user);
  nt->setToken(token);

  if (safe_array_add(arr, nt) == -1) {
	  delete nt;
  }

  safe_array_unlock(arr);
}

void notifications::setUser(std::string value) {
	this->user = value;
}

void notifications::setToken(std::string value) {
	this->token = value;
}

void notifications::handle() {

	safe_array_lock(arr);

	for (int i = 0; i < safe_array_count(arr); i++) {
		notification* ntf = (notification*)safe_array_get(arr, i);

		ntf->setChannels();
				
		switch (ntf->getTrigger()) {
			case none: continue;
			case onchange: {
				ntf->setChannelTrigger();
			}
			case ontime : {
				ntf->notifyOnTimeTrigger();
			} break;
		}
	}
	
	safe_array_unlock(arr);
}

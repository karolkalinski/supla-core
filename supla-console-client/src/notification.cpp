/*
 * notification.cpp
 *
 *  Created on: 18 lut 2020
 *      Author: beku
 */

#include "notification.h"

notification::notification() {
  this->condition = "";
  this->device = "";
  this->message = "";
  this->time = "";
  this->title = "";
  this->trigger = none;
  this->next = 0;  // std::time(0);
  this->notificationCmd = "";
  this->isChannelsSet = false;
  this->lastResult = false;
  this->reset = r_automatic;
  this->priority = 0;
  lck = lck_init();
}

notification::~notification() { lck_free(lck); }

bool notification::getLastResult(void) { return this->lastResult; }

void notification::setLastResult(bool value) { this->lastResult = value; }

bool notification::setNextTime(time_t value) {
  /*don't set on start */
  if (this->next == 0) this->next = value;

  double sec = difftime(value, next);
  this->next = value;
  return sec > 0;
}

time_t notification::getNextTime(void) { return this->next; }

void notification::setPriority(uint8_t value) { this->priority = value; }

uint8_t notification::getPriority(void) { return this->priority; }

std::string notification::buildNotificationCommand() {
  if (this->notificationCmd.length() == 0) {
    notificationCmd.append("curl -d ");
    notificationCmd.append("\"token=");
    notificationCmd.append(this->token);
    notificationCmd.append("&user=");
    notificationCmd.append(this->user);
    notificationCmd.append("&title=");
    notificationCmd.append(this->title);
    notificationCmd.append("&priority=");
    notificationCmd.append(std::to_string(this->priority));

    if (this->device.length() > 0) {
      notificationCmd.append("&device=");
      notificationCmd.append(this->device);
    }

    notificationCmd.append("&message=");
    notificationCmd.append(this->message);
    notificationCmd.append("\" ");
    notificationCmd.append(
        "-H \"Content-Type: application/x-www-form-urlencoded\" ");
    notificationCmd.append(
        "-X POST https://api.pushover.net/1/messages.json >  /dev/null 2>&1");
  }

  return this->notificationCmd;
}

void* execute_notification(void* vp) {
  std::string* sp = static_cast<std::string*>(vp);
  std::string command = *sp;
  delete sp;

  if (command.length() == 0) return NULL;

  int commandResult = system(command.c_str());

  if (commandResult != 0) {
    supla_log(LOG_WARNING, "%s", command.c_str());
    supla_log(LOG_WARNING, "The command above failed with exist status %d",
              commandResult);
  }

  return NULL;
}

std::string notification::getExecuteCmd(void) { return this->executeCmd; }

void notification::setExecuteCmd(std::string value) {
  this->executeCmd = value;
}

void notification::notify(void) {
  if (!isConditionSet()) return;

  lck_lock(lck);

  std::string* command = new std::string(buildNotificationCommand());
  void* vp = static_cast<void*>(command);

  pthread_t thread;
  pthread_create(&thread, NULL, execute_notification, vp);
  pthread_detach(thread);

  /* additional command execution */
  if (this->executeCmd.length() > 0) {
    std::string* executionCmd = new std::string(this->executeCmd);
    void* ec = static_cast<void*>(executionCmd);

    pthread_t thread_c;
    pthread_create(&thread_c, NULL, execute_notification, ec);
    pthread_detach(thread_c);
  }

  lck_unlock(lck);
}
void notification::set_on_change_connection_trigger(void) {
  if (!isChannelsSet) return;

  for (auto channel_struct : this->channels) {
    channel* ch = chnls->find_channel(channel_struct.channelid);

    if (ch) {
      ch->add_notification_on_connection((void*)this);
    } else
      supla_log(LOG_DEBUG,
                "set_channel_on_connection_trigger: channel %d not found",
                channel_struct.channelid);
  }
}

void notification::set_on_change_trigger(void) {
  if (!isChannelsSet) return;

  for (auto channel_struct : this->channels) {
    channel* ch = chnls->find_channel(channel_struct.channelid);

    if (ch) {
      ch->add_notification_on_change((void*)this);
    } else
      supla_log(LOG_DEBUG,
                "set_channel_on_change_trigger: channel %d not found",
                channel_struct.channelid);
  }
}

void notification::setChannels(void) {
  if (isChannelsSet) return;
  if (this->condition.length() == 0) return;

  supla_log(LOG_DEBUG, "setting channels list for notification... %s",
            this->condition.c_str());
  try {
    std::string temp = this->condition;

    std::size_t start = temp.find("%channel_");  // 0
    while (start != std::string::npos) {
      std::size_t end = temp.find("%", start + 1);  // 11

      std::string channelId = temp.substr(start + 9, end - start - 9);

      auto vect = split(channelId, '_');

      if (vect.size() == 1) {
        channels.push_back({std::stoi(vect[0]), 0, false});
        supla_log(LOG_DEBUG, "setting notification for channel %s",
                  vect[0].c_str());
      } else {
        channels.push_back({std::stoi(vect[1]), std::stoi(vect[0]), true});
        supla_log(LOG_DEBUG,
                  "setting notification for channel %s with index %s",
                  vect[1].c_str(), vect[0].c_str());
      }

      temp = temp.substr(end + 1);
      start = temp.find("%channel_");  // 0
    }

    isChannelsSet = true;

    supla_log(LOG_DEBUG, "setting channels list for notification...OK");
  } catch (const std::exception&) {
    supla_log(LOG_WARNING, "syntax error in condition %s",
              this->condition.c_str());
  }
}

bool notification::isConditionSet(void) {
  if (this->channels.size() == 0) return false;

  std::string temp = this->condition;

  for (auto channel_struct : this->channels) {
    channel* chnl = chnls->find_channel(channel_struct.channelid);

    if (!chnl) return false;

    std::string val;

    if (this->trigger == onchange || this->trigger == ontime)
      val = chnl->getStringValue(channel_struct.index);
    else if (this->trigger == onconnection)
      val = std::to_string(chnl->getOnline());
    else
      return false;

    if (channel_struct.wasIndexed)
      replace_string_in_place(
          &temp,
          "%channel_" + std::to_string(channel_struct.index) + "_" +
              std::to_string(channel_struct.channelid) + "%",
          val);
    else
      replace_string_in_place(
          &temp, "%channel_" + std::to_string(channel_struct.channelid) + "%",
          val);
  };

  std::string command;
  command.append("echo ");
  command.append("\"");
  command.append(temp);
  command.append("\" ");
  command.append("| bc");

  supla_log(LOG_DEBUG, "check command %s", command.c_str());

  std::string commandResult = get_command_output(command);
  if (commandResult.compare("0\n") == 0) {
    this->lastResult = false;
    return false;
  }

  if (commandResult.compare("1\n") == 0) {
    if (this->reset == r_automatic) {
      if (this->lastResult != true) {
        this->lastResult = true;
        return true;
      } else
        return false;
    } else {
      return true;
    }
  }

  if (commandResult != "") {
    supla_log(LOG_WARNING, "%s", temp.c_str());
    supla_log(LOG_WARNING, "The command above failed with exist status %d",
              commandResult);
  }

  this->lastResult = false;
  return false;
}

void notification::setToken(std::string value) { this->token = value; }
void notification::setUser(std::string value) { this->user = value; }

void notification::notify_on_time_trigger(void) {
  if (this->time.length() == 0) return;

  time_t cur = std::time(NULL);
  if (difftime(this->next, cur) > 0) return;

  cron_expr expr;
  const char* err = NULL;
  memset(&expr, 0, sizeof(expr));
  cron_parse_expr(this->time.c_str(), &expr, &err);

  if (err) {
    supla_log(LOG_DEBUG, "error parsing crontab value %s", err);
    return;
  }

  time_t next = cron_next(&expr, cur);
  if (setNextTime(next)) {
    supla_log(LOG_DEBUG, "sending on time notification %s", this->time.c_str());
    notify();
  }
}

void notification::setReset(enum_reset value) { this->reset = value; }
void notification::setTrigger(enum_trigger value) { this->trigger = value; }
void notification::setTime(std::string time) { this->time = time; }
void notification::setCondition(std::string value) { this->condition = value; }
void notification::setDevice(std::string value) { this->device = value; }
void notification::setTitle(std::string value) { this->title = value; }
void notification::setMessage(std::string value) { this->message = value; }
enum_trigger notification::getTrigger(void) { return this->trigger; }
std::string notification::getTime(void) { return this->time; }
std::string notification::getCondtion(void) { return this->condition; }
std::string notification::getDevice(void) { return this->device; }
std::string notification::getTitle(void) { return this->title; }
std::string notification::getMessage(void) { return this->message; }

notifications::notifications() { arr = safe_array_init(); }

notifications::~notifications() {
  safe_array_lock(arr);
  for (int i = 0; i < safe_array_count(arr); i++) {
    notification* ntf = (notification*)safe_array_get(arr, i);
    if (ntf) delete ntf;
  }
  safe_array_unlock(arr);
  safe_array_free(arr);
}

void notifications::add_notifiction(enum_trigger trigger, std::string time,
                                    std::string condition, std::string device,
                                    std::string title, std::string message,
                                    std::string token, std::string user,
                                    enum_reset reset, std::string command,
                                    uint8_t priority) {
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
  nt->setReset(reset);
  nt->setExecuteCmd(command);
  nt->setPriority(priority);

  if (safe_array_add(arr, nt) == -1) {
    delete nt;
  }

  safe_array_unlock(arr);
}

void notifications::setUser(std::string value) { this->user = value; }
void notifications::setToken(std::string value) { this->token = value; }

void notifications::handle() {
  safe_array_lock(arr);

  for (int i = 0; i < safe_array_count(arr); i++) {
    notification* ntf = (notification*)safe_array_get(arr, i);

    if (ntf->getTrigger() == none) continue;

    ntf->setChannels();

    switch (ntf->getTrigger()) {
      case onchange: {
        ntf->set_on_change_trigger();
      } break;
      case ontime: {
        ntf->notify_on_time_trigger();
      } break;
      case onconnection: {
        ntf->set_on_change_connection_trigger();
      }
    }
  }

  safe_array_unlock(arr);
}

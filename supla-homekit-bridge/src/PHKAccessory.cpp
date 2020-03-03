//
//  PHKAccessory.c
//  Workbench
//
//  Created by Wai Man Chan on 9/27/14.
//
//

#include "PHKAccessory.h"

#include "Configuration.h"
#include "globals.h"

const char hapJsonType[] = "application/hap+json";
const char pairingTlv8Type[] = "application/pairing+tlv8";

void *announce(void *info) {
  broadcastInfo *_info = (broadcastInfo *)info;

  void *sender = _info->sender;

  char *reply = new char[4086];

  int len = snprintf(reply, 4086,
                     "EVENT/1.0 200 OK\r\nContent-Type: "
                     "application/hap+json\r\nContent-Length: %u\r\n\r\n%s",
                     strlen(_info->desc), _info->desc);

  broadcastMessage(sender, reply, len);

  delete[] reply;

  if (_info->desc) free(_info->desc);

  delete _info;
  return NULL;
}

void handleAccessory(const char *request, unsigned int requestLen, char **reply,
                     unsigned int *replyLen, connectionInfo *sender) {
  supla_log(LOG_DEBUG, "Received http request: %s", request);

  int index = 5;
  char method[5];
  method[4] = 0;
  memcpy(method, request, 4);

  if (method[3] == ' ') {
    method[3] = 0;
    index = 4;
  };

  char path[HTTP_PATH_MAX_LENGTH];
  int i;
  for (i = 0; i < HTTP_PATH_MAX_LENGTH && request[index] != ' '; i++, index++) {
    path[i] = request[index];
  }

  path[i] = 0;

  supla_log(LOG_DEBUG, "Http request path: %s", path);

  const char *dataPtr = request;

  while (true) {
    dataPtr = &dataPtr[1];
    if (dataPtr[0] == '\r' && dataPtr[1] == '\n' && dataPtr[2] == '\r' &&
        dataPtr[3] == '\n')
      break;
  }

  dataPtr += 4;

  char *replyData = NULL;
  unsigned short replyDataLen = 0;

  int statusCode = 0;

  const char *protocol = HTTP_PROTOCOL_1_1;
  const char *returnType = hapJsonType;

  if (strcmp(path, "/accessories") == 0) {
    supla_log(LOG_DEBUG, "Controller asked for accesories list");

    jsoncons::json acc = accessories->describe();
    std::string desc;
    acc.dump(desc);

    replyDataLen = desc.length();
    replyData = new char[replyDataLen + 1];

    memcpy(replyData, desc.c_str(), replyDataLen);

    replyData[replyDataLen] = 0;

    statusCode = HTTP_STATUS_OK;
  } else if (strcmp(path, "/pairings") == 0) {
    PHKNetworkMessage msg(request);

    supla_log(LOG_DEBUG, "Controller asked for pairings list %d",
              *msg.data.dataPtrForIndex(0));

    if (*msg.data.dataPtrForIndex(0) == 3) {
      supla_log(LOG_DEBUG, "pairings: add new user");
      PHKKeyRecord controllerRec;

      memcpy(controllerRec.publicKey, msg.data.dataPtrForIndex(3), 32);
      memcpy(controllerRec.controllerID, msg.data.dataPtrForIndex(1), 36);

      /* addinh new controller key */
      addControllerKey(controllerRec);

      PHKNetworkMessageDataRecord drec;
      drec.activate = true;
      drec.data = new char[1];
      *drec.data = 2;
      drec.index = 6;
      drec.length = 1;

      PHKNetworkMessageData data;
      data.addRecord(drec);
      data.rawData((const char **)&replyData, &replyDataLen);
      returnType = pairingTlv8Type;
      statusCode = HTTP_STATUS_OK;

    } else {
      supla_log(LOG_DEBUG, "pairings: delete user");

      PHKKeyRecord controllerRec;
      memcpy(controllerRec.controllerID, msg.data.dataPtrForIndex(1), 36);

      /* removing conntroller key */
      removeControllerKey(controllerRec);

      PHKNetworkMessageDataRecord drec;
      drec.activate = true;
      drec.data = new char[1];
      *drec.data = 2;
      drec.index = 6;
      drec.length = 1;
      PHKNetworkMessageData data;
      data.addRecord(drec);
      data.rawData((const char **)&replyData, &replyDataLen);
      returnType = pairingTlv8Type;
      statusCode = HTTP_STATUS_OK;
    }
    // Pairing status change, so update
    statusCode = HTTP_STATUS_OK;
    updatePairable();
  } else if (strncmp(path, "/prepare", 8) == 0) {
    supla_log(LOG_DEBUG,
              "Controller sends an characteristic prepare write message");

    /* we allways response with true */
    std::string desc = "{ \"status\" : 0 }";

    replyDataLen = desc.length();
    replyData = new char[replyDataLen + 1];
    memcpy(replyData, desc.c_str(), replyDataLen);
    statusCode = HTTP_STATUS_OK;

  } else if (strncmp(path, "/characteristics", 16) == 0) {
    supla_log(LOG_DEBUG, "Controller sends characteristics message ");

    if (strncmp(method, "GET", 3) == 0) {
      // Read characteristics
      int aid = 0;
      int iid = 0;

      char indexBuffer[1000];

      sscanf(path, "/characteristics?id=%[^\n]", indexBuffer);

      supla_log(LOG_DEBUG, "get characteristics %s with len %d", indexBuffer,
                strlen(indexBuffer));

      statusCode = HTTP_STATUS_NOT_FOUND;
      jsoncons::json characteristics;
      jsoncons::json characteristics_array = jsoncons::json::make_array();
      while (strlen(indexBuffer) > 0) {
        supla_log(LOG_DEBUG, "get characteristics %s with len %d", indexBuffer,
                  strlen(indexBuffer));

        char temp[1000];
        // Initial the temp
        temp[0] = 0;
        sscanf(indexBuffer, "%d.%d%[^\n]", &aid, &iid, temp);
        supla_log(LOG_DEBUG, "get temp %s with len %d", temp, strlen(temp));

        strncpy(indexBuffer, temp, 1000);
        supla_log(LOG_DEBUG, "get characteristics %s with len %d\n",
                  indexBuffer, strlen(indexBuffer));

        // Trim comma
        if (indexBuffer[0] == ',') {
          indexBuffer[0] = '0';
        }

        accessory *accessory = accessories->getAccessoryById(aid);

        if (accessory) {
          characteristic *c = accessory->getCharacteristicById(iid);
          if (c) {
            supla_log(LOG_DEBUG, "ask for one characteristics: %d . %d\n", aid,
                      iid);

            jsoncons::ojson chr = c->describeValue();
            characteristics_array.push_back(chr);
          }
        }
      }
      characteristics["characteristics"] = characteristics_array;

      std::string result;
      characteristics.dump(result);

      supla_log(LOG_DEBUG, "characteristics: %s", result.c_str());

      replyDataLen = result.length();
      replyData = new char[replyDataLen + 1];
      replyData[replyDataLen] = 0;

      memcpy(replyData, result.c_str(), replyDataLen);
      statusCode = HTTP_STATUS_OK;

    } else if (strncmp(method, "PUT", 3) == 0) {
      // Change characteristics
      supla_log(LOG_DEBUG, "PUT characteristics");
      /* TODO lbek parse characteristics json */
      char characteristicsBuffer[1000];
      sscanf(dataPtr, "{\"characteristics\":[{%[^]]s}", characteristicsBuffer);

      char *buffer2 = characteristicsBuffer;
      while (strlen(buffer2) && statusCode != HTTP_STATUS_BAD_REQUEST) {
        bool updateNotify = false;
        char *buffer1;
        buffer1 = strtok_r(buffer2, "}", &buffer2);
        if (*buffer2 != 0) buffer2 += 2;

        int aid = 0;
        int iid = 0;
        char value[16];
        int result = sscanf(buffer1, "\"aid\":%d,\"iid\":%d,\"value\":%s", &aid,
                            &iid, value);
        if (result == 2) {
          sscanf(buffer1, "\"aid\":%d,\"iid\":%d,\"ev\":%s", &aid, &iid, value);
          updateNotify = true;
        } else if (result == 0) {
          sscanf(buffer1,
                 "\"remote\":true,\"value\":%[^,],\"aid\":%d,\"iid\":%d", value,
                 &aid, &iid);
          if (result == 2) {
            sscanf(buffer1, "\"remote\":true,\"aid\":%d,\"iid\":%d,\"ev\":%s",
                   &aid, &iid, value);
            updateNotify = true;
          }
          sender->relay = true;
        }

        accessory *accessory = accessories->getAccessoryById(aid);

        if (accessory == NULL) {
          statusCode = HTTP_STATUS_BAD_REQUEST;
        } else {
          characteristic *c = accessory->getCharacteristicById(iid);

          if (updateNotify) {
            supla_log(LOG_DEBUG,
                      "ask to notify one characteristics: %d . %d -> %s", aid,
                      iid, value);

            if (c == NULL) {
              statusCode = HTTP_STATUS_BAD_REQUEST;
            } else {
              if (c->notifiable()) {
                if (strncmp(value, "1", 1) == 0 ||
                    strncmp(value, "true", 4) == 0)
                  sender->addNotify(c, aid, iid);
                else
                  sender->removeNotify(c);

                statusCode = HTTP_STATUS_NO_CONTENT;
              } else {
                statusCode = HTTP_STATUS_BAD_REQUEST;
              }
            }
          } else {
            supla_log(LOG_DEBUG,
                      "ask to change one characteristics: %d . %d -> %s", aid,
                      iid, value);

            if (c == NULL) {
              statusCode = HTTP_STATUS_BAD_REQUEST;
            } else {
              if (c->writable()) {
                c->setValue(value);
                statusCode = HTTP_STATUS_NO_CONTENT;
              } else {
                statusCode = HTTP_STATUS_BAD_REQUEST;
              }
            }
          }
        }
      }
    } else {
      delete[] replyData;
      return;
    }

  } else {
    // error
    supla_log(LOG_ERR, "asked for something I don't know\n");
    supla_log(LOG_ERR, "request: %s", request);
    supla_log(LOG_ERR, "path: %s", path);
    statusCode = HTTP_STATUS_NOT_FOUND;
  }

  // Calculate the length of header
  char *tmp = new char[256];
  memset(tmp, 0, 256);
  int len = snprintf(
      tmp, 256, "%s %d OK\r\nContent-Type: %s\r\nContent-Length: %u\r\n\r\n",
      protocol, statusCode, returnType, replyDataLen);
  delete[] tmp;

  // replyLen should omit the '\0'.
  (*replyLen) = len + replyDataLen;
  // reply should add '\0', or the printf is incorrect
  *reply = new char[*replyLen + 1];
  memset(*reply, 0, *replyLen + 1);

  /* writing header data */
  snprintf(*reply, len + 1,
           "%s %d OK\r\nContent-Type: %s\r\nContent-Length: %u\r\n\r\n",
           protocol, statusCode, returnType, replyDataLen);

  if (replyData) {
    memcpy(&(*reply)[len], replyData, replyDataLen);
    delete[] replyData;
  }

  /* Here we have full http response */
  supla_log(LOG_DEBUG, "Prepared http response: %s", *reply);
}

#ifndef PHKACCESSORY_H
#define PHKACCESSORY_H
//
//  PHKAccessory.h
//  Workbench
//
//  Created by Wai Man Chan on 9/27/14.
//
//

#include <string.h>
#include <strings.h>
#define __STDC_LIMIT_MACROS
#include <stdint.h>

extern "C" {
#include <stdlib.h>
}

#include <pthread.h>

#include <vector>

#include "PHKControllerRecord.h"
#include "PHKNetworkIP.h"
#include "globals.h"
#include "supla-client-lib/log.h"

#define HTTP_PATH_MAX_LENGTH 1024
#define HTTP_PROTOCOL_1_1 "HTTP/1.1"
#define HTTP_STATUS_OK 200
#define HTTP_STATUS_NOT_FOUND 404
#define HTTP_STATUS_BAD_REQUEST 400
#define HTTP_STATUS_NO_CONTENT 204

struct broadcastInfo {
  void *sender;
  char *desc;
};

void handleAccessory(const char *request, unsigned int requestLen, char **reply,
                     unsigned int *replyLen, connectionInfo *sender);
void *announce(void *info);

#endif

/*
 * globals.h
 *
 *  Created on: 18 lut 2020
 *      Author: beku
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "channel.h"
#include "notification.h"

extern notifications* ntfns;
extern channels* chnls;

typedef struct {
  int channelid;
  int index;
  bool wasIndexed;
} channel_index;

#endif /* GLOBALS_H_ */

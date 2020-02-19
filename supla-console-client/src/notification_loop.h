/*
 * notification_loop.h
 *
 *  Created on: 18 lut 2020
 *      Author: beku
 */

#ifndef NOTIFICATION_LOOP_H_
#define NOTIFICATION_LOOP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "crontab_parser.h"
#include "notification.h"

void notification_loop(void *user_data, void *sthread);
void notification_notify(void* value);

#endif /* NOTIFICATION_LOOP_H_ */

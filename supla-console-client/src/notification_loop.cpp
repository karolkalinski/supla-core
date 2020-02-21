/*
 * notification_loop.cpp
 *
 *  Created on: 18 lut 2020
 *      Author: beku
 */

#include "notification_loop.h"

#include "globals.h"
#include "supla-client-lib/sthread.h"

void notification_loop(void *user_data, void *sthread) {
  while (sthread_isterminated(sthread) == 0) {
    ntfns->handle();
    /* 1 sec sleep */
    usleep(1000000);
  }
}

void notification_notify(void *value) {
  if (value != NULL) ((notification *)value)->notify();
}

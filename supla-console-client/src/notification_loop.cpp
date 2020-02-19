/*
 * notification_loop.cpp
 *
 *  Created on: 18 lut 2020
 *      Author: beku
 */

#include "supla-client-lib/sthread.h"
#include "globals.h"
#include "notification_loop.h"


void notification_loop(void *user_data, void *sthread) {


	while (sthread_isterminated(sthread) == 0) {

		ntfns->handle();
		usleep(1000000);
   }
}

void notification_notify(void* value) {
	((notification*)value)->notify();
}


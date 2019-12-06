#ifndef HOMEKIT_LOOP_H_
#define HOMEKIT_LOOP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "client_device.h"
#include "clientcfg.h"
#include "supla-client-lib/log.h"
#include "supla-client-lib/srpc.h"
#include "supla-client-lib/sthread.h"
#include "supla-client-lib/supla-client.h"
#include "supla-client-lib/tools.h"

void homekit_loop(void *user_data, void *sthread);

#endif /* HOMEKIT_LOOP_H_ */

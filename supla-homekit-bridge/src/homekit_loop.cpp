#include "homekit_loop.h"

#include <fstream>
#include <iostream>

#include "globals.h"
#include "homekit_protocol_network.h"
#include "supla-client-lib/log.h"

void homekit_loop(void *user_data, void *sthread) {
  currentDeviceType = deviceType_bridge;

  /*initAccessorySet - read from supla_device */
  supla_log(LOG_DEBUG, "Waiting for Supla being initialized...");
  while (!channels->getInitialized()) {
    usleep(500);
  };

  supla_log(LOG_DEBUG, "Entering HomeKit loop");

  PHKNetworkIP networkIP;

  while (sthread_isterminated(sthread) == 0) {
    networkIP.handleConnection();
  }

  supla_log(LOG_DEBUG, "Clearing homekit data...");
}

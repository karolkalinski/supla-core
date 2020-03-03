/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "client_loop.h"
#include "clientcfg.h"
#include "globals.h"
#include "homekit_configuration.h"
#include "homekit_controller_record.h"
#include "homekit_loop.h"
#include "homekit_protocol_network.h"
#include "supla-client-lib/log.h"
#include "supla-client-lib/sthread.h"
#include "supla-client-lib/supla-client.h"
#include "supla-client-lib/tools.h"

homekit_accessories *accessories;
client_device_channels *channels;

int main(int argc, char *argv[]) {
  void *client_loop_t = NULL;
  void *homekit_loop_t = NULL;

  printf("SUPLA-HOMEKIT-BRIDGE v%s\n", "1.0.0.ALPHA");

  for (int i = 0; i < argc; i++) {
    if (strcmp("--version", argv[i]) == 0) {
      return 0;
    } else if (strcmp("--reset", argv[i]) == 0) {
      if (remove(Configuration::Instance()
                     .getControllerRecordsAddress()
                     .c_str()) != 0)
        supla_log(LOG_ERR, "Error deleting controller record file\n");
      else {
        printf("Controller record file successfully deleted.\n");
        printf(
            "You need to remove supla-homekit-bridge from your IOS device "
            "now.\n");
      }
      return 0;
    }
  };

  /* code generator */

  srand(time(NULL));
  Configuration::Instance().setDevicePassword(
      std::to_string(rand() % 10) + std::to_string(rand() % 10) +
      std::to_string(rand() % 10) + "-" + std::to_string(rand() % 10) +
      std::to_string(rand() % 10) + "-" + std::to_string(rand() % 10) +
      std::to_string(rand() % 10) + std::to_string(rand() % 10));

  if (!hasController()) {
    printf("Your setup code is %s\n",
           Configuration::Instance().getDevicePassword().c_str());
    printf(
        "You need to provide it on accessory adding proces in your IOS "
        "device\n");
  }
  /*----------------*/

  supla_log(LOG_INFO, "Initializing main loop");

  if (clientcfg_init(argc, argv) == 0) {
    clientcfg_free();
    return EXIT_FAILURE;
  }

  st_mainloop_init();
  st_hook_signals();

  void *sclient = NULL;

  channels = new client_device_channels();
  accessories = new homekit_accessories();

  client_loop_t = sthread_simple_run(client_loop, (void *)&sclient, 0);
  homekit_loop_t = sthread_simple_run(homekit_loop, (void *)&sclient, 0);

  while (st_app_terminate == 0) {
    st_mainloop_wait(5000);
  }

  supla_log(LOG_DEBUG, "Closing homekit socket...");
  close_socket();
  supla_log(LOG_DEBUG, "Terminating supla client loop");
  sthread_twf(client_loop_t);
  supla_log(LOG_DEBUG, "Terminating homekit loop");
  sthread_twf(homekit_loop_t);

  supla_log(LOG_DEBUG, "Closing other stuff");
  st_mainloop_free();
  clientcfg_free();

  delete channels;
  delete accessories;

  return EXIT_SUCCESS;
}

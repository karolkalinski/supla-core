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
#include <sys/types.h>
#include <sys/utsname.h>
#include <unistd.h>

#include "accept_loop.h"
#include "channel-io.h"
#include "devcfg.h"
#include "devconnection.h"
#include "mqtt/mqtt_client.h"
#include "supla-client-lib/ipcsocket.h"
#include "supla-client-lib/log.h"
#include "supla-client-lib/sthread.h"
#include "supla-client-lib/tools.h"
#include "RCSwitch.h"
#include "Sensor.h"

void exit_fail() {
  devcfg_free();
  channelio_free();
  exit(EXIT_FAILURE);
}

void mqtt_client_publish_callback(const char* topic, const char* payload,
                                  char retain, char qos) {
  mqtt_client_publish(topic, payload, retain, qos);
}

int main(int argc, char* argv[]) {
// INIT BLOCK
#ifdef __DEBUG
  char GUIDHEX[SUPLA_GUID_HEXSIZE + 1];
#endif

#ifdef __SINGLE_THREAD
  supla_log(LOG_DEBUG, "SINGLE THREAD MODE");
#endif

  printf("SUPLA-VIRTUAL-DEVICE v%s\n", SOFTWARE_VERSION);
  const int RXPIN=5;
  const int TXPIN=0;
  if(wiringPiSetup() == -1)
	return 0;

  RCSwitch *rc = new RCSwitch(RXPIN,TXPIN);

  for (int i = 0; i < argc; i++) {
    if (strcmp("-v", argv[i]) == 0) {
      return 0;
    }
  }

  if (channelio_init() != 1) {  // Before cfg init
    supla_log(LOG_ERR, "Channel I/O init error!");
    return EXIT_FAILURE;
  }

  if (devcfg_init(argc, argv) == 0) {
    channelio_free();
    return EXIT_FAILURE;
  }

  if (devcfg_getdev_guid() == 0) {
    supla_log(LOG_ERR, "Device GUID error!");
    exit_fail();
  }

  if (devcfg_getdev_authkey() == 0) {
    supla_log(LOG_ERR, "AuthKey error!");
    exit_fail();
  }

  if (run_as_daemon && 0 == st_try_fork()) {
    exit_fail();
  }

  supla_log(LOG_DEBUG, "Protocol v%d", scfg_int(CFG_PROTO));
  supla_write_state_file(scfg_string(CFG_STATE_FILE), LOG_DEBUG, "Starting...");

#ifdef __DEBUG
  st_guid2hex(GUIDHEX, DEVICE_GUID);
  GUIDHEX[SUPLA_GUID_HEXSIZE] = 0;
  supla_log(LOG_DEBUG, "Device GUID: %s", GUIDHEX);
#endif

  st_setpidfile(pidfile_path);
  st_mainloop_init();
  st_hook_signals();

  supla_log(LOG_DEBUG, "Initializing channels...");
  channelio_channel_init();

  // CONNECTION START
  supla_log(LOG_DEBUG, "Initializing server connection...");
  void* dconn = devconnection_start();

  // MAIN LOOP
  supla_log(LOG_DEBUG, "Entering main loop...");
  while (st_app_terminate == 0) {
    if (rc->OokAvailable()) {
      cout << "Stalo sie";
      char message[100];
      rc->getOokCode(message);
      Sensor *s = Sensor::getRightSensor(message);
      if (s!= NULL) {
				printf("Temp : %f\n",s->getTemperature());
				printf("Humidity : %f\n",s->getHumidity());
				printf("Channel : %d\n",s->getChannel());
                               // if((loggingok) && (s->getChannel()>0)) {
                               //         fprintf(fp,"%d,temp%f,hum%f\n",s->getChannel(),s->getTemperature(),s->getHumidity());
                               //         fflush(fp);
                               //         fflush(stdout);
                               // }

        char tmp_value[SUPLA_CHANNELVALUE_SIZE];
        int n;
        double val2 = ( time( 0 ) % 3600 ) / 60, val1 = s->getTemperature();
        n = val1 * 1000;
        memcpy(tmp_value, &n, 4);

        n = val2 * 1000;
        memcpy(&tmp_value[4], &n, 4);
        channelio_set_value(1, tmp_value, 0);
      }
      delete s;

    } else if (rc->NexusAvailable()) {
      char tmp_value[SUPLA_CHANNELVALUE_SIZE];
      rc->getNexusCode(tmp_value);
      channelio_set_value(0, tmp_value, 0);
    }
    st_mainloop_wait(1000000);
  }

  // RELEASE BLOCK
  devconnection_stop(dconn);
  st_delpidfile(pidfile_path);
  devcfg_free();
  channelio_free();
  st_mainloop_free();  // after channelio_free

  return EXIT_SUCCESS;
}
/* ===================================================
C code : test.cpp
* ===================================================
*/

/*#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "RCSwitch.h"
#include "RcOok.h"
#include "Sensor.h"

int main(int argc, char *argv[])
{
	int RXPIN = 5;
	int TXPIN = 0;
        int loggingok;   // Global var indicating logging on or off
        FILE *fp;        // Global var file handle
        
        if(argc==2) {
          fp = fopen(argv[1], "a"); // Log file opened in append mode to avoid destroying data
          loggingok=1;
          if (fp == NULL) {
              perror("Failed to open log file!"); // Exit if file open fails
              exit(EXIT_FAILURE);
          }
        } else {
          loggingok=0;
        }


	RCSwitch *rc = new RCSwitch(RXPIN,TXPIN);

	while (1)
	{
		{

			printf("%s\n",message);

			Sensor *s = Sensor::getRightSensor(message);
			if (s!= NULL)
			{
				printf("Temp : %f\n",s->getTemperature());
				printf("Humidity : %f\n",s->getHumidity());
				printf("Channel : %d\n",s->getChannel());
                                if((loggingok) && (s->getChannel()>0)) {
                                        fprintf(fp,"%d,temp%f,hum%f\n",s->getChannel(),s->getTemperature(),s->getHumidity());
                                        fflush(fp);
                                        fflush(stdout);
                                }
			}
			delete s;
		}
		delay(1000);
	}
}
*/

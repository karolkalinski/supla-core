/*
 * channelstateloop.cpp
 *
 *  Created on: 1 wrz 2020
 *      Author: beku
 */

#include "channelstateloop.h"

#include "globals.h"
#include "supla-client-lib/log.h"
#include "supla-client-lib/safearray.h"
#include "supla-client-lib/sthread.h"
#include "supla-client-lib/supla-client.h"

void channel_state_loop(void *user_data, void *sthread) {
  bool wasFirstCall = false;

  while (sthread_isterminated(sthread) == 0) {
    if (user_data == NULL) {
      sleep(1);
      continue;
    }

    void *channels = chnls->get_channels();
    if (channels != NULL) {
      safe_array_lock(channels);

      int channel_count = safe_array_count(channels);

      wasFirstCall = channel_count > 0;

      for (int i = 0; i < channel_count; i++) {
        channel *temp = (channel *)safe_array_get(channels, i);

        if (temp != NULL) {
          if ((temp->getFlags() & SUPLA_CHANNEL_FLAG_CHANNELSTATE) != 0) {
            supla_log(LOG_DEBUG, "channel %d has state flag",
                      temp->getChannelId());
            supla_client_get_channel_state(*(void **)user_data,
                                           temp->getChannelId());
          }
        }
      }

      safe_array_unlock(channels);
    }

    /* 30 minut sleep */
    if (wasFirstCall)
      sleep(1800);
    else
      sleep(5);
  }
}

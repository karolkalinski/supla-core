#include "homekit_loop.h"
#include <iostream>
#include <fstream>
#include "PHKNetworkIP.h"

void homekit_loop(void *user_data, void *sthread) {
    
	/*initAccessorySet - read from supla_device */
        
	PHKNetworkIP networkIP;
	while (sthread_isterminated(sthread) == 0) {
		networkIP.handleConnection();
	}
}
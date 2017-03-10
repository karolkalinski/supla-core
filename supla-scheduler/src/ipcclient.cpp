/*
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

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "ipcclient.h"
#include "log.h"

#define IPC_SAUTH_KEY_SIZE 16

const char socket_path[] = "/tmp/supla-server-ctrl.sock";
const char hello[] = "SUPLA SERVER CTRL\n";

const char cmd_is_iodev_connected[] = "IS-IODEV-CONNECTED";

const char cmd_get_double_value[] = "GET-DOUBLE-VALUE";
const char cmd_get_char_value[] = "GET-CHAR-VALUE";
const char cmd_get_rgbw_value[] = "GET-RGBW-VALUE";

const char cmd_set_char_value[] = "SET-CHAR-VALUE";
const char cmd_set_rgbw_value[] = "SET-RGBW-VALUE";

const char ipc_result_value[] = "VALUE:";
const char ipc_result_ok[] = "OK:";
const char ipc_result_connected[] = "CONNECTED:";
const char ipc_result_disconnected[] = "DISCONNECTED:";

ipc_client::ipc_client() {

	this->sfd = -1;
	ipc_sauth_key = NULL;

	int   ipc_shmid = -1;
	key_t key;
	key = ftok(socket_path, 'S');

	if((ipc_shmid = shmget(key, IPC_SAUTH_KEY_SIZE, 0)) == -1)
		return;

	if ( (ipc_sauth_key = (char *)shmat(ipc_shmid, 0, 0)) == (char*)-1 )
		ipc_sauth_key = NULL;
}

ipc_client::~ipc_client() {

	ipc_disconnect();
}

int ipc_client::read(void) {

    fd_set set;
    struct timeval timeout;
    FD_ZERO(&set);
    FD_SET(sfd, &set);
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

	if ( sfd == -1 )
		return 0;

    memset(buffer, 0, IPC_BUFFER_SIZE);

    int rv = select(sfd + 1, &set, NULL, NULL, &timeout);

    if ( rv == 0 ) {
    	return 0;
    };

    ssize_t len = recv(sfd, buffer, IPC_BUFFER_SIZE, 0);

    if ( len > 0 ) {
    	if ( len >= IPC_BUFFER_SIZE )
    		len = IPC_BUFFER_SIZE;

    	buffer[len] = 0;
    	//supla_log(LOG_DEBUG, "%s", buffer);
    	return len;
    }

    return 0;
}

bool ipc_client::ipc_connect(void) {

	struct sockaddr_un remote;
	int len;

	if ( sfd != -1 )
		return true;

	if ((sfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
	       return false;
	}

	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, socket_path);

    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(sfd, (struct sockaddr *)&remote, len) == -1) {
    	ipc_disconnect();
    	return false;
    }

    if ( read() && strcmp(buffer, hello) == 0 )
    	return true;

    ipc_disconnect();

	return false;
}


bool ipc_client::ipc_disconnect(void) {

	if ( sfd == -1 )
		return false;

	close(sfd);
	sfd = -1;

	return true;
}

bool ipc_client::auth(void) {

	if ( !ipc_connect() || ipc_sauth_key == NULL )
		return false;

	snprintf(buffer, IPC_BUFFER_SIZE, "SAUTH:");
	memcpy(&buffer[6], ipc_sauth_key, IPC_SAUTH_KEY_SIZE);

	buffer[6+IPC_SAUTH_KEY_SIZE] = '\n';

	send(sfd, buffer, strlen(buffer), 0);

	if ( read() && strcmp(buffer, "AUTH_OK\n") == 0 )
		return true;

	return false;
}

bool ipc_client::get_value(const char *cmd, int user_id, int device_id, int channel_id) {

	if ( !ipc_connect() )
		return false;

	snprintf(buffer, IPC_BUFFER_SIZE, "%s:%i,%i,%i\n", cmd, user_id, device_id, channel_id);
	send(sfd, buffer, strlen(buffer), 0);

    if ( read()
    	 && memcmp(buffer, ipc_result_value, strlen(ipc_result_value)) == 0 ) {

    	return true;
    }

	return false;

}

char ipc_client::is_connected(int user_id, int device_id) {

	if ( ipc_connect() ) {


		snprintf(buffer, IPC_BUFFER_SIZE, "%s:%i,%i\n", cmd_is_iodev_connected, user_id, device_id);
		//supla_log(LOG_DEBUG, "%s", buffer);
		send(sfd, buffer, strlen(buffer), 0);

	    if ( read() ) {

	    	if ( memcmp(buffer, ipc_result_connected, strlen(ipc_result_connected)) == 0 )
		    	return IPC_RESULT_CONNECTED;

	    	if ( memcmp(buffer, ipc_result_disconnected, strlen(ipc_result_disconnected)) == 0 )
		    	return IPC_RESULT_DISCONNECTED;
	    }
	}


	return IPC_RESULT_SERVER_UNREACHABLE;
}

bool ipc_client::get_double_value(int user_id, int device_id, int channel_id, double *value) {

	if ( value == NULL
		 || !get_value(cmd_get_double_value, user_id, device_id, channel_id)
		 || sscanf (&buffer[strlen(ipc_result_value)], "%lf", value) != 1  )
		return false;


	return true;
}

bool ipc_client::get_char_value(int user_id, int device_id, int channel_id, char *value) {

	int x = 0;

	if ( value == NULL
		 || !get_value(cmd_get_char_value, user_id, device_id, channel_id)
		 || sscanf (&buffer[strlen(ipc_result_value)], "%i", &x) != 1  )
		return false;

	if ( (unsigned int)x >= 255 )
		return false;

	*value = x;

	return true;
}

bool ipc_client::get_rgbw_value(int user_id, int device_id, int channel_id, int *color, char *color_brightness, char *brightness) {

	int _color_brightness = 0;
	int _brightness = 0;

	if ( color == NULL
		 || color_brightness == NULL
		 || brightness == NULL
		 || !get_value(cmd_get_rgbw_value, user_id, device_id, channel_id)
		 || sscanf (&buffer[strlen(ipc_result_value)], "%i,%i,%i", color, &_color_brightness, &_brightness) != 3  )
		return false;

	if ( _color_brightness < 0
		 || _color_brightness > 100
		 || _brightness < 0
		 || _brightness > 100 )
		return false;

	*color_brightness = _color_brightness;
	*brightness = _brightness;

	return true;
}

bool ipc_client::check_set_result(void) {

    if ( read()
    	 && memcmp(buffer, ipc_result_ok, strlen(ipc_result_ok)) == 0 ) {

    	return true;
    }

    return false;
}

bool ipc_client::set_char_value(int user_id, int device_id, int channel_id, char value) {

	if ( !auth() )
		return false;

	snprintf(buffer, IPC_BUFFER_SIZE, "%s:%i,%i,%i,%i\n", cmd_set_char_value, user_id, device_id, channel_id, value);
	send(sfd, buffer, strlen(buffer), 0);

	return check_set_result();
}

bool ipc_client::set_rgbw_value(int user_id, int device_id, int channel_id, int color, char color_brightness, char brightness) {

	if ( !auth() )
		return false;

	snprintf(buffer, IPC_BUFFER_SIZE, "%s:%i,%i,%i,%i,%i,%i\n", cmd_set_rgbw_value, user_id, device_id, channel_id, color, color_brightness, brightness);
	send(sfd, buffer, strlen(buffer), 0);

	return check_set_result();
}

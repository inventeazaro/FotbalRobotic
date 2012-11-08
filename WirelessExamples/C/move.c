#include <signal.h>
#include <stdio.h>
#include <string.h>

#ifndef WIN32
#  include <unistd.h>
#else
#  include <process.h>
#  define snprintf sprintf_s
#endif

#include <mosquitto.h>

#define mqtt_host "localhost"
#define mqtt_port 1883

static int run = 1;

void handle_signal(int s)
{
	run = 0;
}

void connect_callback(struct mosquitto *mosq, void *obj, int result)
{
}

void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
}

int main(int argc, char *argv[])
{
	char clientid[24];
	struct mosquitto *mosq;
	int rc = 0;

	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);

	mosquitto_lib_init();


	mosq = mosquitto_new(clientid, true, NULL);
	if(mosq){
		mosquitto_connect_callback_set(mosq, connect_callback);
		mosquitto_message_callback_set(mosq, message_callback);

		rc = mosquitto_connect(mosq, mqtt_host, mqtt_port, 60);
				
		mosquitto_subscribe(mosq, NULL, "#", 0);
	
		while(run){
			rc = mosquitto_loop(mosq, -1, 1);
			if(run && rc){
				sleep(20);
				mosquitto_reconnect(mosq);
				}
			}
			mosquitto_destroy(mosq);
	}

	mosquitto_lib_cleanup();

	return rc;
}


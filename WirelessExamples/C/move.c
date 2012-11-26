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

struct control{
    int left;
    int right;
    int time;
} ctr;

struct robotCoords {
    int id; // id robot - 1..10
    int x;  // 0...800  - relative la terenul de joc
    int y;  // 0...600 
    int angle; // unghi fata de baza ringului
    int timestamp; // timpul cand au fost calculate - UNIXTIME - Ex: 1352460922
};

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
	struct robotCoords *coordonate = (struct robotCoords *)message->payload;
	
	printf("X: %d Y: %d\n", coordonate->x, coordonate->y);
	fflush(stdout);
}

int main(int argc, char *argv[])
{
	char clientid[24]="Move Test";
	struct mosquitto *mosq;
	int rc = 0;
	int mid;

	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);

	mosquitto_lib_init();


	mosq = mosquitto_new(clientid, true, NULL);
	if(mosq){
		mosquitto_connect_callback_set(mosq, connect_callback);
		mosquitto_message_callback_set(mosq, message_callback);

		rc = mosquitto_connect(mosq, mqtt_host, mqtt_port, 60);
				
		mosquitto_subscribe(mosq, NULL, "coords", 0);
		
		ctr.left=100;
		ctr.right=100;
		ctr.time=0;
		mosquitto_publish(mosq, &mid, "in15", sizeof(ctr), &ctr, 2, false);
	
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


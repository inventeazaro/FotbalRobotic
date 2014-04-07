#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <iomanip>
#include <iostream>
#include <mosquitto.h>

using namespace std;

char clientid[24]="FoRO"; //modificat in functie de numele echipei
struct mosquitto *mosq;

#define mqtt_host "192.168.0.100"  //adresa ip a serverului
#define mqtt_port 1883

struct RobotControl{
    int left;
    int right;
    int time;
    int left_rotation;
    int right_rotation;
};

struct RobotCoords {
    int id; // id robot - 1..10
    int x;  // 0...800  - relative la terenul de joc
    int y;  // 0...600
    int angle; // unghi fata de baza ringului
    int timestamp; // timpul cand au fost calculate - UNIXTIME - Ex: 1352460922
};


struct RobotControl control[11];
struct RobotCoords  coords[11];

static int run = 1;

char robotId[] = "/r0";  //reprezinta numarul robotului
unsigned int rId = 4;  //o sa aveti nevoie de id-ul robotului si sub forma de int

struct timeval  tv;

void connect_callback(struct mosquitto *mosq, void *obj, int result)
{
}

void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
    struct RobotCoords *coordonate = (struct RobotCoords *)message->payload;

    coords[coordonate->id].id = coordonate->id;
    coords[coordonate->id].x  = coordonate->x;
    coords[coordonate->id].y  = coordonate->y;
    coords[coordonate->id].angle = coordonate->angle;
    coords[coordonate->id].timestamp = coordonate->timestamp;

    cerr<<"Id: "<<coordonate->id<<"Coords("<<coordonate->x<<":"<<coordonate->y<<")"<<"Angle:"<<coordonate->angle<<endl; //in loc de cout pentru a afisa in consola ca pe eroare
}

void calculate_robot_next_movement() {
    //Orienteaza-te la un anumit unghi
    struct RobotCoords thisR = coords[rId];

    // Vreau la 90deg
    // Folosind encoderele puteti sa fixati mai usor directia robotului
    if(thisR.angle>70 && thisR.angle <110)
    {
        // e pe aici, oprim motoarele
        control[rId].left = 0;
        control[rId].right = 0;
        control[rId].time  =0;
    }
    else
    {
            control[rId].left = 40;
            control[rId].right = -40;
            control[rId].time  = 30;
    }
}

void do_robot_control_loop() {
    int mid;
    fflush(stdout);

    calculate_robot_next_movement();

    mosquitto_publish(mosq, &mid, robotId, sizeof(struct RobotControl), &control[rId], 0, false);
    gettimeofday(&tv, NULL);
}

int need_to_send() {
    struct timeval now;

    gettimeofday(&now, NULL);

    if(tv.tv_usec > 800000)
        gettimeofday(&tv, NULL);

    if(now.tv_usec >= tv.tv_usec + 100000) {
            return 1;
    }

    return 0;
}

int main()
{
    int rc = 0;
    int mid;

    mosquitto_lib_init();
    mosq = mosquitto_new(clientid, true, NULL);
    if(mosq){
        mosquitto_connect_callback_set(mosq, connect_callback);
        mosquitto_message_callback_set(mosq, message_callback);

        rc = mosquitto_connect(mosq, mqtt_host, mqtt_port, 60);

        mosquitto_subscribe(mosq, NULL, "coords", 0);
        gettimeofday(&tv, NULL);

        while(run){

          //  calculate_robot_next_movement();

            if(need_to_send()) {
                do_robot_control_loop();
            }

            rc = mosquitto_loop(mosq, 1, 50);
            if(run && rc){
                sleep(1);
                mosquitto_reconnect(mosq);
            }
        }
        mosquitto_destroy(mosq);
    }

    mosquitto_lib_cleanup();

    return rc;
}


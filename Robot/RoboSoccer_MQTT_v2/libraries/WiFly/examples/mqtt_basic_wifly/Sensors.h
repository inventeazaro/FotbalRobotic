#ifndef _SENSORS_H_
#define _SENSORS_H_

#include <UberdustSensors.h>

#define MAX_SENSORS_NUM 20

int sensorCount = 0; 

lightSensor *light1 = new lightSensor("light1", A0);

temperatureSensor *temp1 = new temperatureSensor("temp1", A1);

RandomSensor *rand1 = new RandomSensor("rand1",50);

zoneSensor *zone1 = new zoneSensor("zone1",9);


#endif
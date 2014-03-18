#include <SPI.h>
#include <WiFly.h>
#include <PubSubClient.h>

#include "Credentials.h"
#include "Sensors.h"

// Update these with values suitable for your network.
//uint8_t server[] = { 150, 140, 5, 20 };
char *server = "q.m2m.io";
CoapSensor *sensorsArray[MAX_SENSORS_NUM];
char *mac;				//wifly mac address
char channel[30];		//topic to subscribe to (s + mac)

WiFlyClient wiFlyClient;
PubSubClient client(server, 1883, callback, wiFlyClient);

void callback(char* topic, uint8_t* payload, unsigned int length) {
	// handle message arrived
	/* topic = part of the variable header:has topic name of the topic where the publish received
	 * NOTE: variable header does not contain the 2 bytes with the 
	 *       publish msg ID
	 * payload = pointer to the first item of the buffer array that
	 *           contains the message tha was published
	 *          EXAMPLE of payload: lights,1
	 * length = the length of the payload, until which index of payload
	 */
	digitalWrite(8, HIGH);
	delay(200);
	digitalWrite(8, LOW);
	delay(200);
	
	char sensorName[35];							// topic to publish to (mac + sensor)
	char * sensor = strtok((char *) payload, ",");	// sensor name
	int i = strcspn((char *) payload, ",");			//remaininig of payload

	uint8_t *value = payload+i;						//sensor's value
	size_t vallen = length-i;						//sensor's value length

	uint8_t output[10];
	size_t output_len;

	for (uint8_t i = 0; i < sensorCount; i++) {
		if(!strcmp(sensor, sensorsArray[i]->get_name())) {
			sensorsArray[i]->set_value(value, vallen, output, &output_len);
			strcpy(sensorName, "public/");
			strcat(sensorName, mac);							// sensorName = mac+sensor
			strcat(sensorName, "/");
			strcat(sensorName, sensor);
			client.publish(sensorName, output, output_len);
			break;
		}
	}
}

void setup()
{
	pinMode(8,OUTPUT);

	// lots of time for the WiFly to start up
	delay(5000);
	
	Serial.begin(9600);   // Start hardware Serial for the RN-XV
	WiFly.setUart(&Serial); // Tell the WiFly library that we are not using the SPIUart
	
	WiFly.begin();
	
	// Join the WiFi network
	if (!WiFly.join(ssid, passphrase, mode)) {
		Serial.println("Association failed.");
		while(1){
			// Hang on failure.
		}
	}
	Serial.println("Association succeeded.");

	set_channel();

	add_sensor(light1);
	add_sensor(temp1);
	add_sensor(rand1);
	add_sensor(zone1);

	Serial.println(sensorCount);

	if (client.connect("arduinoClient")) {
		client.subscribe(channel);
	}
	else
		Serial.println("Connection failed.");
}

void loop()
{
	static unsigned long timestamp = 0;
	if(!client.loop()) {
		Serial.println("Client disconnected.");
		while(1) {}
	}
	if(millis() - timestamp > 30000) {
		timestamp = millis();
		sensors_loop();
	}
}

void sensors_loop()
{
	uint8_t output[10];
	size_t output_len;
	char sensorName[40];
	

	for (uint8_t i = 0; i < sensorCount; i++) {
		sensorsArray[i]->check();
		strcpy(sensorName, "public/");
		strcat(sensorName, mac);
		strcat(sensorName, "/");
		strcat(sensorName, sensorsArray[i]->get_name());
		sensorsArray[i]->get_value(output, &output_len);
		client.publish(sensorName, output, output_len);
		delay(100);
	}
}

void add_sensor(CoapSensor * sensor)
{
	sensorsArray[sensorCount++] = sensor;
}

void set_channel()
{
	//channel[0] = 's';
	mac = WiFly.getMAC();
	strcpy(channel, "public/s");
	strcat(channel, mac);
}
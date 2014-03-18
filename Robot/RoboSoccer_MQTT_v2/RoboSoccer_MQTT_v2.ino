#include <SoftwareSerial.h>
#include <SPI.h>
#include <WiFly.h>
#include <PubSubClient.h>
#include "id.h"

#define SW_VERSION    "RS v0.8"
#define MQTT_TIMEOUT  1000

int MOTOR2_PIN1 = 10;
int MOTOR2_PIN2 = 9;
int MOTOR1_PIN1 = 6;
int MOTOR1_PIN2 = 5;

int ENCODER_R = 2;
int ENCODER_L = 3;


// Connection to wireless network
// It takes ~ 26 seconds to connect to wifi and mqtt server

// Update these with values suitable for your network.
byte server[] = { 192, 168, 0, 100 };

WiFlyClient wiFlyClient;
PubSubClient client(server, 1883, callback, wiFlyClient);

const char mySSID[] = "fotbalrobotic";
const char myPassword[] = "inventeaza";

SoftwareSerial mySerial(4, 7); //RX, TX

int motor1 = 0; //left
int motor2 = 0; //right
int time   = 0;
int rotation = 0;
long long lastTimeReceived=0;

int left_r = 0;
int right_r = 0;

struct control {
  long left;
  long right;
  long time;
  long rotation;
};

void callback(char* topic, uint8_t* payload, unsigned int length) {
  struct control *ctr = (struct control *)payload;
  
  motor1 = ctr->left;
  motor2 = ctr->right;
  time   = ctr->time;
  rotation = ctr->rotation;
  
  if (time == 0 && rotation == 0)
    time = MQTT_TIMEOUT;
  lastTimeReceived = millis();
    
  mySerial.print((long int)lastTimeReceived);
  mySerial.print(" : ");
  mySerial.print(motor1); 
  mySerial.print(" | "); 
  mySerial.println(motor2);
}

void left() {
  left_r++;
}

void right() {
  right_r++;
}

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  mySerial.println(F("Starting..."));

  pinMode(MOTOR1_PIN1, OUTPUT);
  pinMode(MOTOR1_PIN2, OUTPUT);
  pinMode(MOTOR2_PIN1, OUTPUT);
  pinMode(MOTOR2_PIN2, OUTPUT);
  
  pinMode(ENCODER_R, INPUT);
  pinMode(ENCODER_L, INPUT);
  
  attachInterrupt(0, right, CHANGE);
  attachInterrupt(1, left, CHANGE);
  
  mySerial.println(F("Attempting wireless connection"));
  WiFly.setUart(&Serial);
  //wiFly.begin(&Serial, &mySerial);
  WiFly.begin();

  //WiFly.join(("fotbalrobotic"), ("inventeaza"));
  //mySerial.println(F("Connected to wireless network!"));
  //mySerial.println(F("Attempting MQTT connection!"));

  if (client.connect("RSB " ROBOT_ID)) {
    client.publish("status", SW_VERSION " " ROBOT_ID);
    client.subscribe(ROBOT_ID);
  }else{
    mySerial.println(F("Connection problem"));
  }
  
  mySerial.println(F("Successfuly connected and running!"));
  mySerial.print(F("FreeRAM: "));
  mySerial.println(freeRAM());
}

void loop() {
  if(millis() >= (lastTimeReceived+time) &&
    (motor1 || motor2)) {
    motor1 = 0;
    motor2 = 0;
  }
  
  go(motor1, motor2);
  
  if(millis() > lastTimeReceived+time) {
    motor1 = 0;
    motor2 = 0;
    go(motor1, motor2);
  }
  
  if (!client.loop()) {
    // disconnected
    if (client.connect("RSB " ROBOT_ID)) {
        client.publish("status", SW_VERSION " " ROBOT_ID);
        client.subscribe(ROBOT_ID);
    }
  }
}

void go(int speedLeft, int speedRight) {
  if (speedLeft > 0) {
    analogWrite(MOTOR1_PIN1, speedLeft);
    analogWrite(MOTOR1_PIN2, 0);
  }
  else {
    analogWrite(MOTOR1_PIN1, 0);
    analogWrite(MOTOR1_PIN2, -speedLeft);
  }

  if (speedRight > 0) {
    analogWrite(MOTOR2_PIN1, speedRight);
    analogWrite(MOTOR2_PIN2, 0);
  }
  else {
    analogWrite(MOTOR2_PIN1, 0);
    analogWrite(MOTOR2_PIN2, -speedRight);
  }
}

static int freeRAM () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

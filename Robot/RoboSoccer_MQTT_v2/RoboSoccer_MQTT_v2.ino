#include <SoftwareSerial.h>

#include <SPI.h>
#include <WiFly.h>
#include <PubSubClient.h>
#include "id.h"

#define SW_VERSION    "RoboSoccer v0.4"
#define MQTT_TIMEOUT  1000

int MOTOR2_PIN1 = 5;
int MOTOR2_PIN2 = 3;
int MOTOR1_PIN1 = 6;
int MOTOR1_PIN2 = 9;

int BUZZER_PIN = 7;

int lowvoltage = 0;

// Update these with values suitable for your network.
byte server[] = { 192, 168, 1, 72 };

WiFlyClient wiFlyClient;
PubSubClient client(server, 1883, callback, wiFlyClient);

SoftwareSerial mySerial(2, 4); //RX, TX

int motor1=0; //left
int motor2=0; //right
int time;
long long lastTimeReceived=0;

struct control {
  long left;
  long right;
  long time;
};

void callback(char* topic, uint8_t* payload, unsigned int length) {
  struct control *ctr = (struct control *)payload;
  
  motor1 = ctr->left;
  motor2 = ctr->right;
  lastTimeReceived = millis();
    
  mySerial.print((long int)lastTimeReceived);
  mySerial.print(" : ");
  mySerial.print(motor1); 
  mySerial.print(" | "); 
  mySerial.println(motor2);
}

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  pinMode(MOTOR1_PIN1, OUTPUT);
  pinMode(MOTOR1_PIN2, OUTPUT);
  pinMode(MOTOR2_PIN1, OUTPUT);
  pinMode(MOTOR2_PIN2, OUTPUT);
  
  pinMode(BUZZER_PIN, OUTPUT);
  mySerial.println(F("Attempting wireless connection"));
  WiFly.setUart(&Serial);
  WiFly.begin();

  WiFly.join(("fotbalrobotic"), ("dorelrobotel"));
  mySerial.println(F("Connected to wireless network!"));
  mySerial.println(F("Attempting MQTT connection!"));

  if (client.connect("RoboSoccerBot " ROBOT_ID)) {
    client.publish(("status"),(SW_VERSION " " ROBOT_ID));
    client.subscribe((ROBOT_ID));
  }else{
    mySerial.println(F("Connection problem"));
  }
  
  mySerial.println(F("Successfuly connected and running!"));
  mySerial.print(F("FreeRAM: "));
  mySerial.println(freeRAM());
}

void loop() {
  int v = analogRead(0);
  v = (v/2 - 10);
  if(v > 310 && lowvoltage == 0){
    if(millis() > lastTimeReceived+MQTT_TIMEOUT) {
      motor1 = 0;
      motor2 = 0;
      mySerial.println("Emergency motor break!!!");
  
      if(!client.connected()) {
        if (client.connect("RoboSoccerBot " ROBOT_ID)) {
          client.publish(("status"),(SW_VERSION " " ROBOT_ID));
          client.subscribe((ROBOT_ID));
        } else {
          mySerial.println(F("Connection problem"));
        }
      }
    }
    go(motor1,motor2);
    client.loop();
  }else{
    lowvoltage = 1;
    tone(7, 1000, 1000);
    delay(200);
    tone(7, 5000, 1000);
    delay(200);
    tone(7, 2000, 1000);
    delay(200);
    tone(7, 4000, 1000);
    //go(0, 0);
    //go(0, 0);
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

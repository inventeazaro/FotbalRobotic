#include <SoftwareSerial.h>
#include <SPI.h>
#include <WiFly.h>
#include <PubSubClient.h>
#include "id.h"
#define MQTT_KEEPALIVE 300 
#define SW_VERSION    "RS v0.8"
#define MQTT_TIMEOUT  1000

int MOTOR2_PIN1 = 10;
int MOTOR2_PIN2 = 9;
int MOTOR1_PIN1 = 6;
int MOTOR1_PIN2 = 5;

int encoder_l = 3;
int encoder_r = 2;
int buttonState_l, buttonState_r, lastButtonState_l, lastButtonState_r;

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
long long lastTimeReceived=0;

volatile unsigned int left_r = 0;
volatile unsigned int right_r = 0;

struct control {
  long left;
  long right;
  long time;
  long rotation_left;
  long rotation_right;
};

void callback(char* topic, uint8_t* payload, unsigned int length) {
  struct control *ctr = (struct control *)payload;
  
  motor1 = ctr->left;
  motor2 = ctr->right;
  time   = ctr->time;
  left_r = ctr->rotation_left;
  right_r= ctr->rotation_right;
  
  if (time == 0 && left_r == 0)
    time = MQTT_TIMEOUT;
  lastTimeReceived = millis();
    
  mySerial.print((long int)lastTimeReceived);
  mySerial.print(" : ");
  mySerial.print(motor1); 
  mySerial.print(" | "); 
  mySerial.println(motor2);
}

void leftInt() {
  left_r++;
}

void rightInt() {
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
  
  pinMode(encoder_l, INPUT);
  pinMode(encoder_r, INPUT);
  
  digitalWrite(encoder_l, HIGH);
  digitalWrite(encoder_r, HIGH);
  
  attachInterrupt(0, rightInt, CHANGE);
  attachInterrupt(1, leftInt, CHANGE);
  
#ifdef COMM_ENABLED
  mySerial.println(F("Attempting wireless connection"));
  WiFly.setUart(&Serial);
  //wiFly.begin(&Serial, &mySerial);
  WiFly.begin();
  if (client.connect("RSB " ROBOT_ID)) {
    client.publish("status", SW_VERSION " " ROBOT_ID);
    client.subscribe(ROBOT_ID);
  }else{
    mySerial.println(F("Connection problem"));
  }
  
  mySerial.println(F("Successfuly connected and running!"));
#endif
  mySerial.print(F("FreeRAM: "));
  mySerial.println(freeRAM());
}

long nr = 0;

void loop() {
#ifdef COMM_ENABLED
  client.loop();
#endif
  
  //daca am val pt encoder apelez comanda encoder
//  if (left_r > 0 || right_r > 0)  
//    {
//
//    }
//  else   
    if(millis() >= (lastTimeReceived+time) && (motor1 || motor2)) 
    {
      motor1 = 0;
      motor2 = 0;
    }
  
  go(motor1, motor2);


#ifdef COMM_ENABLED
  if (nr > 35000) //pentru a nu se deconecta, dar tot pierde conexiunea
  {
    nr = 0;
    client.publish("status",ROBOT_ID "<- sunt inca prezent");
  }
  
  if (!client.loop()) {
    // disconnected
    if (client.connect("RSB " ROBOT_ID)) {
        client.publish("status", SW_VERSION " " ROBOT_ID);
        client.subscribe(ROBOT_ID);
    }
  }
  
  if(!client.connected()){
    mySerial.println("disconnected, tring to reconnect");
    if (client.connect("RSB " ROBOT_ID)) {
        client.publish("status", SW_VERSION " " ROBOT_ID);
        client.subscribe(ROBOT_ID);
    }
  }
#endif

  if(millis()%1000 == 0){
    mySerial.println("ALIVE!");
    mySerial.print(left_r, DEC);
    mySerial.print("|");
    mySerial.println(right_r, DEC);
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

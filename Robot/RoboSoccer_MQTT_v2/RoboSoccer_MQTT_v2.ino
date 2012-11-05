#include <SoftwareSerial.h>

#include <SPI.h>
#include <WiFly.h>
#include <PubSubClient.h>

#define SW_VERSION "RoboSoccer v0.2"
#define ROBOT_ID   "in2"


int MOTOR2_PIN1 = 5;
int MOTOR2_PIN2 = 3;
int MOTOR1_PIN1 = 6;
int MOTOR1_PIN2 = 9;

// Update these with values suitable for your network.
byte mac[]    = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte server[] = { 192, 168, 1, 72 };

WiFlyClient wiFlyClient;
PubSubClient client(server, 1883, callback, wiFlyClient);

SoftwareSerial mySerial(2, 4); // RX, TX

int motor1=0; //left
int motor2=0; //right
int time;

struct control {
long left;
long right;
long time;
};

void callback(char* topic, uint8_t* payload, unsigned int length) {
  
  struct control *ctr = (struct control *)payload;
  
  motor1 = ctr->left;
  motor2 = ctr->right;
    
  mySerial.print(motor1);
  mySerial.print(" | ");
  mySerial.println(motor2);
}

void setup() {
  Serial.begin(9600);

  pinMode(MOTOR1_PIN1, OUTPUT);
  pinMode(MOTOR1_PIN2, OUTPUT);
  pinMode(MOTOR2_PIN1, OUTPUT);
  pinMode(MOTOR2_PIN2, OUTPUT);

  WiFly.setUart(&Serial);
  WiFly.begin();

  WiFly.join("fotbalrobotic", "dorelrobotel");

  if (client.connect("RoboSoccerBot")) {
    client.publish("status",SW_VERSION " " ROBOT_ID);
    client.subscribe(ROBOT_ID);
  }else{
  }
  
  mySerial.begin(9600);
  mySerial.println("Hello, world?");
}

void loop() {
  go(motor1,motor2);
  client.loop();
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

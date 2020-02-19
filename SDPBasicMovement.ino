#include "SDPArduino.h"
#include <Wire.h>
int i = 0;
#define SensorPin A0
 float sensorValue = 0;

void setup(){
  SDPsetup();
  Serial.println("Connection established!");
  Serial.println("Entering wireless control mode...");
}

void moveForward(int time) {
  String output = "Move Forward 100% for ms: ";
  output += time;
  Serial.println(output);
  motorForward(2, 50);
  motorBackward(3, 50);
  motorForward(4, 50);
  motorBackward(5, 50);
  delay(time);
  motorAllStop();
}

void moveBackward(int time) {
  String output = "Move Backward 100% for ms: ";
  output += time;
  Serial.println(output);
  motorBackward(2, 50);
  motorForward(3, 50);
  motorBackward(4, 50);
  motorForward(5, 50);
  delay(time);
  motorAllStop();
}

void moveLeft(int time) {
  String output = "Move left 100% for ms: ";
  output += time;
  Serial.println(output);
  motorForward(2, 50);
  motorBackward(4, 50);
  motorForward(5, 50);
  motorBackward(3, 50);
  delay(time);
  motorAllStop();
}

void moveRight(int time) {
  String output = "Move right 100% for ms: ";
  output += time;
  Serial.println(output);
  motorBackward(2, 50);
  motorForward(4, 50);
  motorBackward(5, 50);
  motorForward(3, 50);
  delay(time);
  motorAllStop();
}

void lowerArm()
{
	Serial.println("Motor 0 Forward 100%");
	motorBackward(0, 50);
	delay(700);
	motorStop(0);
}

void raiseArm()
{
	Serial.println("Motor 0 Backwards 100%");
	motorForward(0, 50);
	delay(700);
	motorStop(0);
}


void loop(){
  
        // Wait till for start command
        while(!Serial.find("start"));
	int timeMove = 2500;
        int moveDelay = 2500;
        
        // Movement tests
	moveForward(timeMove);
        delay(moveDelay);
        moveBackward(timeMove);
        delay(moveDelay);
        //moveLeft(timeMove);
        //delay(moveDelay);
        //moveRight(timeMove);
        //delay(moveDelay);
        
        // Lower sensor arm
        lowerArm();
        delay(1000);
        
        // Take 5 moisture senor readings across 5 seconds
        for (int i = 0; i <= 4; i++) {
          sensorValue = analogRead(SensorPin);
          Serial.println(sensorValue);
          delay(1000);
        }
    
        // Raise sensor arm
        raiseArm();
        motorAllStop();
}


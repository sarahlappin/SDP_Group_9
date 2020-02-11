#include "SDPArduino.h"
#include <Wire.h>

#define SENSOR_PIN A0

#define SENSOR_DEPLOYMENT_MOTOR 0
//#define UNUSED_MOTOR 1
#define FRONT_LEFT_MOTOR 2
#define FRONT_RIGHT_MOTOR 3
#define BACK_LEFT_MOTOR 4
#define BACK_RIGHT_MOTOR 5

#define SENSOR_DEPLOYMENT_POLARITY 0
#define UNUSED_POLARITY 0
#define FRONT_LEFT_POLARITY 1
#define FRONT_RIGHT_POLARITY 0
#define BACK_LEFT_POLARITY 1
#define BACK_RIGHT_POLARITY 

#define SAMPLING_NUMBER 500 //number of samples to take, average and then return
#define SAMPLING_DELAY 1 //wait 1ms between each sample

#define MOTOR_POWER_LEVEL 50
#define SENSOR_DEPLOYMENT_TIME 700 //time motors should turn for to deploy and retract mechanism

bool safeToDeploySensors;
int[] motorPolarites = [SENSOR_DEPLOYMENT_POLARITY, UNUSED_POLARITY, FRONT_LEFT_POLARITY, FRONT_RIGHT_POLARITY,
                        BACK_LEFT_POLARITY, BACK_RIGHT_POLARITY];

void setup(){
  SDPsetup();
  Serial.println("Connection established!");
  Serial.println("Entering wireless control mode...");
  safeToDeploySensors = true; //ready to deploy
}

void setMoveForward(int motorNumber) { //sets the motor to go forward taking into account polarity
  if (motorPolarities[motorNumber] == 1) {
    motorForward(motorNumber, MOTOR_POWER_LEVEL);
  }
  else {
    motorBackward(motorNumber, MOTOR_POWER_LEVEL);
  }
}

void setMoveBackward(int motorNumber) { //sets the motor to go backward taking into account polarity
  if (motorPolarities[motorNumber] == 1) {
    motorBackward(motorNumber, MOTOR_POWER_LEVEL);
  }
  else {
    motorForward(motorNumber, MOTOR_POWER_LEVEL);
  }
}

void moveForward(int time) {
  safeToDeploySensors = false;

  Serial.println("Move forward");

  setMoveForward(FRONT_LEFT_MOTOR);
  setMoveForward(FRONT_RIGHT_MOTOR);
  setMoveForward(BACK_LEFT_MOTOR);
  setMoveForward(BACK_RIGHT_MOTOR);
  delay(time);
  motorAllStop();
  safeToDeploySensors = true;
}

void moveBackward(int time) {
  safeToDeploySensors = false;

  Serial.println("Moving backward");

  setMoveBackward(FRONT_LEFT_MOTOR);
  setMoveBackward(FRONT_RIGHT_MOTOR);
  setMoveBackward(BACK_LEFT_MOTOR);
  setMoveBackward(BACK_RIGHT_MOTOR);
  delay(time);
  motorAllStop();
  safeToDeploySensors = true;
}

void turnLeft(int time) {
  safeToDeploySensors = false;

  Serial.println("Turning left");

  setMoveForward(FRONT_LEFT_MOTOR);
  setMoveForward(BACK_LEFT_MOTOR);
  setMoveBackward(FRONT_RIGHT_MOTOR);
  setMoveBackward(BACK_RIGHT_MOTOR);
  delay(time);
  motorAllStop();
  safeToDeploySensors = true;
}

void turnRight(int time) {
  safeToDeploySensors = false;

  Serial.println("Turning right");
  
  setMoveBackward(FRONT_LEFT_MOTOR);
  setMoveBackward(BACK_LEFT_MOTOR);
  setMoveForward(FRONT_RIGHT_MOTOR);
  setMoveForward(BACK_RIGHT_MOTOR);
  delay(time);
  motorAllStop();
  safeToDeploySensors = true;
}

void ensureSafeToDeploy() { //software interlock that stops robot movement to deploy sensors
  motorAllStop(); //Ensures that no motors are running so we know it is safe to deploy
  safeToDeploySensors = true;
}

void lowerArm()
{
	Serial.println("Sensor deployment lowering");
  ensureSafeToDeploy();
  if (safeToDeploySensors) {
    setMoveForward(SENSOR_DEPLOYMENT_MOTOR);
    delay(SENSOR_DEPLOYMENT_TIME);
    motorStop(SENSOR_DEPLOYMENT_MOTOR);
  }
  else {
    Serial.println("Could not deploy sensor as the robot is still moving.")
  }
}

void raiseArm()
{
	Serial.println("Sensor deployment mechanism rising");
  ensureSafeToDeploy();
  if (safeToDeploySensors) {
    setMoveBackward(SENSOR_DEPLOYMENT_MOTOR);
    delay(SENSOR_DEPLOYMENT_TIME);
    motorStop(SENSOR_DEPLOYMENT_MOTOR);
  }
  else {
    Serial.println("Could not deploy sensor as the robot is still moving.")
  }
}

double getMoistureReading() {
    Serial.println("Taking moisture reading");

    float sensorValueTotal;
    for (int i = 0; i < SAMPLING_NUMBER; i++) {
      sensorValueTotal = analogRead(SENSOR_PIN);
      Serial.println(sensorValue);
      delay(SAMPLING_DELAY);
    }

    Serial.println("Average moisture reading taken")

    return (double) sensorValueTotal / (double) SAMPLING_NUMBER;
}

void runTestSequence() {
  int timeMove = 2500;
  int moveDelay = 2500;
  
  // Movement tests
  moveForward(timeMove);
  delay(moveDelay);
  moveBackward(timeMove);
  delay(moveDelay);
  turnLeft(timeMove);
  delay(moveDelay);
  turnRight(timeMove);
  delay(moveDelay);
  
  // Lower sensor arm
  lowerArm();
  delay(1000);
  
  Serial.println(getMoistureReading());

  // Raise sensor arm
  raiseArm();
  motorAllStop();
}

void loop(){
  
        // Wait till for start command
        while(!Serial.find("start test"));
        runTestSequence();
}

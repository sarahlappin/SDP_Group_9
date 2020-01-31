#include "SDPArduino.h"
#include <Wire.h>
int i = 0;

void setup(){
  SDPsetup();
  helloWorld();
}

void moveForward(int time) {
  Serial.println("Move Forward 100% for ms: " + time);
  motorForward(2, 100);
  motorBackward(3, 100);
  motorForward(4, 100);
  motorBackward(5, 100);
  delay(time);
  motorAllStop();
}

void moveBackward(int time) {
  Serial.println("Move Backward 100% for ms: " + time);
  motorBackward(2, 100);
  motorForward(3, 100);
  motorBackward(4, 100);
  motorForward(5, 100);
  delay(time);
  motorAllStop();
}

void loop(){
  // Move Forwards for 2.5 secs
moveForward(2500);

Serial.println("All Motors Stop");
motorAllStop();
delay(100);

  //Move backwards for 2.5 secs
moveBackward(2500);

Serial.println("All Motors Stop");
motorAllStop();
delay(100);

// Lower poker
Serial.println("Motor 0 Forward 100%");
motorBackward(0, 100);
delay(2000);
//Raise poker
Serial.println("Motor 0 Backwards 100%");
motorForward(0, 100);
delay(2000);
Serial.println("Motor 0 Stop");
motorStop(0);

//Wait for restart
delay(2500);
}

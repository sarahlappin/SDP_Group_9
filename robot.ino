#include "SDPArduino.h"
#include <Wire.h>
#include <string.h>
#include "NewPing.h"
#include "ITG3200.h" // Gyroscope import

#define MOISTURE_SENSOR_PIN A0
#define C0_SENSOR_PIN A1

#define BAUD_RATE 115200

#define clickerPin 6    // the number of the clicker digital input pin

//mapping of motors on motor board
#define SENSOR_DEPLOYMENT_MOTOR 1
//#define UNUSED_MOTOR 0
#define FRONT_LEFT_MOTOR 2
#define FRONT_RIGHT_MOTOR 4
#define BACK_LEFT_MOTOR 5
#define BACK_RIGHT_MOTOR 3

//mapping of the direction that is considered forward by the motors
#define SENSOR_DEPLOYMENT_POLARITY 1
#define UNUSED_POLARITY 0
#define FRONT_LEFT_POLARITY 1
#define FRONT_RIGHT_POLARITY 0
#define BACK_LEFT_POLARITY 1
#define BACK_RIGHT_POLARITY 0

//number of samples to take, average and then return
#define SAMPLING_NUMBER 500
//wait 1ms between each sample
#define SAMPLING_DELAY 1

#define MOTOR_POWER_LEVEL 30
#define DEPLOYMENT_MOTOR_UP_POWER 60
#define DEPLOYMENT_MOTOR_DOWN_POWER 30

//Turning power
#define TURNING_FORWARDS_POWER 70 // for motors going forwards
#define TURNING_BACKWARDS_POWER -12 // for motors going backwards

//time motors should turn for to retract mechanism
#define SENSOR_RETRACT_TIME 1500

//number of attempts to send a sample before throwing error
#define MAX_MESSAGE_TIMEOUT 10
#define MESSAGE_TIME_DELAY 50

#define MAX_NUMBER_OF_REQUESTS 5

//number of seconds to move for each of the moves in manual mode
#define MOVEMENT_TIME 1

#define NOT_KNOWN "unknown"

#define MAX_DISTANCE_ERROR 35 // Margin of error for distance
#define MAX_ANGLE_ERROR 15    // Margin of error for robot angle in Degrees

// Starting angle in degrees
#define START_ANGLE 90 

#define TRIGGER_PIN 5    // defines trigger pin for ultrasonic sensor
#define ECHO_PIN A2    // defines echo pin for ultrasonic sensor
#define MAX_DISTANCE 200  // defines maximum distance at 20cm

using namespace std;

bool safeToDeploySensors;
int motorPolarities[6] = {UNUSED_POLARITY, SENSOR_DEPLOYMENT_POLARITY, FRONT_LEFT_POLARITY, FRONT_RIGHT_POLARITY, BACK_LEFT_POLARITY, BACK_RIGHT_POLARITY};
ITG3200 gyro;


//NewPing setup of pins and maximum distance
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); 

class GPSGridCoordinate {
    private:
        double latitude;
        double longitude;

    public:
        GPSGridCoordinate(double lat, double longi) {
            latitude = lat;
            longitude = longi;
        }

        GPSGridCoordinate(String coordinateString) {
            latitude = coordinateString.substring(0, coordinateString.indexOf(',')).toFloat();
            longitude = coordinateString.substring(coordinateString.indexOf(',') + 1, coordinateString.length()).toFloat();
        }

        double getLatitudeX() {
            return latitude;
        }

        double getLongitudeY() {
            return longitude;
        }

        void setLatitude(double val) {
            latitude = val;
        }

        void setLongitude(double val) {
            longitude = val;
        }
        
        String toCSV() {
            String lat = String(getLatitudeX());
            String longi = String(getLongitudeY());
            lat += ",";
            lat += longi;
            return lat;
        }
};

class Robot {
    private:

        double current_angle;
        
        void setMoveForward(int motorNumber, int power) { //sets the motor to go forward taking into account polarity
            if (motorPolarities[motorNumber] == 1) {
                motorForward(motorNumber, power);
            }
            else {
                motorBackward(motorNumber, power);
            }
        }

        void setMoveBackward(int motorNumber, int power) { //sets the motor to go backward taking into account polarity
            if (motorPolarities[motorNumber] == 1) {
                motorBackward(motorNumber, power);
            }
            else {
                motorForward(motorNumber, power);
            }
        }

        void ensureSafeToDeploy() { //software interlock that stops robot movement to deploy sensors
            motorAllStop(); //Ensures that no motors are running so we know it is safe to deploy
            safeToDeploySensors = true;
        }

        bool isLowered() {

          int reading = digitalRead(clickerPin);
          Serial.println(reading);
          // if the input just went from LOW and HIGH and we've waited long enough
          // to ignore any noise on the circuit, toggle the output pin and remember
          // the time
          if (reading == HIGH) {
              return true;   
          }
          return false;
        }
        
        void lowerArm() {

            if (safeToDeploySensors) {
                setMoveForward(SENSOR_DEPLOYMENT_MOTOR, DEPLOYMENT_MOTOR_DOWN_POWER);
                Serial.println("Sensor deployment lowering");

                while (!isLowered()) {
                  delay(10);
                }
                
                motorStop(SENSOR_DEPLOYMENT_MOTOR);
            }
            else {
                Serial.println("Could not deploy sensor as the robot is still moving.");
            }
        }

        void raiseArm() {

            if (safeToDeploySensors) {
                setMoveBackward(SENSOR_DEPLOYMENT_MOTOR, DEPLOYMENT_MOTOR_UP_POWER);
                Serial.println("Sensor deployment rising");
                delay(SENSOR_RETRACT_TIME);
                motorStop(SENSOR_DEPLOYMENT_MOTOR);
            }
            else {
                Serial.println("Could not retract sensor as the robot is still moving.");
            }
        }

        double getMoistureReading() {
            Serial.println("Taking moisture reading");

            float sensorValueTotal = 0;
            for (int i = 0; i < SAMPLING_NUMBER; i++) {
                sensorValueTotal += analogRead(MOISTURE_SENSOR_PIN);
                delay(SAMPLING_DELAY);
            }

            Serial.println("Average moisture reading taken");

            return (double) sensorValueTotal / (double) SAMPLING_NUMBER;
        }

        double getC0Reading() {
            Serial.println("Taking C0 reading");

            float sensorValueTotal = 0;
            for (int i = 0; i < SAMPLING_NUMBER; i++) {
                sensorValueTotal += analogRead(C0_SENSOR_PIN);
                delay(SAMPLING_DELAY);
            }

            Serial.println("Average C0 reading taken");

            return (double) sensorValueTotal / (double) SAMPLING_NUMBER;
        }

        String getSurveyDetailsString(int attemptNumber) {
            if (attemptNumber > MAX_NUMBER_OF_REQUESTS) {
                Serial.println("Max number of survey requests recieved with no matching responses");
                return NOT_KNOWN;
            }

            int timeoutCounter = 0;
            
            Serial.println("<getSurvey/>"); //sends to the user
            delay(MESSAGE_TIME_DELAY); //give some time before retransmitting

            String openingTag = "<survey>";
            String closingTag = "</survey>";
            int positionCounter;

            //finds the first tag
            while (positionCounter <= openingTag.length() && timeoutCounter <= MAX_MESSAGE_TIMEOUT) {
                if (Serial.available()) { //characters are being sent
                    char character = Serial.read(); //read one char
                    if(character == openingTag[positionCounter]) {
                        positionCounter++;
                        timeoutCounter = 0;
                    }
                }
                timeoutCounter++;
            }

            if (timeoutCounter > MAX_MESSAGE_TIMEOUT) { Serial.println("Timeout reached when looking for survey details."); return askForLocation(attemptNumber++);}

            //get the first of the survey details string
            String surveyDetails1 = "";
            while (timeoutCounter <= MAX_MESSAGE_TIMEOUT) {
                if (Serial.available()) { //characters are being sent
                    char character = Serial.read(); //read one char
                    if((character >= '0' && character <= '9') || character == '.' || character == ',') {
                        surveyDetails1 = surveyDetails1 + character;
                    }
                    else {
                        timeoutCounter = 0;
                        break;
                    }
                }
                timeoutCounter++;
            }
            
            if (timeoutCounter > MAX_MESSAGE_TIMEOUT) { 
              Serial.println("Timeout reached when looking for survey details."); 
              return askForLocation(attemptNumber++);
            }

            //get the second of the survey details string
            String surveyDetails2 = "";
            while (timeoutCounter <= MAX_MESSAGE_TIMEOUT) {
                if (Serial.available()) { //characters are being sent
                    char character = Serial.read(); //read one char
                    if((character >= '0' && character <= '9') || character == '.' || character == ',') {
                        surveyDetails2 = surveyDetails2 + character;
                    }
                    else {
                        timeoutCounter = 0;
                        break;
                    }
                }
                timeoutCounter++;
            }

            if (timeoutCounter > MAX_MESSAGE_TIMEOUT) { 
              Serial.println("Timeout reached when looking for survey details"); 
              return askForLocation(attemptNumber++);
            }

            if (surveyDetails1.equals(surveyDetails2)) { //if they're the same
                return surveyDetails1;
            }
            else {
                //try again
                return getSurveyDetailsString(attemptNumber++);
            }
        }

        String askForLocation(int attemptNumber) {
            if (attemptNumber > MAX_NUMBER_OF_REQUESTS) {
                Serial.println("Max number of GPS requests recieved with no matching responses");
                return NOT_KNOWN;
            }
            
            int timeoutCounter = 0;
            
            Serial.println("<getLocation/>"); //sends to the user
            delay(MESSAGE_TIME_DELAY); //give some time before retransmitting

            String openingTag = "<location>";
            String closingTag = "</location>";
            int positionCounter = 0;

            //finds the first tag
            int positionInMessage = 0;
            char character;
            String coordinate1 = "";
            String coordinate2 = "";
            String fullMessage = "";
            
            while (positionInMessage <= 2 && timeoutCounter <= MAX_MESSAGE_TIMEOUT) {
                if (Serial.available()) { //characters are being sent
                    character = Serial.read(); //read one char
                    fullMessage += character;

                    // First tag
                    if (positionInMessage == 0) {
                        if(character == openingTag[positionCounter]) {
                            positionCounter++;
                            timeoutCounter = 0;
                        }
                        if (positionCounter >= openingTag.length()) {
                            positionInMessage = 1;
                        }

                        // coordinates
                    } else if (positionInMessage < 3 && ((character >= '0' && character <= '9') || character == '.' || character == ',' || character == '-')) { //next set of coordinates begin
                        
                        if (positionInMessage == 1) {
                            coordinate1 = coordinate1 + character;
                        }
                        else if (positionInMessage == 2) {
                            coordinate2 = coordinate2 + character;
                        }
                        
                    } else if (character == '|') {                      
                        positionInMessage = 2;
                    } else if (character == '<') {
                        positionInMessage = 3;
                    } 
                }
                
                delay(MESSAGE_TIME_DELAY);
                timeoutCounter++;
            }
            
            if (timeoutCounter > MAX_MESSAGE_TIMEOUT){
              Serial.println("Getting location timed out");
            }
            else {
              if (coordinate1.equals(coordinate2)) { //if they're the same
                  Serial.println(coordinate1);
                  return coordinate1;
              }
              else {
                  //try again
                  return askForLocation(attemptNumber++);
              }
            }
        }

        double askForAngle(int attemptNumber) {
            if (attemptNumber > MAX_NUMBER_OF_REQUESTS) {
                Serial.println("Max number of angle requests recieved with no matching responses");
                return 0;
            }
            
            int timeoutCounter = 0;
            
            Serial.println("<getAngle/>"); //sends to the user
            delay(MESSAGE_TIME_DELAY); //give some time before retransmitting

            String openingTag = "<angle>";
            String closingTag = "</angle>";
            int positionCounter = 0;

            //finds the first tag
            int positionInMessage = 0;
            char character;
            String coordinate1 = "";
            String coordinate2 = "";
            String fullMessage = "";
            
            while (positionInMessage <= 2 && timeoutCounter <= MAX_MESSAGE_TIMEOUT) {
                if (Serial.available()) { //characters are being sent
                    character = Serial.read(); //read one char
                    fullMessage += character;

                    // First tag
                    if (positionInMessage == 0) {
                        if(character == openingTag[positionCounter]) {
                            positionCounter++;
                            timeoutCounter = 0;
                        }
                        if (positionCounter >= openingTag.length()) {
                            positionInMessage = 1;
                        }

                        // coordinates
                    } else if (positionInMessage < 3 && ((character >= '0' && character <= '9') || character == '.'|| character == '-')) { //next set of coordinates begin
                        
                        if (positionInMessage == 1) {
                            coordinate1 = coordinate1 + character;
                        }
                        else if (positionInMessage == 2) {
                            coordinate2 = coordinate2 + character;
                        }
                        
                    } else if (character == '|') {                      
                        positionInMessage = 2;
                    } else if (character == '<') {
                        positionInMessage = 3;
                    } 
                }
                
                delay(MESSAGE_TIME_DELAY);
                timeoutCounter++;
            }
            
            if (timeoutCounter > MAX_MESSAGE_TIMEOUT){
              Serial.println("Getting angle timed out");
            }
            else {
              if (coordinate1.equals(coordinate2)) { //if they're the same
                  Serial.println(coordinate1);
                  return coordinate1.toDouble();
              }
              else {
                  //try again
                  return askForAngle(attemptNumber++);
              }
            }
        }

        void sendSample(unsigned long time, GPSGridCoordinate sampleLocation, double moistureValue, double c0Reading) { //should this be void? Maybe return an error?
            String sampleText = String(time);
            sampleText += ",";
            sampleText += sampleLocation.toCSV();
            sampleText += ",";
            sampleText += String(moistureValue);
            
            //if you are reading this comment then don't judge - Arduino is awkward when it comes to strings
            //please do better next time
            
            String messageText = "<sample>";
            messageText += sampleText;
            messageText += "|";
            messageText += sampleText;
            messageText += "</sample>"; //sends twice and keeps sending until DICE confirms that it has recieved

            Serial.println(messageText); //sends to the user
            delay(MESSAGE_TIME_DELAY); //give some time before retransmitting

            int timeoutCounter = 0;
            while (timeoutCounter <= MAX_MESSAGE_TIMEOUT && !Serial.find("<sampleRecieved/>")) {
                Serial.println(messageText); //try again
                delay(MESSAGE_TIME_DELAY); //give some time before retransmitting
                timeoutCounter++;
            }

            //Check if it was transmitted
            if (timeoutCounter <= MAX_MESSAGE_TIMEOUT) { //failed to send - timeout
                Serial.println("Failed to transmit sample: " + sampleText);
            }
            else {
                Serial.println("Managed to transmit sample.");
            }
        }

        double calculateDistance(double startX, double startY, double destX, double destY) {
            return sqrt(pow(destX - startX, 2) + pow(destY - startY, 2));
        }

        double get_angle_clockwise_from_north(double startX, double startY, double destX, double destY) 
		{
            return atan2(destY - startY, destX - startX)/M_PI * 180;
        }

    public:

        Robot() {
            current_angle = START_ANGLE;
        }
  
          GPSGridCoordinate* getLocation() { //method would be replaced with a GPS module in final product
              String coordinateString = askForLocation(0);
              if (coordinateString.equals(NOT_KNOWN)) {
                  return new GPSGridCoordinate(0, 0); //no idea where you are
              }
              else {
                  return new GPSGridCoordinate(coordinateString);
              }
          }
          double getAngle() { //method would be replaced with a GPS module in final product
              return askForAngle(0);
          }

          void printPositionDetails(double startX, double startY, double destX, double destY)
          {
              Serial.print("Current Position: ");
              Serial.print(startX);
              Serial.print(", ");
              Serial.print(startY);
          
              Serial.print("\tTarget Position: ");
              Serial.print(destX);
              Serial.print(", ");
              Serial.println(destY);
          }

          void printAngleDetails(double angleRobot, double angleTarget, double angleDiff)
          {
              Serial.print("Current Angle: ");
              Serial.print(angleRobot);
              
              Serial.print("\tTarget Angle: ");
              Serial.print(angleTarget);

              Serial.print("\tAngle Difference: ");
              Serial.println(angleDiff);
          }
		
        void move(double destX, double destY)
        {    
            // Use vision system to get robot coordinates
            GPSGridCoordinate* robot_pos = getLocation();

            double startX = robot_pos->getLatitudeX();
            double startY = robot_pos->getLongitudeY();

            Serial.println("Starting Movement:");
            printPositionDetails(startX, startY, destX, destY);

      			// Calculate the distance between the positions
            double distance = calculateDistance(startX, startY, destX, destY);

            while (distance > MAX_DISTANCE_ERROR) 
			      {
                double angleTarget = get_angle_clockwise_from_north(startX, startY, destX, destY);                
                double robotAngle = getAngle();
                double angleDifference = angleTarget - robotAngle;
                printAngleDetails(robotAngle, angleTarget, angleDifference);

                while (abs(angleDifference) > MAX_ANGLE_ERROR) 
                {
                    if (angleDifference > 180 || ( angleDifference > -180 && angleDifference < 0)) turnLeft(500);
                    else turnRight(500);
                    robotAngle = getAngle();
               
                    angleTarget = get_angle_clockwise_from_north(startX, startY, destX, destY);                
                    angleDifference = angleTarget - robotAngle;
                    printAngleDetails(robotAngle, angleTarget, angleDifference);

                    moveForward(10000 / abs(angleDifference) + 1);
                }
                //update location and distance via vision system
                robot_pos = getLocation();
                startX = robot_pos->getLatitudeX();
                startY = robot_pos->getLongitudeY();
                
                distance = calculateDistance(startX, startY, destX, destY);
                printPositionDetails(startX, startY, destX, destY);
            }

        }

        bool objectDetected() {

            unsigned int distance = sonar.ping_cm();
            //Serial.print(distance);
            //Serial.print("cm ");

            if (distance < 15 && distance > 0) {
                return true;
            } else {
                return false;
            }
        }
        
        void moveForward(int time) {
            if (safeToDeploySensors) {
                safeToDeploySensors = false;
            
                Serial.println("Move forward");
            
                setMoveForward(FRONT_LEFT_MOTOR, MOTOR_POWER_LEVEL);
                setMoveForward(FRONT_RIGHT_MOTOR, MOTOR_POWER_LEVEL);
                setMoveForward(BACK_LEFT_MOTOR, MOTOR_POWER_LEVEL);
                setMoveForward(BACK_RIGHT_MOTOR, MOTOR_POWER_LEVEL);

                
                for (int i = 0; i < time / 10; i++) {           
                    if (objectDetected()) {
                        ensureSafeToDeploy();
                        return;
                    } else {
                        delay(10);
                    }
                } 

            }
            else {
                Serial.println("Could not move forward as sensor arm is still deployed");
            }

            ensureSafeToDeploy();
        }

        void moveBackward(int time) {
            if (safeToDeploySensors) {
                safeToDeploySensors = false;
            
                Serial.println("Moving backward");
            
                setMoveBackward(FRONT_LEFT_MOTOR, MOTOR_POWER_LEVEL);
                setMoveBackward(FRONT_RIGHT_MOTOR, MOTOR_POWER_LEVEL);
                setMoveBackward(BACK_LEFT_MOTOR, MOTOR_POWER_LEVEL);
                setMoveBackward(BACK_RIGHT_MOTOR, MOTOR_POWER_LEVEL);
                delay(time);
            }
            else {
                Serial.println("Could not move backward as sensor arm is still deployed");
            }

            ensureSafeToDeploy();
        }

        void testMotors() {
            if (safeToDeploySensors) {
                safeToDeploySensors = false;           
            
                setMoveForward(FRONT_LEFT_MOTOR, MOTOR_POWER_LEVEL);
                delay(1000);
                motorAllStop();
                delay(1000);
                setMoveForward(FRONT_RIGHT_MOTOR, MOTOR_POWER_LEVEL);
                delay(1000);
                motorAllStop();
                delay(1000);
                setMoveForward(BACK_LEFT_MOTOR, MOTOR_POWER_LEVEL);
                delay(1000);
                motorAllStop();
                delay(1000);
                setMoveForward(BACK_RIGHT_MOTOR, MOTOR_POWER_LEVEL);
                delay(1000);
                motorAllStop();
                delay(1000);
                setMoveForward(FRONT_LEFT_MOTOR, MOTOR_POWER_LEVEL);
                setMoveForward(FRONT_RIGHT_MOTOR, MOTOR_POWER_LEVEL);
                setMoveForward(BACK_LEFT_MOTOR, MOTOR_POWER_LEVEL);
                setMoveForward(BACK_RIGHT_MOTOR, MOTOR_POWER_LEVEL);
                delay(1000);
                motorAllStop();
                delay(5000);
            }
            else {
                Serial.println("Could not move backward as sensor arm is still deployed");
            }

            ensureSafeToDeploy();
        }

        void testTurning() {

            if (safeToDeploySensors) {          
            
                turnLeft(3000);
                delay(3000);
                turnRight(3000);
                delay(3000);
            }
            else {
                Serial.println("Could not test turning as sensor arm is still deployed");
            }

            ensureSafeToDeploy();
        }

        void testArmMotor() {

            Serial.println("Testing arm motors");
            lowerArm();
            delay(2000);
            raiseArm();
            delay(2000);
            Serial.println("Stopping testing arm motors");
        }
        
        void turnRight(int time) {
            if (safeToDeploySensors) {
                safeToDeploySensors = false;
            
                Serial.println("Turning right");
            
                setMoveForward(FRONT_LEFT_MOTOR, TURNING_FORWARDS_POWER);
                setMoveForward(BACK_LEFT_MOTOR, TURNING_FORWARDS_POWER);
                setMoveBackward(FRONT_RIGHT_MOTOR, TURNING_BACKWARDS_POWER);
                setMoveBackward(BACK_RIGHT_MOTOR, TURNING_BACKWARDS_POWER);
                delay(time);
            }
            else {
                Serial.println("Could not turn right as sensor arm is still deployed");
            }

            ensureSafeToDeploy();
        }

        void turnLeft(int time) {
            if (safeToDeploySensors) {
                safeToDeploySensors = false;
            
                Serial.println("Turning left");
                
                setMoveBackward(FRONT_LEFT_MOTOR, TURNING_BACKWARDS_POWER);
                setMoveBackward(BACK_LEFT_MOTOR, TURNING_BACKWARDS_POWER);
                setMoveForward(FRONT_RIGHT_MOTOR, TURNING_FORWARDS_POWER);
                setMoveForward(BACK_RIGHT_MOTOR, TURNING_FORWARDS_POWER);
                delay(time);
                motorAllStop();
            }
            else {
                Serial.println("Could not turn left as sensor arm is still deployed");
            }

            ensureSafeToDeploy();
        }

        void takeSamples() { //method takes all samples and return them to the DICE machine
            Serial.println("Starting sampling");
            lowerArm();
            unsigned long time = millis(); //timestamp
            GPSGridCoordinate *sampleLocation = getLocation();
            double moistureValue = getMoistureReading();
            double c0Reading = getC0Reading();
            sendSample(time, *sampleLocation, moistureValue, c0Reading);
            //this will likely need rewritten so that samples can be taken concurrently
            //check if Arduino is actually more efficient doing this before making concurrent
            raiseArm();
            
            delete(sampleLocation);
        }

        void runSurvey(GPSGridCoordinate startPosition, GPSGridCoordinate endPosition, double samplingFrequency) {
            Serial.println("Starting survey...");
            Serial.print("Start position: ");
            Serial.println(startPosition.toCSV());
            
            Serial.print("End position: ");
            Serial.println(endPosition.toCSV());
            
            Serial.print("Sampling frequency: ");
            Serial.println(samplingFrequency);
            
            //Need to implement this function
            //
            //
            //
            //
            //
            //
            Serial.println("Function still to be implemented");
            runTestSequence();
            //
            //
            //
            //
            //
            //
            //
            //
        }

        void initiateSurvey() {
            String surveyRequest = getSurveyDetailsString(0); //get survey details (starting at attempt 0)
            if (!surveyRequest.equals(NOT_KNOWN)) {
                //parse the survey string
                int startLatPos  = 0;
                int startLongPos = surveyRequest.indexOf(',');
                int endLatPos    = surveyRequest.indexOf(',', startLongPos);
                int endLongPos   = surveyRequest.indexOf(',', endLatPos);
                int samplingFrequencyPos = surveyRequest.indexOf(',', endLongPos);

                if (endLatPos > 0 && startLongPos > 0 && endLongPos > 0 && samplingFrequencyPos > 0) { //ensure that there are enough values
                    String startLat  = surveyRequest.substring(startLatPos, startLongPos);
                    String startLong = surveyRequest.substring(startLongPos, endLatPos);
                    String endLat    = surveyRequest.substring(endLatPos, endLongPos);
                    String endLong   = surveyRequest.substring(endLongPos, samplingFrequencyPos);
                    String samplingFrequency = surveyRequest.substring(samplingFrequencyPos, surveyRequest.length());

                    Serial.println("Survey data given was as follows: ");
                    Serial.println(startLat);
                    Serial.println(startLong);
                    Serial.println(endLat);
                    Serial.println(endLong);
                    Serial.println(samplingFrequency);

                    GPSGridCoordinate* startPosition = new GPSGridCoordinate(startLat.toFloat(), startLong.toFloat());
                    GPSGridCoordinate* endPosition   = new GPSGridCoordinate(endLat.toFloat(), endLong.toFloat());
                    runSurvey(*startPosition, *endPosition, samplingFrequency.toFloat()); //begin surveying
                    delete(startPosition);
                    delete(endPosition);
                }
                else {
                    Serial.println("Could not start survey as not enough parameters were given. Should be given: start latitude, start longitude, end latitude, end longitude, sampling frequency");
                }
            }
            else {
                Serial.println("Could not start survey as details were not sent by workstation.");
            }
        }

        void runTestSequence() {
            Serial.println("Running test sequence...");
            
            // Movement tests
            moveForward(2500);

            //check for objects
              if (objectDetected()) {
              
                moveBackward(2000);
                delay(1000);
                turnLeft(3000);
                delay(1000);
                //turnRight(3000);
                //delay(1000);
                
                Serial.println("detected object!");
              }

            
            // Lower sensor arm
            lowerArm();
            delay(2000);
            
            //Serial.println(getMoistureReading());

            // Raise sensor arm
            raiseArm();
            motorAllStop();
            delay(5000);
        } 
};

Robot *robot;
int counter;

void setup(){
    SDPsetup();
    Serial.begin(BAUD_RATE);
    delay(2000);
    //gyro.init();
    //gyro.zeroCalibrate(200, 10); //sample 200 times to calibrate and it will take 200*10ms
    Serial.println("Connection established!");
    Serial.println("Entering wireless control mode...");
    safeToDeploySensors = true; //ready to deploy
    robot = new Robot();
    counter = 0;
}



void loop(){


        
        //robot->takeSamples();
        Serial.println("Start");
        robot->move(0.1, 0.1);
        Serial.println("Finish");
        //delay(5000);

        
        Serial.println("Start");
        robot->move(0, 350);
        Serial.println("Finish");


        //robot->runTestSequence();
        //robot->testArmMotor();
}

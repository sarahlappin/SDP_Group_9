#include "SDPArduino.h"
#include <Wire.h>
#include <string.h>
#include "NewPing.h"
#include "ITG3200.h" // Gyroscope import

#define SENSOR_PIN A0

#define BAUD_RATE 9600

//mapping of motors on motor board
#define SENSOR_DEPLOYMENT_MOTOR 0
//#define UNUSED_MOTOR 1
#define FRONT_LEFT_MOTOR 2
#define FRONT_RIGHT_MOTOR 4
#define BACK_LEFT_MOTOR 5
#define BACK_RIGHT_MOTOR 3

//mapping of the direction that is considered forward by the motors
#define SENSOR_DEPLOYMENT_POLARITY 0
#define UNUSED_POLARITY 0
#define FRONT_LEFT_POLARITY 1
#define FRONT_RIGHT_POLARITY 0
#define BACK_LEFT_POLARITY 1
#define BACK_RIGHT_POLARITY 0

//number of samples to take, average and then return
#define SAMPLING_NUMBER 500
//wait 1ms between each sample
#define SAMPLING_DELAY 1

#define MOTOR_POWER_LEVEL 35
#define DEPLOYMENT_MOTOR_POWER 30

//Turning power
#define TURNING_FORWARDS_POWER 70 // for motors going forwards
#define TURNING_BACKWARDS_POWER -12 // for motors going backwards

//time motors should turn for to deploy and retract mechanism
#define SENSOR_DEPLOYMENT_TIME 2000

//number of attempts to send a sample before throwing error
#define MAX_MESSAGE_TIMEOUT 500
#define MESSAGE_TIME_DELAY 50

#define MAX_NUMBER_OF_REQUESTS 5

//number of seconds to move for each of the moves in manual mode
#define MOVEMENT_TIME 1

#define NOT_KNOWN "unknown"

#define MAX_DISTANCE_ERROR 1 // Margin of error for distance
#define MAX_ANGLE_ERROR 1    // Margin of error for robot angle in Degrees

// Starting angle in degrees
#define START_ANGLE 90 

#define TRIGGER_PIN 5    // defines trigger pin for ultrasonic sensor
#define ECHO_PIN A2    // defines echo pin for ultrasonic sensor
#define MAX_DISTANCE 200  // defines maximum distance at 20cm

using namespace std;

bool safeToDeploySensors;
int motorPolarities[6] = {SENSOR_DEPLOYMENT_POLARITY, UNUSED_POLARITY, FRONT_LEFT_POLARITY, FRONT_RIGHT_POLARITY, BACK_LEFT_POLARITY, BACK_RIGHT_POLARITY};
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

        double getLatitude() {
            return latitude;
        }

        double getLongitude() {
            return longitude;
        }

        void setLatitude(double val) {
            latitude = val;
        }

        void setLongitude(double val) {
            longitude = val;
        }
        
        String toCSV() {
            String lat = String(getLatitude());
            String longi = String(getLongitude());
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

        void lowerArm() {
            Serial.println("Sensor deployment lowering");
            ensureSafeToDeploy();
            if (safeToDeploySensors) {
                setMoveForward(SENSOR_DEPLOYMENT_MOTOR, DEPLOYMENT_MOTOR_POWER);
                delay(SENSOR_DEPLOYMENT_TIME);
                motorStop(SENSOR_DEPLOYMENT_MOTOR);
            }
            else {
                Serial.println("Could not deploy sensor as the robot is still moving.");
            }
        }

        void raiseArm() {
            Serial.println("Sensor deployment mechanism rising");
            ensureSafeToDeploy();
            if (safeToDeploySensors) {
                setMoveBackward(SENSOR_DEPLOYMENT_MOTOR, DEPLOYMENT_MOTOR_POWER);
                delay(SENSOR_DEPLOYMENT_TIME);
                motorStop(SENSOR_DEPLOYMENT_MOTOR);
            }
            else {
                Serial.println("Could not deploy sensor as the robot is still moving.");
            }
        }

        double getMoistureReading() {
            Serial.println("Taking moisture reading");

            float sensorValueTotal = 0;
            for (int i = 0; i < SAMPLING_NUMBER; i++) {
            sensorValueTotal += analogRead(SENSOR_PIN);
            delay(SAMPLING_DELAY);
            }

            Serial.println("Average moisture reading taken");

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
                    } else if (positionInMessage < 3 && ((character >= '0' && character <= '9') || character == '.' || character == ',')) { //next set of coordinates begin
                        
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

        void sendSample(unsigned long time, GPSGridCoordinate sampleLocation, double moistureValue) { //should this be void? Maybe return an error?
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

//        double calculateDistance(double startX, double startY, double destX, double destY) {
//            sqrt(pow(destX - startX, 2) + pow(destY - startY, 2));
//        }
//
//        double get_angle_clockwise_from_north(double startX, double startY, double destX, double destY) {
//            // calculates angle created by vector from start point to center and vector from start point to destination point
//            // Create vectors from the centre for both positions
//            double LineFromStartToNorth [2] = {startX, (startY + 1)};
//            double lineFromStartToDest [2] = {(destX - startX), (destY - startY)};
//
//            // Calculate the inner angle between them
//            double pq = (LineFromStartToNorth[0] * lineFromStartToDest[0]) + (LineFromStartToNorth[1] * lineFromStartToDest[1]);
//            double magPQ = sqrt(pow(LineFromStartToNorth[0], 2) + pow(LineFromStartToNorth[1], 2)) * sqrt(pow(lineFromStartToDest[0], 2) + pow(lineFromStartToDest[1], 2));   
//            double pqDivMag = pq/magPQ;
//
//            if (magPQ == 0) return -1; // Error for divide by 0
//
//            double angle = acos(pq/magPQ);
//
//            // If destination is right of robot then the inner angle is the bearing from north
//            // else return 360 - that angle for the clockwise bearing from north 
//            if (LineFromStartToNorth[0] <= lineFromStartToDest[0]) {
//                return (angle/M_PI * 180);
//            } else {
//                return 360 - (angle/M_PI * 180);
//            }
//        }

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

//        void move(double destX, double destY)
//        {    
//            // Use vision system to get robot coordinates
//            Serial.println("get gpd");
//            GPSGridCoordinate* robot_pos = getLocation();
//            Serial.println("get gps");
//
//            double startX = robot_pos->getLongitude();
//            double startY = robot_pos->getLatitude();
//            
//            // Calculate the distance between the positions
//            double distance = calculateDistance(startX, startY, destX, destY);
//
//            while (distance > MAX_DISTANCE_ERROR) {
//        
//                // Get the angle to turn (if more than 180 then turn left 180 - angle)
//                double angleStartToDest = get_angle_clockwise_from_north(startX, startY, destX, destY);
//                double angle_to_turn = abs(current_angle - angleStartToDest) >= 180 ? abs(current_angle - angleStartToDest - 180) : abs(current_angle - angleStartToDest);
//                bool turn_left = abs(current_angle - angleStartToDest) >= 180 ? true : false;
//
//                if (turn_left) {
//                    while ( abs(current_angle - angleStartToDest) > MAX_ANGLE_ERROR) {
//                        turnLeft(100);
//                    }
//                } else {
//                    while ( abs(current_angle - angleStartToDest) > MAX_ANGLE_ERROR) {
//                        turnRight(100);
//                    }
//                }
//
//                // move forward, probably needs to be more sophisticated
//                // should be able to handle object detection to stop
//                moveForward(1000); 
//
//                //Update angle using gyroscope
//                float x, y, z;
//                gyro.getAngularVelocity(&x, &y, &z);
//                current_angle += double(z); // Current angle from north is previous plus the change
//
//                //update location and distance via vision system
//                robot_pos = getLocation();
//                startX = robot_pos->getLongitude();
//                startY = robot_pos->getLatitude();
//                distance = calculateDistance(startX, startY, destX, destY);
//
//            }
//
//        }

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
                setMoveForward(FRONT_RIGHT_MOTOR, MOTOR_POWER_LEVEL);
                delay(1000);
                motorAllStop();
                setMoveForward(BACK_LEFT_MOTOR, MOTOR_POWER_LEVEL);
                delay(1000);
                motorAllStop();
                setMoveForward(BACK_RIGHT_MOTOR, MOTOR_POWER_LEVEL);
                delay(1000);
                motorAllStop();
                
                setMoveForward(FRONT_LEFT_MOTOR, MOTOR_POWER_LEVEL);
                setMoveForward(FRONT_RIGHT_MOTOR, MOTOR_POWER_LEVEL);
                setMoveForward(BACK_LEFT_MOTOR, MOTOR_POWER_LEVEL);
                setMoveForward(BACK_RIGHT_MOTOR, MOTOR_POWER_LEVEL);
                delay(1000);
                motorAllStop();
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
            sendSample(time, *sampleLocation, moistureValue);
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

            
            unsigned int distance = sonar.ping_cm();

            //check for objects
              if (objectDetected()) {

                
                moveBackward(2000);
                delay(1000);
                turnLeft(5000);
                delay(1000);
                turnRight(5000);
                delay(1000);
                
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
    gyro.init();
    gyro.zeroCalibrate(200, 10); //sample 200 times to calibrate and it will take 200*10ms
    Serial.println("Connection established!");
    Serial.println("Entering wireless control mode...");
    safeToDeploySensors = true; //ready to deploy
    robot = new Robot();
    counter = 0;
}



void loop(){

        /*
        if (Serial.find("<manual/>")) {
            while(!Serial.find("<endManual/>")) {
                  if (!Serial.find("<stop/>")) {
                    if (Serial.find("<forward/>")) {
                        robot->moveForward(MOVEMENT_TIME);
                    }
                    else if (Serial.find("<backward/>")) {
                        robot->moveBackward(MOVEMENT_TIME);
                    }
                    else if (Serial.find("<right/>")) {
                        robot->turnRight(MOVEMENT_TIME);
                    }
                    else if (Serial.find("<left/>")) {
                        robot->turnLeft(MOVEMENT_TIME);
                    }
                    else if (Serial.find("<takeSample/>")) {
                        robot->takeSamples();
                    }
                }
            }
            
        } 
        
        else if (Serial.find("<testServer/>")) {

            while (1) {

                GPSGridCoordinate *g = robot->getLocation();
                Serial.print(counter);
                Serial.print(": ");
                if (g->getLatitude() == 1.2 && g->getLongitude() == 3.4) {
                    Serial.println("Success");
                }
                else {
                    Serial.println(g->getLongitude());
                }
                counter++;
            }
        }

        
        else if (Serial.find("<test/>")) {
            robot->runTestSequence();
        }
        
        else if (Serial.find("<survey/>")) {
            robot->initiateSurvey();
        }
        
        else {
            Serial.println("Awaiting instructions.");
            robot->moveForward(1000);
            Serial.println("Location:");
            robot->getLocation();
            Serial.println("Location done");
        }*/
        
        robot->runTestSequence();

        //robot->testTurning();
        
        /*while (true) {

            GPSGridCoordinate *g = robot->getLocation();
            Serial.println(g->getLatitude());
            Serial.println(g->getLongitude());
            
            delay(1000);
        }*/
}

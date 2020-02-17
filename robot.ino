#include "SDPArduino.h"
#include <Wire.h>
#include <ctime>
#include <format>
#include <string.h>

#define SENSOR_PIN A0

#define BAUD_RATE 9600

//mapping of motors on motor board
#define SENSOR_DEPLOYMENT_MOTOR 0
//#define UNUSED_MOTOR 1
#define FRONT_LEFT_MOTOR 2
#define FRONT_RIGHT_MOTOR 3
#define BACK_LEFT_MOTOR 4
#define BACK_RIGHT_MOTOR 5

//mapping of the direction that is considered forward by the motors
#define SENSOR_DEPLOYMENT_POLARITY 0
#define UNUSED_POLARITY 0
#define FRONT_LEFT_POLARITY 1
#define FRONT_RIGHT_POLARITY 0
#define BACK_LEFT_POLARITY 1
#define BACK_RIGHT_POLARITY 

//number of samples to take, average and then return
#define SAMPLING_NUMBER 500
//wait 1ms between each sample
#define SAMPLING_DELAY 1

#define MOTOR_POWER_LEVEL 50
//time motors should turn for to deploy and retract mechanism
#define SENSOR_DEPLOYMENT_TIME 700

//number of attempts to send a sample before throwing error
#define MAX_MESSAGE_TIMEOUT 50
#define MESSAGE_TIME_DELAY 5

#define MAX_NUMBER_OF_GPS_REQUESTS 5

#define LOCATION_NOT_KNOWN "unknown"

using namespace std;

bool safeToDeploySensors;
int motorPolarities[6] = {SENSOR_DEPLOYMENT_POLARITY, UNUSED_POLARITY, FRONT_LEFT_POLARITY, FRONT_RIGHT_POLARITY, BACK_LEFT_POLARITY, BACK_RIGHT_POLARITY};


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
            longitude = coordinateString.substring(coordinateString.indexOf(','), coordinateString.length()).toFloat();
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
}

class robot {
    private:
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

        void ensureSafeToDeploy() { //software interlock that stops robot movement to deploy sensors
            motorAllStop(); //Ensures that no motors are running so we know it is safe to deploy
            safeToDeploySensors = true;
        }

        void lowerArm() {
            Serial.println("Sensor deployment lowering");
            ensureSafeToDeploy();
            if (safeToDeploySensors) {
                setMoveForward(SENSOR_DEPLOYMENT_MOTOR);
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
                setMoveBackward(SENSOR_DEPLOYMENT_MOTOR);
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
        String askForLocation(int attemptNumber) {
            if (attemptNumber > MAX_NUMBER_OF_GPS_REQUESTS) {
                Serial.println("Max number of GPS requests recieved with no matching responses");
                return LOCATION_NOT_KNOWN;
            }
            
            int timeoutCounter = 0;
            
            Serial.println("<getLocation/>"); //sends to the user
            delay(MESSAGE_TIME_DELAY); //give some time before retransmitting

            String openingTag = "<location>";
            String closingTag = "</location>";
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

            if (timeoutCounter > MAX_MESSAGE_TIMEOUT) { Serial.println("Timeout reached when looking for GPS data."); return askForLocation(attemptNumber++);}

            //get the first GPS coordinate
            String coordinate1 = "";
            while (timeoutCounter <= MAX_MESSAGE_TIMEOUT) {
                if (Serial.available()) { //characters are being sent
                    char character = Serial.read(); //read one char
                    if((character >= '0' && character <= '9') || character == '.' || character == ',') { //next set of coordinates begin
                        coordinate1 = coordinate1 + character;
                    }
                    else {
                        timeoutCounter = 0;
                        break;
                    }
                }
                timeoutCounter++;
            }

            if (timeoutCounter > MAX_MESSAGE_TIMEOUT) { Serial.println("Timeout reached when looking for GPS data."); return askForLocation(attemptNumber++);;}

            //get the second GPS coordinate
            String coordinate2 = "";
            while (timeoutCounter <= MAX_MESSAGE_TIMEOUT) {
                if (Serial.available()) { //characters are being sent
                    char character = Serial.read(); //read one char
                    if((character >= '0' && character <= '9') || character == '.' || character == ',') { //next set of coordinates begin
                        coordinate2 = coordinate2 + character;
                    }
                    else {
                        timeoutCounter = 0;
                        break;
                    }
                }
                timeoutCounter++;
            }

            if (timeoutCounter > MAX_MESSAGE_TIMEOUT) { Serial.println("Timeout reached when looking for GPS data."); return askForLocation(attemptNumber++);;}

            if (coordinate1.equals(coordinate2)) { //if they're the same
                return coordinate1;
            }
            else {
                //try again
                return askForLocation(attemptNumber++);
            }
        }

        GPSGridCoordinate* getLocation() { //method would be replaced with a GPS module in final product
            String coordinateString = askForLocation(0);
            if (coordinateString.equals(LOCATION_NOT_KNOWN)) {
                return new GPSGridCoordinate(0, 0); //no idea where you are
            }
            else {
                return new GPSGridCoordinate(coordinateString);
            }
        }
        void sendSample(double time, GPSGridCoordinate sampleLocation, double moistureValue) { //should this be void? Maybe return an error?
            String sampleText = format("{},{},{}", String(time), sampleLocation.toCSV(), String(moistureValue));
            String messageText = format("<sample>{}|{}</sample>", sampleText, sampleText); //sends twice and keeps sending until DICE confirms that it has recieved

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
                Serial.println(format("Failed to transmit sample: {}", sampleText));
            }
            else {
                Serial.println("Managed to transmit sample.");
            }
        }

    public:
        void moveForward(int time) {
            if (safeToDeploySensors) {
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
            else {
                Serial.println("Could not move forward as sensor arm is still deployed");
            }
        }

        void moveBackward(int time) {
            if (safeToDeploySensors) {
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
            else {
                Serial.println("Could not move backward as sensor arm is still deployed");
            }
        }

        void turnLeft(int time) {
            if (safeToDeploySensors) {
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
            else {
                Serial.println("Could not turn left as sensor arm is still deployed");
            }
        }

        void turnRight(int time) {
            if (safeToDeploySensors) {
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
            else {
                Serial.println("Could not turn right as sensor arm is still deployed");
            }
        }

        void takeSamples() { //method takes all samples and return them to the DICE machine
            Serial.println("Starting sampling");
            lowerArm();
            double time = time(0); //timestamp
            GPSGridCoordinate sampleLocation = getLocation();
            double moistureValue = getMoistureReading();
            sendSample(time, sampleLocation, moistureValue);
            //this will likely need rewritten so that samples can be taken concurrently
            //check if Arduino is actually more efficient doing this before making concurrent
            raiseArm();
            
            delete(sampleLocation);
        }

        void initiateSurvey(GPSGridCoordinate startPosition, GPSGridCoordinate endPosition, double samplingFrequency) {
            Serial.println("Starting survey...");
            Serial.print("Start position: ");
            Serial.println(startPosition.toString());
            
            Serial.print("End position: ");
            Serial.println(endPosition.toString());
            
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

        void runTestSequence() {
            Serial.println("Running test sequence...");
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
}

void setup(){
  SDPsetup();
  Serial.begin(BAUD_RATE);
  Serial.println("Connection established!");
  Serial.println("Entering wireless control mode...");
  safeToDeploySensors = true; //ready to deploy
}



void loop(){
  
        // Wait till for start command
        //while(!Serial.find("start test"));
        runTestSequence();
}

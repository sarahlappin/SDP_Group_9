import serial
from time import sleep
import sys
import imageFilter
import fileTransfer
import datetime
import detectTargets


targetCoordinates = detectTargets.getTargets()

BAUD = 9600

try:
    serial = serial.Serial("/dev/ttyACM0", BAUD, timeout = .1)
except:
    print("Changing port.......................")
    try:
        serial = serial.Serial("/dev/ttyACM1", BAUD, timeout = .1)
    except:
        try:
            serial = serial.Serial("/dev/ttyACM2", BAUD, timeout = .1)
        except:
            serial = serial.Serial("/dev/ttyACM3", BAUD, timeout = .1)

getLocationTag = "<getLocation/>"
getSurveyTag = "<getSurvey/>"
surveyOpeningTag = "<survey>"
surveyClosingTag = "</survey>"
getAngleTag = "<getAngle/>"

completeSurveyOpening = "<surveyComplete>"
completeSurveyClosing = "</surveyComplete>"

openingSampleTag = "<sample>"
closingSampleTag = "</sample>"
verificationDeliminator = "|"
sampleRecieved = "<sampleRecieved/>"

sampleCSVHeading = "LandID,Latitude,Longitude,TimeStamp,Moisture(%),C0(ppmv),pH\n"
tempFileAddress = "tempSampleData.csv"

redHistory = [0,0]
blueHistory = [0,0]
greenHistory = [0,0]
orangeHistory = [0,0]

def getLocation():
    a, _ = imageFilter.findCoordinates(redHistory, blueHistory, greenHistory, orangeHistory)
    print(a)

    #serial.write(a.encode)
    return a

def getSurvey():
    return 1.2, 2.3, 4.5, 6.76, 1.1

def getAngle():
    _, a = imageFilter.findCoordinates(redHistory, blueHistory, greenHistory, orangeHistory)
    print(a)
    return round(a, 2)

def saveSample(sample):
    #save as a CSV file and then call fileTransfer.py to transmit to MongoDB
    try:
        tempFile = open(tempFileAddress, "w")
        tempFile.write(sampleCSVHeading + sample)
        tempFile.close()

        return fileTransfer.sendCSV(tempFileAddress)
    except Exception as e:
        print("** Could not create the temp file in saveSample() in workstation.py")
        return False

def sendLocation():
    latitude, longitude = getLocation()
    location = "<location>" + str(latitude) + "," + str(longitude) + "|" + str(latitude) + "," + str(longitude) + "</location>"
    sendToRobot(location)

def sendAngle():
    angle = getAngle()
    anglePrint = "<angle>" + str(angle) + "|" + str(angle) + "</angle>"
    sendToRobot(anglePrint)

def receiveSample(line):
    #sample has been sent back
    sample1 = line[(line.index(openingSampleTag) + len(openingSampleTag)) : line.index(verificationDeliminator)]
    sample2 = line[(line.index(verificationDeliminator) + len(verificationDeliminator)): line.index(closingSampleTag)]
    if sample1 == sample2: #if they match then the sample has been correctly received
        serial.write(sampleRecieved.encode()) #let the robot know that it has recieved it
        print("** Sample received ==> " + sample1)
        if not saveSample(sample1):
            while not saveSample(sample1):
                print("** Could not manage to transmit sample to MongoDB, trying again...")
        print("** Sample transmitted")

def sendNextSurvey():
    surveyID, startLatitude, startLongitude, endLatitude, endLongitude, samplingFrequency = fileTransfer.getNextSurvey()
    if surveyID != False: #survey was found else do nothing
        surveyString = str(surveyID) + "," + str(startLatitude) + "," +  str(startLongitude) + "," +  str(endLatitude) + "," +  str(endLongitude) + "," +  str(samplingFrequency)
        outputSurveyString = surveyOpeningTag + surveyString + "|" + surveyString + surveyClosingTag
        sendToRobot(outputSurveyString)
    else:
        print("Survey request was recieved but no surveys were ready to be completed")

def markSurveyAsComplete(line):
    id = line[(line.index(completeSurveyOpening) + len(completeSurveyOpening)) : line.index(completeSurveyClosing)]
    fileTransfer.markSurveyComplete(id, datetime.datetime.now().timestamp())

def sendToRobot(data):
    print("Workstation > eIEIO: " + data)
    serial.write(data.encode())

def interpretLine(line):
    if getLocationTag in line:
        sendLocation()
    if getAngleTag in line:
        sendAngle()
    if openingSampleTag in line and closingSampleTag in line and verificationDeliminator in line:
        receiveSample(line)
    if getSurveyTag in line:
        sendNextSurvey()
    if completeSurveyOpening in line and completeSurveyClosing in line:
        markSurveyAsComplete(line)

manualMode = False
surveyMode = False


print("Starting workstation")
while True:
    try:
        line = str(serial.readline().decode().strip('\n').strip('\r')) #get each line and decode it
        if (len(line)>0):
            print("eIEIO > Workstation: " + line)
        interpretLine(line)
    except Exception as e:
        print("** Exception while reading data: " + str(e))

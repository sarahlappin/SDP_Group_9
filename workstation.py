import serial
from time import sleep
import sys
import imageFilter
import fileTransfer

BAUD = 9600

serial = serial.Serial("/dev/ttyACM1", BAUD, timeout = .1)

getLocationTag = "<getLocation/>"
getSurveyTag = "<getSurvey/>"
getAngleTag = "<getAngle/>"
openingSampleTag = "<sample>"
closingSampleTag = "</sample>"
verificationDeliminator = "|"
sampleRecieved = "<sampleRecieved/>"

sampleCSVHeading = "LandID,Latitude,Longitude,TimeStamp,C0(ppmv),Moisture(%)\n"
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
    print("** ==>" + location)
    serial.write(location.encode())

def sendAngle():
    angle = getAngle()
    anglePrint = "<angle>" + str(angle) + "|" + str(angle) + "</angle>"
    print("** ==>" + anglePrint)
    serial.write(anglePrint.encode())

def sendSurveyDetails():
    startLat, startLong, endLat, endLong, samplingFrequency = getSurvey()
    messageText = str(startLat) + b"," + str(startLong) + b"," + str(endLat) + b"," + str(endLong) + b"," + str(samplingFrequency)
    serial.write(("<survey>" + messageText + "|" + messageText + "</survey>").encode())

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

def interpretLine(line):
    if getLocationTag in line:
        sendLocation()
    elif getAngleTag in line:
        sendAngle()
    if getSurveyTag in line:
        sendSurveyDetails()
    if openingSampleTag in line and closingSampleTag in line and verificationDeliminator in line:
        receiveSample(line)

manualMode = False
surveyMode = False

while True:
    try:
        line = str(serial.readline().decode().strip('\n').strip('\r')) #get each line and decode it
        if (len(line)>0):
            print("eIEIO > Workstation: " + line)
        interpretLine(line)
    except Exception as e:
        print("** Exception while reading data: " + str(e))

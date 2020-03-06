import serial
from time import sleep
import sys
import imageFilter

BAUD = 9600

serial = serial.Serial("/dev/ttyACM0", BAUD, timeout = .1)

getLocationTag = "<getLocation/>"
getSurveyTag = "<getSurvey/>"
getAngleTag = "<getAngle/>"
openingSampleTag = "<sample>"
closingSampleTag = "</sample>"
verificationDeliminator = "|"
sampleRecieved = "<sampleRecieved/>"

redHistory = [0,0]
blueHistory = [0,0]
greenHistory = [0,0]
orangeHistory = [0,0]

def getLocation():
    a, _ = imageFilter.findCoordinates(redHistory, blueHistory, greenHistory, orangeHistory)
    #print(a)
    #serial.write(a.encode)
    return a

def getSurvey():
    return 1.2, 2.3, 4.5, 6.76, 1.1

def getAngle():
    _, a = imageFilter.findCoordinates(redHistory, blueHistory, greenHistory, orangeHistory)
    return  a

def saveSample(sample):
    q = 1 #do nothing

def interpretLine(line):
    if getLocationTag in line:
        latitude, longitude = getLocation()
        location = "<location>" + str(latitude) + "," + str(longitude) + "|" + str(latitude) + "," + str(longitude) + "</location>"
        print("==>" + location)
        serial.write(location.encode())
    elif getAngleTag in line:
        angle = getAngle()
        anglePrint = "<angle>" + str(angle) + "|" + str(angle) + "</angle>"
        print("==>" + anglePrint)
        serial.write(anglePrint.encode())
    if getSurveyTag in line:
        startLat, startLong, endLat, endLong, samplingFrequency = getSurvey()
        messageText = str(startLat) + b"," + str(startLong) + b"," + str(endLat) + b"," + str(endLong) + b"," + str(samplingFrequency)
        serial.write(("<survey>" + messageText + "|" + messageText + "</survey>").encode())
    if openingSampleTag in line and closingSampleTag in line and verificationDeliminator in line:
        #sample has been sent back
        sample1 = line[(line.index(openingSampleTag) + len(openingSampleTag)) : line.index(verificationDeliminator)]
        sample2 = line[(line.index(verificationDeliminator) + len(verificationDeliminator)): line.index(closingSampleTag)]
        if sample1 == sample2: #if they match then the sample has been correctly received
            serial.write(sampleRecieved.encode()) #let the robot know that it has recieved it
            print("Sample received ====> " + sample1)
            saveSample(sample1)

manualMode = False
surveyMode = False

while True:
    # if surveyMode:
    #     # send details of the survey
    # elif manualMode:
    #     if keyboard.press(escape):
    #         break
    #     elif keyboard.press('w'):
    #         serial.write("<forward/>")
    #     elif keyboard.press('a'):
    #         serial.write("<left/>")
    #     elif keyboard.press('s'):
    #         serial.write("<backward/>")
    #     elif keyboard.press('d'):
    #         serial.write("<right/>")
    #     elif keyboard.press(space):
    #         serial.write("<takeSample/>")
    #     elif keyboard.press('q'):
    #         serial.write("<stop/>")
    # elif keyboard.press('1'): #manual mode
    #     manualMode = True
    # elif keyboard.press('2'):
    #     surveyMode = True

    try:
        line = str(serial.readline().decode().strip('\n').strip('\r')) #get each line and decode it
        if (len(line)>0):
            print(line)
        interpretLine(line)
    except:
        print("****** Exception while reading data - probably from an encoding error")

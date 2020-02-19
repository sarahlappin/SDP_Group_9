import serial
from time import sleep
import sys

BAUD = 9600

serial = serial.Serial("/dev/ttyACM0", BAUD, timeout = .1)

getLocationTag = "<getLocation/>"
getSurveyTag = "<getSurvey/>"
openingSampleTag = "<sample>"
closingSampleTag = "</sample>"
verificationDeliminator = "|"
sampleRecieved = "<sampleRecieved/>"


def getLocation():
    return 1,2

def getSurvey():
    return 1.2, 2.3, 4.5, 6.76, 1.1

def saveSample(sample):
    q = 1 #do nothing 

def interpretLine(line):
    #print line
    if getLocationTag in line:
        latitude, longitude = getLocation()
        location = b"<location>" + str(latitude) + "," + str(longitude) + b"</location>"
        print "==>" + location
        serial.write(location)
    if getSurveyTag in line:
        startLat, startLong, endLat, endLong, samplingFrequency = getSurvey()
        messageText = str(startLat) + b"," + str(startLong) + b"," + str(endLat) + b"," + str(endLong) + b"," + str(samplingFrequency)
        serial.write(b"<survey>" + messageText + b"|" + messageText + b"</survey>")
    if openingSampleTag in line and closingSampleTag in line and verificationDeliminator in line:
        #sample has been sent back
        sample1 = line[(line.index(openingSampleTag) + len(openingSampleTag)) : line.index(verificationDeliminator)]
        sample2 = line[(line.index(verificationDeliminator) + len(verificationDeliminator)): line.index(closingSampleTag)]
        if sample1 == sample2: #if they match then the sample has been correctly received
            serial.write(sampleRecieved) #let the robot know that it has recieved it
            print "Sample received ====> " + sample1
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
            interpretLine(line)
        except:
            print "****** Exception while reading data - probably from an encoding error"

	
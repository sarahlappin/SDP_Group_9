import serial
from time import sleep
import sys


COM = 'COM3'# /dev/ttyACM0 (Linux)
BAUD = 9600

ser = serial.Serial("/dev/ttyACM0", BAUD, timeout = .1)

getLocationTag = "<getLocation/>"
openingSampleTag = "<sample>"
closingSampleTag = "</sample>"
verificationDeliminator = "|"
sampleRecieved = "<sampleRecieved/>"


print 'Waiting for device'
sleep(3)
print ser.name
print "*******************************"

def getLocation():
    return 1,2

def saveSample(sample):
    q = 1 #do nothing 

def interpretLine(line):
    #print line
    if getLocationTag in line:
        latitude, longitude = getLocation()
        location = b"<location>" + str(latitude) + "," + str(longitude) + b"</location>"
        print "==>" + location
        ser.write(location)
    if openingSampleTag in line and closingSampleTag in line and verificationDeliminator in line:
        #sample has been sent back
        sample1 = line[(line.index(openingSampleTag) + len(openingSampleTag)) : line.index(verificationDeliminator)]
        sample2 = line[(line.index(verificationDeliminator) + len(verificationDeliminator)): line.index(closingSampleTag)]
        if sample1 == sample2: #if they match then the sample has been correctly received
            ser.write(sampleRecieved) #let the robot know that it has recieved it
            print "Sample received ====> " + sample1
            saveSample(sample1)

#check args
if("-m" in sys.argv or "--monitor" in sys.argv):
	monitor = True
else:
	monitor= False

manualMode = False
surveyMode = False

while True:
    # if surveyMode:
    #     # send details of the survey
    # elif manualMode:
    #     if keyboard.press(escape):
    #         break
    #     elif keyboard.press('w'):
    #         ser.write("<forward/>")
    #     elif keyboard.press('a'):
    #         ser.write("<left/>")
    #     elif keyboard.press('s'):
    #         ser.write("<backward/>")
    #     elif keyboard.press('d'):
    #         ser.write("<right/>")
    #     elif keyboard.press(space):
    #         ser.write("<takeSample/>")
    #     elif keyboard.press('q'):
    #         ser.write("<stop/>")
    # elif keyboard.press('1'): #manual mode
    #     manualMode = True
    # elif keyboard.press('2'):
    #     surveyMode = True
    
	try:
            line = str(ser.readline().decode().strip('\n').strip('\r')) #Capture serial output as a decoded string
            interpretLine(line)
        except:
            print "****** Exception while reading data - probably from an encoding error"

	
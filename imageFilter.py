import numpy as np
import cv2
import math
import glob
import time

instructions = "Red Circle is the start of the coordinate system. Orange Circle is the front of the car." \
               "Blue Circle is the rear of the car. Once the streaming starts, press p to print the distance" \
               "and the angle between the blue and orange circle."
pixelToMillimeterX = 1#0.8575
pixelToMillimeterY = 1#0.741

def midpoint(a, b):
    (x1, y1) = a
    (x2, y2) = b
    return (x1 + x2) / 2, (y1 + y2) / 2


def calculateDistance(a, b):
    (x1, y1) = a
    (x2, y2) = b
    dist = math.sqrt(((x2 - x1) * pixelToMillimeterX) ** 2 + ((y2 - y1)*pixelToMillimeterY) ** 2)
    return dist


def findCentre(img, previousPoint, dst):
    centers = []
    # Find contours with matching filters
    contours, hierarchy = cv2.findContours(img, 1, 2)
    for cnt in contours:
        epsilon = 0.001 * cv2.arcLength(cnt, True)
        approx = cv2.approxPolyDP(cnt, epsilon, True)
        area = cv2.contourArea(cnt)
        if (area > 50 and area < 200) or (len(contours) == 1 and area > 0):
            #print(len(approx))
            cv2.drawContours(dst, [cnt], -1, (0, 0, 255), 1)
            #cv2.drawContours(dst, [approx], -1, (0, 0, 255), 1)
            #print(calculateDistance(approx[0], approx[1]))
            M = cv2.moments(cnt)
            cX = int(M['m10'] / M['m00'])
            cY = int(M['m01'] / M['m00'])
            cv2.circle(dst, (cX, cY), 1, (0, 0, 255), -1)
            centers.append([cX, cY])
    if len(centers) == 0:
        return previousPoint
    if len(centers) != 1:
        minDistance = 10000
        minPoint = []
        for point in centers:
            if calculateDistance(previousPoint, point) < minDistance and len(previousPoint) > 0:
                minDistance = calculateDistance(previousPoint, point)
                minPoint = point
        return minPoint
    return centers[0]


def findCoordinates(redHistory, blueHistory, greenHistory, orangeHistory):
    cap = cv2.VideoCapture(-1)
    testNo = 0
    nTests = 5
    # = 1
    while (True):
        #name = str(i) + ".png"
        #i += 1
        # Capture frame-by-frame
        ret, image = cap.read()  # ret = 1 if the video is captured; frame is the image
        #image = cv2.imread("6.png")
        #cv2.imwrite(name, image)
        #image = cv2.imread("123254.png")
        if testNo < nTests:
            testNo+=1
            continue
        ##image = cv2.imread('4.png')
        # Convert image to grayscale
        # gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        # clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(8, 8))
        # image = clahe.apply(gray)

        # Load data for undistortion
        with np.load('calib3.npz') as X:
            mtx, dist, _, _ = [X[i] for i in ('mtx', 'dist', 'rvecs', 'tvecs')]

        h, w = image.shape[:2]
        newcameramtx, roi = cv2.getOptimalNewCameraMatrix(mtx, dist, (w, h), 1, (w, h))

        # Undistort
        x, y, w, h = roi
        dst = cv2.undistort(image, mtx, dist, None, newcameramtx)
        #dst = dst[y:y + h, x:x + w]

        image_src = cv2.cvtColor(dst, cv2.COLOR_BGR2HSV)

        # Apply mask on image
        # img = cv2.inRange(image_src, (15,105,200), (25,165,230))
        #red = cv2.inRange(image_src, (0, 70, 180), (15, 255, 255))
        #white = cv2.inRange(image_src, (0, 0, 134), (179, 30, 255))
        green = cv2.inRange(image_src, (75, 50, 120), (100, 255, 255))
        blue = cv2.inRange(image_src, (105, 50, 120), (125, 255, 255))
        orange = cv2.inRange(image_src, (0, 50, 120), (20, 255, 255))


        #redHistory = findCentre(red, redHistory, dst)
        #print("Green:")
        greenHistory = findCentre(green, greenHistory, dst)
        #print("Blue:")
        blueHistory = findCentre(blue, blueHistory, dst)
        #print("Orange:")
        orangeHistory = findCentre(orange, orangeHistory, dst)
        #print("")
        #print("White:")
        #whiteHistory = findCentre(white, [0,0], dst)



        blueCoordinates = np.round((np.array(blueHistory) - np.array(greenHistory)) * pixelToMillimeterX)
        orangeCoordinates = np.round((np.array(orangeHistory) - np.array(greenHistory)) * pixelToMillimeterY)

        distanceP = calculateDistance(blueCoordinates, orangeCoordinates)

        angleRadians = math.atan2(blueCoordinates[1] - orangeCoordinates[1], blueCoordinates[0] - orangeCoordinates[0])
        angleDegrees = math.degrees(angleRadians)


        #target = (350, 100)
        #cv2.circle(dst, target, 10, 0, thickness=1, lineType=8, shift=0)
        #cv2.circle(dst, target, 1, 0, -1)

        # Display the original image
        #cv2.imshow('Original Capture', image)
        #cv2.imshow('Calibrated Capture', dst)

        copyDst = dst
        cv2.circle(copyDst, ((int)(midpoint(blueCoordinates, orangeCoordinates)[0])+ greenHistory[0], (int) (midpoint(blueCoordinates, orangeCoordinates)[1]) + greenHistory[1]),  1, (0, 0, 255), -1)
        cv2.circle(copyDst, (150 + greenHistory[0],150 + greenHistory[1]), 1, (0, 0, 255), -1)

        cv2.imwrite('1.png', copyDst)
        #cv2.imwrite('green.png', green)
        #cv2.imwrite('orange.png', orange)

        #cv2.imshow('Red Mask', red)
        #cv2.imshow('Green Mask', green)
        #cv2.imshow('Blue Mask', blue)
        #cv2.imshow('Orange Mask', orange)

        #k = cv2.waitKey(1)
        k = ord('r')
        ##if k == ord('p'):  # press q to quit

        """""
        print("Green: ", greenHistory)
        print("BlueH: ", blueHistory)
        print("OrangeH: ", orangeHistory)
        print("Blue: ", blueCoordinates)
        print("Orange: ", orangeCoordinates)
        print("Distance: ", round(distanceP), "cm")
        print("Angle: ", round(angleDegrees), "degrees")
        print("")
        """
        if k == ord('r'):
            return midpoint(blueCoordinates, orangeCoordinates), angleDegrees
        elif k == ord('q'):  # press q to quit
            break

    # When everything done, release the capture

    #cap.release()
    cv2.destroyAllWindows()


#print(findCoordinates([0,0], [0,0], [0,0], [0,0]))

#print("Small Dist: ", calculateDistance((227,321), (249, 330)))
#print("Large Dist: ", calculateDistance((227,321), (235, 282)))

import numpy as np
import cv2
import math
import glob
import time

instructions = "Red Circle is the start of the coordinate system. Orange Circle is the front of the car." \
               "Blue Circle is the rear of the car. Once the streaming starts, press p to print the distance" \
               "and the angle between the blue and orange circle."


def midpoint(a, b):
    (x1, y1) = a
    (x2, y2) = b
    return (x1 + x2) / 2, (y1 + y2) / 2


def calculateDistance(a, b):
    (x1, y1) = a
    (x2, y2) = b
    dist = math.sqrt((x2 - x1) ** 2 + (y2 - y1) ** 2)
    return dist


def findCentre(img, previousPoint, dst):
    centers = []
    # Find contours with matching filters
    contours, hierarchy = cv2.findContours(img, 1, 2)
    for cnt in contours:
        epsilon = 0.1 * cv2.arcLength(cnt, True)
        area = cv2.contourArea(cnt)
        if (area > 80 and area < 200) or (len(contours) == 1 and area > 0):
            cv2.drawContours(dst, [cnt], -1, (0, 0, 0), 1)
            M = cv2.moments(cnt)
            cX = int(M['m10'] / M['m00'])
            cY = int(M['m01'] / M['m00'])
            cv2.circle(dst, (cX, cY), 1, (0, 0, 255), -1)
            centers.append([cX, cY])
    if len(centers) == 0:
        return previousPoint
    if len(centers) != 1:
        minDistance = 1000
        minPoint = []
        for point in centers:
            if calculateDistance(previousPoint, point) < minDistance and len(previousPoint) > 0:
                minDistance = calculateDistance(previousPoint, point)
                minPoint = point
        return minPoint
    return centers[0]


def findCoordinates(redHistory, blueHistory, greenHistory, orangeHistory):
    cap = cv2.VideoCapture(0)
    pixelToCentimeters = 60/70
    testNo = 0
    nTests = 5
    while (True):
        # Capture frame-by-frame
        ret, image = cap.read()  # ret = 1 if the video is captured; frame is the image

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
        dst = dst[y:y + h, x:x + w]

        image_src = cv2.cvtColor(dst, cv2.COLOR_BGR2HSV)

        # Apply mask on image
        # img = cv2.inRange(image_src, (15,105,200), (25,165,230))
        red = cv2.inRange(image_src, (0, 70, 180), (15, 255, 255))
        green = cv2.inRange(image_src, (75, 65, 130), (95, 255, 210))
        blue = cv2.inRange(image_src, (105, 50, 160), (125, 255, 255))
        orange = cv2.inRange(image_src, (0, 50, 120), (35, 255, 255))

        redHistory = findCentre(red, redHistory, dst)
        greenHistory = findCentre(green, greenHistory, dst)
        blueHistory = findCentre(blue, blueHistory, dst)
        orangeHistory = findCentre(orange, orangeHistory, dst)

        blueCoordinates = np.round((np.array(blueHistory) - np.array(greenHistory)) * pixelToCentimeters)
        orangeCoordinates = np.round((np.array(orangeHistory) - np.array(greenHistory)) * pixelToCentimeters)

        distanceP = calculateDistance(blueCoordinates, orangeCoordinates)

        angleRadians = math.atan2(blueCoordinates[1] - orangeCoordinates[1], blueCoordinates[0] - orangeCoordinates[0])
        angleDegrees = math.degrees(angleRadians)

        # Display the original image
        # cv2.imshow('Original Capture', image)
        # cv2.imshow('Calibrated Capture', dst)
        cv2.imwrite('123254.png', dst)
        cv2.imwrite('12325.png', image)
        # cv2.imshow('Red Mask', red)
        # cv2.imshow('Green Mask', green)
        # cv2.imshow('Blue Mask', blue)
        # cv2.imshow('Orange Mask', orange)

        # k = cv2.waitKey(1)
        if (testNo == nTests):
            k = ord('r')
        else:
            testNo+=1
            k = ord('t')

        #if k == ord('p'):  # press q to quit
        # print("Green: ", greenHistory)
        # print("BlueH: ", blueHistory)
        # print("OrangeH: ", orangeHistory)
        # print("Blue: ", blueCoordinates)
        # print("Orange: ", orangeCoordinates)
        # print("Distance: ", round(distanceP), "mm")
        # print("Angle: ", round(angleDegrees), "degrees")
        # print("")
        if k == ord('r'):
            return midpoint(blueCoordinates, orangeCoordinates)
        elif k == ord('q'):  # press q to quit
            break

    # When everything done, release the capture

    cap.release()
    cv2.destroyAllWindows()

#print(findCoordinates([0,0], [0,0], [0,0], [0,0]))

import numpy as np
import cv2
import math
import glob
import time
import tkinter

"""""
    cap = cv2.VideoCapture(-1)
    testNo = 0
    nTests = 10

    while testNo < nTests:
        ret, image = cap.read()  # ret = 1 if the video is captured; frame is the image
        testNo += 1

    finalImage = image
"""

dst = cv2.imread("1.png")

arrayCounter = 0
maxCounter = 0
arrayStates = np.zeros((100, 480, 640))
arrayStates[arrayCounter] = np.zeros(dst.shape[:2], np.uint8)

imageStates = np.zeros((100, 480, 640, 3), np.uint8)
imageStates[arrayCounter] = dst.copy()

closeWindow = False

while not closeWindow:
    roi = cv2.selectROI('Image', dst, False, False)
    x, y, w, h = roi


    def undoButton():
        global arrayCounter
        if arrayCounter > 0:
            arrayCounter -= 1
        global dst
        dst = imageStates[arrayCounter].copy()
        cv2.imshow("Image", dst)


    def redoButton():
        global arrayCounter
        if arrayCounter < maxCounter:
            arrayCounter += 1
        global dst
        dst = imageStates[arrayCounter].copy()
        cv2.imshow("Image", dst)


    def confirmButton():
        global dst
        dst = imageStates[arrayCounter].copy()
        cv2.imshow("Image", dst)
        window.destroy()


    def quitButton():
        cv2.destroyAllWindows()
        window.destroy()
        global closeWindow
        closeWindow = True


    if w is not 0 and h is not 0:
        arrayCounter += 1
        maxCounter = arrayCounter
        arrayStates[arrayCounter] = arrayStates[arrayCounter - 1].copy()

        maskImage = imageStates[arrayCounter - 1].copy()

        for i in range(y, y + h):
            for j in range(x, x + w):
                if not arrayStates[arrayCounter][i][j]:
                    maskImage[i][j] = (0, 255, 0)
                    arrayStates[arrayCounter][i][j] = 1

        alpha = 0.15
        dst = cv2.addWeighted(maskImage, alpha, dst, 1 - alpha, 0, dst)
        imageStates[arrayCounter] = dst.copy()
        cv2.imshow("Image", dst)

        window = tkinter.Tk()
        window.title("GUI")
        bottom_frame = tkinter.Frame(window).pack(side="bottom")
        btn1 = tkinter.Button(bottom_frame, text="Confirm", command=confirmButton).pack(side="left")
        btn2 = tkinter.Button(bottom_frame, text="Undo", command=undoButton).pack(side="left")
        btn3 = tkinter.Button(bottom_frame, text="Redo", command=redoButton).pack(side="left")
        btn4 = tkinter.Button(bottom_frame, text="Quit", command=quitButton).pack(side="left")
        window.mainloop()

foundL = False
topMostPoint = 0, 0
for i in range(0, 480):
    for j in range(0, 640):
        if arrayStates[arrayCounter][i][j] == 1 and not foundL:
            topMostPoint = i, j
            foundL = True

topCordY, topCordX = topMostPoint
moveY = 50
moveX = 50
imageGrid = dst.copy()
gridLines = np.zeros((480, 640))

for i in range(topCordY, 480, moveY):
    for j in range(0, 640):
        if arrayStates[arrayCounter][i][j] == 1:
            imageGrid[i][j] = (0, 0, 0)
            gridLines[i][j] = 1
for i in range(0, 480):
    for j in range(topCordX, 640, moveX):
        if arrayStates[arrayCounter][i][j] == 1:
            imageGrid[i][j] = (0, 0, 0)
            gridLines[i][j] = 1

for i in range(topCordY, 0, -moveY):
    for j in range(0, 640):
        if arrayStates[arrayCounter][i][j] == 1:
            imageGrid[i][j] = (0, 0, 0)
            gridLines[i][j] = 1
for i in range(0, 480):
    for j in range(topCordX, 0, -moveX):
        if arrayStates[arrayCounter][i][j] == 1:
            imageGrid[i][j] = (0, 0, 0)
            gridLines[i][j] = 1

for i in range(1, 479):
    for j in range(1, 639):
        if arrayStates[arrayCounter][i][j] == 1:
            if (arrayStates[arrayCounter][i][j + 1] == 0 or
                    arrayStates[arrayCounter][i][j - 1] == 0 or
                    arrayStates[arrayCounter][i + 1][j] == 0 or
                    arrayStates[arrayCounter][i - 1][j] == 0):
                imageGrid[i][j] = (0, 0, 0)
                gridLines[i][j] = 1

for i in range(topCordY, 480, moveY):
    for j in range(topCordX, 640, moveX):
        squareGrid = np.zeros((480, 640))
        for k in range(i, i + moveY):
            for l in range(j, j + moveX):
                if k < 480 and l < 640:
                    if arrayStates[arrayCounter][k][l] == 1:
                        squareGrid[k][l] = 1
        M = cv2.moments(squareGrid)
        if M["m00"] != 0:
            cX = int(M["m10"] / M["m00"])
            cY = int(M["m01"] / M["m00"])
            cv2.circle(imageGrid, (cX, cY), 1, (0, 0, 255), -1)
    for j in range(topCordX, 0, -moveX):
        squareGrid = np.zeros((480, 640))
        for k in range(i, i + moveY):
            for l in range(j, j + moveX):
                if k < 480 and l < 640:
                    if arrayStates[arrayCounter][k][l] == 1:
                        squareGrid[k][l] = 1
        M = cv2.moments(squareGrid)
        if M["m00"] != 0:
            cX = int(M["m10"] / M["m00"])
            cY = int(M["m01"] / M["m00"])
            cv2.circle(imageGrid, (cX, cY), 1, (0, 0, 255), -1)


cv2.imwrite("imageGrid.png", imageGrid)

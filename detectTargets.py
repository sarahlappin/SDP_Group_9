import numpy as np
import cv2
import tkinter

"""""
cap = cv2.VideoCapture(-1)
testNo = 0
nTests = 10

while testNo < nTests:
    ret, image = cap.read()  # ret = 1 if the video is captured; frame is the image
    testNo += 1

with np.load('calib3.npz') as X:
    mtx, dist, _, _ = [X[i] for i in ('mtx', 'dist', 'rvecs', 'tvecs')]

h, w = image.shape[:2]
newcameramtx, roi = cv2.getOptimalNewCameraMatrix(mtx, dist, (w, h), 1, (w, h))

# Undistort
dst = cv2.undistort(image, mtx, dist, None, newcameramtx)
cv2.imwrite("test.png", dst)
"""
dst = cv2.imread("test.png")

arrayCounter = 0
maxCounter = 0
arrayStates = np.zeros((100, 480, 640))
arrayStates[arrayCounter] = np.zeros(dst.shape[:2], np.uint8)

imageStates = np.zeros((100, 480, 640, 3), np.uint8)
imageStates[arrayCounter] = dst.copy()

currentState = np.zeros((100, 480, 640, 3), np.uint8)
currentState[arrayCounter] = dst.copy()

closeWindow = False


def addShadow():
    alpha = 0.3
    global dst
    global imageStates
    global arrayCounter
    global currentState
    maskTemp = imageStates[arrayCounter].copy()
    currentState[arrayCounter] = cv2.addWeighted(maskTemp, alpha, dst, 1 - alpha, 0)
    cv2.imshow("Image", currentState[arrayCounter])


while not closeWindow:
    roi = cv2.selectROI('Image', currentState[arrayCounter], False, False)
    x, y, w, h = roi


    def undoButton():
        global arrayCounter
        if arrayCounter > 0:
            arrayCounter -= 1
        global dst
        addShadow()


    def redoButton():
        global arrayCounter
        if arrayCounter < maxCounter:
            arrayCounter += 1
        global dst
        addShadow()


    def confirmButton():
        global dst
        global imageStates
        global arrayCounter
        addShadow()
        window.destroy()


    def quitButton():
        window.destroy()
        addShadow()
        windowSure = tkinter.Tk()
        windowSure.title("Confirm Action")

        def confirmQuit():
            windowSure.destroy()
            global closeWindow
            closeWindow = True

        def rejectQuit():
            windowSure.destroy()
            addShadow()

        text = tkinter.Label(windowSure, text="Are you sure you don't want to add any other areas to the selection?",
                             width=30, wraplength=250).pack()
        bottomFrameSure = tkinter.Frame(windowSure).pack(side="bottom")
        btn1 = tkinter.Button(bottomFrameSure, text="Yes", command=confirmQuit, padx=50).pack(side="left")
        btn2 = tkinter.Button(bottomFrameSure, text="No", command=rejectQuit, padx=50).pack(side="left")
        windowSure.mainloop()


    if w is not 0 and h is not 0:
        arrayCounter += 1
        maxCounter = arrayCounter
        arrayStates[arrayCounter] = arrayStates[arrayCounter - 1].copy()
        imageStates[arrayCounter] = imageStates[arrayCounter - 1].copy()
        for i in range(y, y + h):
            for j in range(x, x + w):
                if not arrayStates[arrayCounter][i][j]:
                    imageStates[arrayCounter][i][j] = (220, 220, 220)
                    arrayStates[arrayCounter][i][j] = 1

        addShadow()

        window = tkinter.Tk()
        window.title("Area Selection")
        bottom_frame = tkinter.Frame(window).pack(side="bottom")
        btn1 = tkinter.Button(bottom_frame, text="Confirm", command=confirmButton).pack(side="left")
        btn2 = tkinter.Button(bottom_frame, text="Undo", command=undoButton).pack(side="left")
        btn3 = tkinter.Button(bottom_frame, text="Redo", command=redoButton).pack(side="left")
        btn4 = tkinter.Button(bottom_frame, text="Save", command=quitButton).pack(side="left")
        window.mainloop()

foundL = False
topMostPoint = 0, 0
for i in range(0, 480):
    for j in range(0, 640):
        if arrayStates[arrayCounter][i][j] == 1 and not foundL:
            topMostPoint = i, j
            foundL = True

topCordY, topCordX = topMostPoint
moveY = 20
moveX = 20
gridLines = np.zeros((480, 640))

targetCoordinates = {}

for i in range(topCordY, 480, moveY):
    for j in range(0, 640):
        if arrayStates[arrayCounter][i][j] == 1:
            dst[i][j] = (0, 0, 0)
            gridLines[i][j] = 1
for i in range(0, 480):
    for j in range(topCordX, 640, moveX):
        if arrayStates[arrayCounter][i][j] == 1:
            dst[i][j] = (0, 0, 0)
            gridLines[i][j] = 1

for i in range(topCordY, 0, -moveY):
    for j in range(0, 640):
        if arrayStates[arrayCounter][i][j] == 1:
            dst[i][j] = (0, 0, 0)
            gridLines[i][j] = 1
for i in range(0, 480):
    for j in range(topCordX, 0, -moveX):
        if arrayStates[arrayCounter][i][j] == 1:
            dst[i][j] = (0, 0, 0)
            gridLines[i][j] = 1

for i in range(1, 479):
    for j in range(1, 639):
        if arrayStates[arrayCounter][i][j] == 1:
            if (arrayStates[arrayCounter][i][j + 1] == 0 or
                    arrayStates[arrayCounter][i][j - 1] == 0 or
                    arrayStates[arrayCounter][i + 1][j] == 0 or
                    arrayStates[arrayCounter][i - 1][j] == 0):
                dst[i][j] = (0, 0, 0)
                gridLines[i][j] = 1

numberedTargets = np.zeros((480, 640))
targetNo = 1
for i in range(topCordY, 480, moveY):
    for j in range(topCordX, 640, moveX):
        squareGrid = np.zeros((480, 640))
        for k in range(i, i + moveY):
            for l in range(j, j + moveX):
                if k < 480 and l < 640:
                    if arrayStates[arrayCounter][k][l] == 1:
                        squareGrid[k][l] = 1
                        numberedTargets[k][l] = targetNo
        M = cv2.moments(squareGrid)
        if M["m00"] != 0:
            cX = int(M["m10"] / M["m00"])
            cY = int(M["m01"] / M["m00"])
            # cv2.circle(dst, (cX, cY), 1, (0, 0, 255), -1)
            targetCoordinates[targetNo] = (cX, cY)
            targetNo += 1

    for j in range(topCordX, 0, -moveX):
        squareGrid = np.zeros((480, 640))
        for k in range(i, i + moveY):
            for l in range(j, j + moveX):
                if k < 480 and l < 640:
                    if arrayStates[arrayCounter][k][l] == 1:
                        squareGrid[k][l] = 1
                        numberedTargets[k][l] = targetNo
        M = cv2.moments(squareGrid)
        if M["m00"] != 0:
            cX = int(M["m10"] / M["m00"])
            cY = int(M["m01"] / M["m00"])
            # cv2.circle(dst, (cX, cY), 1, (0, 0, 255), -1)
            targetCoordinates[targetNo] = (cX, cY)
            targetNo += 1
addShadow()


def changeColour(colourAlready):
    if np.array_equal(colourAlready, [220, 220, 220]):
        return (0, 255, 0), True
    else:
        return (220, 220, 220), False


targetsList = []


def clickCoord(event, mouseX, mouseY, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:
        global numberedTargets
        global targetCoordinates
        global imageStates
        global arrayCounter
        global arrayStates
        global dst
        # cv2.circle(imageGrid, (mouseX, mouseY), 1, (0, 0, 255), -1)
        targetNumber = numberedTargets[mouseY][mouseX]
        if targetNumber in targetCoordinates.keys():
            nearestTarget = targetCoordinates[targetNumber]
            # print(nearestTarget)
            halfStepX = int(moveX / 2)
            halfStepY = int(moveY / 2)
            nearestTargetX, nearestTargetY = nearestTarget
            maskImageTemp = imageStates[arrayCounter].copy()

            for i in range(nearestTargetY - halfStepY - 1, nearestTargetY + halfStepY + 1):
                for j in range(nearestTargetX - halfStepX - 1, nearestTargetX + halfStepX + 1):
                    if i < 480 and j < 640:
                        if arrayStates[arrayCounter][i][j] == 1 and numberedTargets[i][j] == targetNumber:
                            maskImageTemp[i][j], selectedT = changeColour(maskImageTemp[i][j])
            imageStates[arrayCounter] = maskImageTemp
            if selectedT:
                targetsList.append(nearestTarget)
            else:
                targetsList.remove(nearestTarget)
            addShadow()


cv2.namedWindow("Image")
cv2.setMouseCallback("Image", clickCoord)

while True:
    cv2.imshow("Image", currentState[arrayCounter])
    k = cv2.waitKey(1)
    if k == ord('q'):
        break

cv2.imwrite("imageGrid.png", currentState[arrayCounter])
print(targetsList)

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

finalImage = cv2.imread("1.png")
with np.load('calib3.npz') as X:
    mtx, dist, _, _ = [X[i] for i in ('mtx', 'dist', 'rvecs', 'tvecs')]

h, w = finalImage.shape[:2]
newcameramtx, roi = cv2.getOptimalNewCameraMatrix(mtx, dist, (w, h), 1, (w, h))

# Undistort
dst = cv2.undistort(finalImage, mtx, dist, None, newcameramtx)

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
leftMostPoint = 0, 0
for i in range(0, 480):
    for j in range(0,640):
        if arrayStates[arrayCounter][i][j] == 1 and not foundL:
            leftMostPoint = i, j
            foundL = True

leftX, leftY = leftMostPoint
stepX = 50
stepY = 50
imageGrid = dst.copy()
for i in range(leftX, 480, stepX):
    for j in range(0, 640):
        if arrayStates[arrayCounter][i][j] == 1:
            imageGrid[i][j] = (255,255,255)
for i in range(0, 480):
    for j in range(leftY, 640, stepY):
        if arrayStates[arrayCounter][i][j] == 1:
            imageGrid[i][j] = (255, 255, 255)

cv2.imwrite("imageGrid.png", imageGrid)


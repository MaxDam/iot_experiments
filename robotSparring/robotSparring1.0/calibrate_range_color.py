import cv2
import numpy as np
import glob
import time

def nothing(x):
    pass

# Create a window
cv2.namedWindow('image')

# Create trackbars for color change
# Hue is from 0-179 for Opencv
cv2.createTrackbar('HMin', 'image', 0, 179, nothing)
cv2.createTrackbar('SMin', 'image', 0, 255, nothing)
cv2.createTrackbar('VMin', 'image', 0, 255, nothing)
cv2.createTrackbar('HMax', 'image', 0, 179, nothing)
cv2.createTrackbar('SMax', 'image', 0, 255, nothing)
cv2.createTrackbar('VMax', 'image', 0, 255, nothing)

# Set default value for Max HSV trackbars
cv2.setTrackbarPos('HMin', 'image', 13)
cv2.setTrackbarPos('SMin', 'image', 130)
cv2.setTrackbarPos('VMin', 'image', 255)
cv2.setTrackbarPos('HMax', 'image', 20)
cv2.setTrackbarPos('SMax', 'image', 255)
cv2.setTrackbarPos('VMax', 'image', 255)

MIN_RADIUS_THRWSHOLD = 10

#import images
filenames = [img for img in glob.glob("frames/*.jpg")]

#infinite loop
index = 0
while True:

    #open image from filenames
    image = cv2.imread(filenames[index], 1)
    index += 1
    if index >= len(filenames): index = 0

    # Get current positions of all trackbars
    hMin = cv2.getTrackbarPos('HMin', 'image')
    sMin = cv2.getTrackbarPos('SMin', 'image')
    vMin = cv2.getTrackbarPos('VMin', 'image')
    hMax = cv2.getTrackbarPos('HMax', 'image')
    sMax = cv2.getTrackbarPos('SMax', 'image')
    vMax = cv2.getTrackbarPos('VMax', 'image')

    # Set minimum and maximum HSV values to display
    lower = np.array([hMin, sMin, vMin])
    upper = np.array([hMax, sMax, vMax])

    # Convert to HSV format and color threshold
    #image = cv2.GaussianBlur(image, (11, 11), 0)
    hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
    mask = cv2.inRange(hsv, lower, upper)
    mask = cv2.erode(mask, None, iterations=2)
    mask = cv2.dilate(mask, None, iterations=2)

    # Print HSV value ranges
    print("((%d, %d, %d), (%d, %d, %d))" % (hMin , sMin , vMin, hMax, sMax , vMax))

    #trova i contorni
    contours  = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)[-2]
    center = None
    
    #scorre i contorni trovati
    for contour in contours:
        #ottiene il cerchio ed il centro
        ((x, y), radius) = cv2.minEnclosingCircle(contour)
        M = cv2.moments(contour)
        center = (int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"]))
            
        #cerca la palla con il raggio maggiore
        maxRadius = MIN_RADIUS_THRWSHOLD
        maxCenter = (0,0)
        if radius > maxRadius:
            maxRadius = radius
            maxCenter = center
    
    #se ha trovato la palla la disegna sull'immagine
    if len(contours) > 0:
        cv2.circle(image, maxCenter, int(maxRadius), (0, 255, 255), 2)
        cv2.circle(image, maxCenter, 5, (0, 0, 255), -1)

    # Display result image
    cv2.imshow('original', image)
    cv2.imshow('mask', mask)

    #exit
    k = cv2.waitKey(1)
    if(k==ord("q")): 
        break
    
    #sleep
    time.sleep(0.3)

cv2.destroyAllWindows()
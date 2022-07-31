#dipendenze:
#sudo apt-get update
#sudo apt-get install python-picamera python3-picamera
#pip install imutils 

#start camera:
#sudo service motion stop
#sudo modprobe bcm2835-v4l2

#start stream video:
#if error: Gtk-WARNING **: cannot open display
#export DISPLAY=":0"

from __future__ import print_function
from imutils.video import VideoStream
import imutils
import cv2
from math import sin, cos, radians
import jointdriver as jd
import time
import random
import io
import numpy as np
import math
import picamera

INPUT_VIDEO_STREAM = True
if(INPUT_VIDEO_STREAM):
    #inizializza il video stream
    cameraW, cameraH = (320, 240)
    vs = VideoStream(src=0,resolution=(cameraW, cameraH)).start()
else:
    #inizializza la camera
    camera = picamera.PiCamera()
    #camera.resolution = (280, 260)
    camera.resolution = (320, 240)
    #camera.resolution = (640, 480)
    camera.start_preview()

    #ottiene le coordinate della camera
    cameraW, cameraH = camera.resolution

#carica il cascade file
face_cascade = cv2.CascadeClassifier("haarcascade/haarcascade_frontalface_alt2.xml")
#face_cascade = cv2.CascadeClassifier("haarcascade/haarcascade_frontalface_default.xml")

#color thresholds
colorThresholds = (
    #calcolati a mano
    ((60, 242, 36), (97, 255, 255)), #green
    #((16, 197, 0), (47, 255, 255)) #orange
    #( (10,  150,   150), (30,  255, 255) ), #orange day
    #( (40,  100,   150), (90,  255, 255) ), #green day
    #( (10,  150,   100), (30,  255, 255) ), #orange night
    #( (40,  100,   0), (90,  255, 150) ), #green night
    
    #calcolati con l'applicazione
    #( (13,  0,   255), (50,  255, 255) ), #orangeDay
    #( (0,   185, 181), (19,  247, 246) ), #orangeNight
    #( (61,  91,  133), (85,  255, 255) ), #greenDay
    #( (70,  156, 64),  (87,  255, 255) ), #greenNight
    #( (97,  115, 136), (121, 250, 255) ), #blueDay
    #( (107, 153, 127), (123, 255, 242) )  #blueNight
)
MIN_RADIUS_THRWSHOLD = 10

#settaggi del face detection
faceDetectionSettings = {
    'scaleFactor': 1.3, 
    'minNeighbors': 3, 
    'minSize': (50, 50)
}

def rotate_image(image, angle):
    if angle == 0: return image
    height, width = image.shape[:2]
    rot_mat = cv2.getRotationMatrix2D((width/2, height/2), angle, 0.9)
    result = cv2.warpAffine(image, rot_mat, (width, height), flags=cv2.INTER_LINEAR)
    return result

def rotate_point(pos, img, angle):
    if angle == 0: return pos
    x = pos[0] - img.shape[1]*0.4
    y = pos[1] - img.shape[0]*0.4
    newx = x*cos(radians(angle)) + y*sin(radians(angle)) + img.shape[1]*0.4
    newy = -x*sin(radians(angle)) + y*cos(radians(angle)) + img.shape[0]*0.4
    return int(newx), int(newy), pos[2], pos[3]

#prende un frame dalla camera e lo ritorna
def captureFrame():
    
    if(INPUT_VIDEO_STREAM): 
        frame = vs.read()
        #frame = imutils.rotate(frame, angle=180)
        return frame
    else:
        #crea uno stream in memoria
        stream = io.BytesIO()
        #cattura il frame corrente dalla camera e lo inserisce nello stream in memoria
        camera.capture(stream, format='jpeg')
        #converte lo stream in memoria in un array numpy
        image_arr = np.fromstring(stream.getvalue(), dtype=np.uint8)
        #crea una immagine opencv dall'array numpy
        image = cv2.imdecode(image_arr, 1)
        #torna l'immagine ctturata
        return image

#detect ball
def detectBall(debug=False):
    detected = None

    #cattura un frame dalla camera
    frame = captureFrame()

    blurred = frame #cv2.GaussianBlur(frame, (11, 11), 0)
    hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)
    
    #applica i range di colori
    mask = None
    for minColor, maxColor in colorThresholds:
        #minColor = np.asarray(minColor, dtype=np.float32)
        #maxColor = np.asarray(maxColor, dtype=np.float32)
        if mask is None:
             mask = cv2.inRange(hsv, minColor, maxColor)
        else:
            mask |= cv2.inRange(hsv, minColor, maxColor)

    mask = cv2.erode(mask, None, iterations=2)
    mask = cv2.dilate(mask, None, iterations=2)
    
    #print center color
    #printRangeCenterColor(frame, 10)
        
    #trova i contorni
    contours  = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)[-2]
    center = None
    
    #scorre i contorni trovati
    for contour in contours:
        #ottiene il cerchio ed il centro
        ((x, y), radius) = cv2.minEnclosingCircle(contour)
        M = cv2.moments(contour)
        center = (int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"]))
        #print("radius %s" % radius)   
            
        #cerca la palla con il raggio maggiore
        maxRadius = MIN_RADIUS_THRWSHOLD
        maxCenter = (0,0)
        if radius > maxRadius:
            maxRadius = radius
            maxCenter = center
        '''    
        if radius > MIN_RADIUS_THRWSHOLD:
            if(debug): 
                print("ball found %s %s" % (radius, str(center)))
                cv2.circle(frame, (int(x), int(y)), int(radius), (0, 255, 255), 2)
                cv2.circle(frame, center, 5, (0, 0, 255), -1)

            #torna il cerchio trovato
            detected = (center, radius)
            break
        '''
    
    #se ha trovato la palla la ritorna
    if len(contours) > 0:
        if(debug): 
            print("ball found %s %s" % (radius, str(maxCenter)))
            cv2.circle(frame, maxCenter, int(maxRadius), (0, 255, 255), 2)
            cv2.circle(frame, maxCenter, 5, (0, 0, 255), -1)
        
        #torna il centro ed il raggio della palla individuata
        detected = (maxCenter, maxRadius)

    #salva l'immagine in un file
    if(debug): 
        cv2.imwrite('frames/'+time.strftime("%Y%m%d-%H%M%S")+'.mask.jpg', mask)
        cv2.imwrite('frames/'+time.strftime("%Y%m%d-%H%M%S")+'.jpg', frame)     

    #torna il cerchio trovato
    return detected 


#stampa il range di colori nella posizione centrale della immagine
#da provare intervallo (hMin,100,100), (hMax,255,255)    
def printRangeCenterColor(frame, interval=10, minRange=[255,255,255], maxRange=[0,0,0], debug=False):
    centerX, centerY = cameraW//2, cameraH//2
    
    #draw center cross
    if(debug):
        cv2.line(frame, (centerX-interval, centerY), (centerX+interval, centerY), (0, 255, 0), thickness=2)
        cv2.line(frame, (centerX, centerY-interval), (centerX, centerY+interval), (0, 255, 0), thickness=2)
        
    #scorre tutti i pixels intorno al centro in base all'intervallo
    for x in range(centerX-interval, centerX+interval):
        for y in range(centerY-interval, centerY+interval):
            pixelColor = frame[x,y]            
            
            #color conversion rgb to hsv
            pixelColor = cv2.cvtColor(np.uint8([[[pixelColor[0], pixelColor[1], pixelColor[2]]]]), cv2.COLOR_BGR2HSV)[0][0]   
            
            #set max e min colors
            for i in range(3):
                if pixelColor[i] < minRange[i]: minRange[i] = pixelColor[i]
                if pixelColor[i] > maxRange[i]: maxRange[i] = pixelColor[i]
            
    #if(debug): 
    print("center color range (%s, %s, %s), (%s, %s, %s)" % (minRange[0], minRange[1], minRange[2], maxRange[0], maxRange[1], maxRange[2]))
    
    return (minRange, maxRange)    


#segue la palla
def followBall(neckDegree, headDegree, debug=False):
        
    #gradi limite del collo
    MAX_NECK_DEGREE = 40
    MIN_NECK_DEGREE = -40
    MAX_HEAD_DEGREE = 40
    MIN_HEAD_DEGREE = -40
    ballCenter = None

    #pixel di soglia e step in base alla distanza dal centro
    PIXEL_THRESHOLD = 30
    #L_DEGREE_STEP = 4
    #M_DEGREE_STEP = 6
    #H_DEGREE_STEP = 8
    L_DEGREE_STEP = 2
    M_DEGREE_STEP = 4
    H_DEGREE_STEP = 6
    
    #cerca la palla all'interno del frame, e se la trova..
    detected = detectBall(debug=debug)
    if detected is not None:
        ballCenter, _ = detected

        #ottiene le differenze del centro rispetto alle coordinate della camera
        w_diff = ballCenter[0] - cameraW//2
        h_diff = ballCenter[1] - cameraH//2

        #decide di quanto muoversi in base alla distanza dal centro
        wDegreeStep = L_DEGREE_STEP
        if(abs(w_diff) > (cameraW//8)*3): wDegreeStep = H_DEGREE_STEP
        elif(abs(w_diff) > cameraW//4):   wDegreeStep = M_DEGREE_STEP
        hDegreeStep = L_DEGREE_STEP
        if(abs(h_diff) > (cameraH//8)*3): hDegreeStep = H_DEGREE_STEP
        elif(abs(h_diff) > cameraH//4):   hDegreeStep = M_DEGREE_STEP

        #decide in quale direzione andare
        if(w_diff > PIXEL_THRESHOLD):    neckDegree += wDegreeStep
        elif(w_diff < -PIXEL_THRESHOLD): neckDegree -= wDegreeStep
        if(h_diff > PIXEL_THRESHOLD):    headDegree += hDegreeStep
        elif(h_diff < -PIXEL_THRESHOLD): headDegree -= hDegreeStep

        #limita entro un range i movimenti
        neckDegree = max( min(neckDegree, MAX_NECK_DEGREE), MIN_NECK_DEGREE )
        headDegree = max( min(headDegree, MAX_HEAD_DEGREE), MIN_HEAD_DEGREE )

        #muove la testa
        jd.moveJoint(jd.NECK, neckDegree)
        jd.moveJoint(jd.HEAD, headDegree)

    #torna le nuove coordinate
    return ballCenter, neckDegree, headDegree

#detect face
def detectFace(debug=False):
    detected = None

    #cattura un frame dalla camera
    frame = captureFrame()
    
    #converte in scala di grigi
    frameGray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    #gira l'immagine nelle diverse angolazioni
    for angle in [0, -25, 25]:
        frameRotated = rotate_image(frameGray, angle)

        #effettua il detect dell'immagine
        detected = face_cascade.detectMultiScale(frameRotated, **faceDetectionSettings)
        if len(detected):
            #se ha trovato qualcosa ottiene il rettangolo riportandolo dopo avere ruotato al contrario le coordinate
            detected = [rotate_point(detected[-1], frameGray, -angle)]
            break
    
    if(debug):
        if detected is not None:
            print("face found %s" % str(detected))
        #visualizza sull'immagine i rettangoli trovati
        for x, y, w, h in detected[-1:]:
            cv2.rectangle(frame, (x, y), (x+w, y+h), (255,0,0), 2)
        #salva l'immagine in un file
        cv2.imwrite('frames/'+time.strftime("%Y%m%d-%H%M%S")+'.jpg', frame)

    #torna il rettangolo trovato
    return detected


#segue la faccia
def followFace(neckDegree, headDegree, debug=False):
        
    MAX_NECK_DEGREE = 35
    MIN_NECK_DEGREE = -35
    MAX_HEAD_DEGREE = 35
    MIN_HEAD_DEGREE = -35
    DEGREE_STEP_SIZE = 2
    PIXEL_THRESHOLD = 10
    faceRect = None

    #cerca la faccia all'interno del frame, e se la trova..
    detected = detectFace()
    if detected is not None:
        faceRect = detected[-1:]
        faceCenterX = faceRect[0] + faceRect[2]//2
        faceCenterY = faceRect[1] + faceRect[3]//2

        #ottiene differenze con le coordinate della camera
        w_diff = faceCenterX - cameraW//2
        h_diff = faceCenterY - cameraH//2

        #decide in quale direzione andare
        if(w_diff > PIXEL_THRESHOLD):
            neckDegree += DEGREE_STEP_SIZE
            print("- collo a destra %s" % (w_diff))        
        if(w_diff < -PIXEL_THRESHOLD):
            neckDegree -= DEGREE_STEP_SIZE
            print("- collo a sinstra %s" % (w_diff))
        if(h_diff > PIXEL_THRESHOLD):
            headDegree += DEGREE_STEP_SIZE
            print("- testa su %s" % (h_diff))
        if(h_diff < -PIXEL_THRESHOLD):
            headDegree -= DEGREE_STEP_SIZE
            print("- testa giu %s" % (h_diff))

        #limita entro un range i movimenti
        neckDegree = max( min(neckDegree, MAX_NECK_DEGREE), MIN_NECK_DEGREE )
        headDegree = max( min(headDegree, MAX_HEAD_DEGREE), MIN_HEAD_DEGREE )

        #muove la testa
        jd.moveJoint(jd.NECK, neckDegree)
        jd.moveJoint(jd.HEAD, headDegree)

    #torna le nuove coordinate
    return faceRect, neckDegree, headDegree

#register images from cam
def registerImages(outputPath='frames/output.avi', seconds=60):
    diffTime = 0
    startTime = time.time()
    while(diffTime < seconds):
        diffTime = time.time() - startTime
        frame = captureFrame()
        cv2.imwrite('frames/'+time.strftime("%Y%m%d-%H%M%S")+'.jpg', frame)   

#stop detector
def stop():
    if(INPUT_VIDEO_STREAM): vs.stop()
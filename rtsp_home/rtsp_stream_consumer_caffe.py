import cv2 as cv
import numpy as np
from threading import Thread, Lock
import time

RTSP_STREAM = "rtsp://admincamera:adminpwd@192.168.1.7:554/stream1"
cap = cv.VideoCapture(RTSP_STREAM)
if not cap.isOpened():
	print('RTSP stream not found')
	exit(-1)
        
net = cv.dnn.readNetFromCaffe("model/weights-prototxt.txt", "model/res_ssd_300Dim.caffeModel")
processing = False
frame = None

def detect_face():
	global processing, frame
	processing = True
	(height, width) = frame.shape[:2]
	blob = cv.dnn.blobFromImage(cv.resize(frame, (300, 300)), 1.0, (300, 300), (104.0, 177.0, 123.0))
	net.setInput(blob)
	detections = net.forward()
	if(len(detections) > 0):
		for i in range(0, detections.shape[2]):
			confidence = detections[0, 0, i, 2]
			if confidence < 0.5: continue
			box = detections[0, 0, i, 3:7] * np.array([width, height, width, height])
			(x1, y1, x2, y2) = box.astype("int")		
			y = y1 - 10 if y1 - 10 > 10 else y1 + 10
			cv.rectangle(frame, (x1, y1), (x2, y2), (0, 0, 255), 2)
			
	#time.sleep(5)
	processing = False	
		
while(cap.isOpened()):
	try:
		ret, frame = cap.read()
		if not ret:
			print('frame empty')
			continue
            
		frame = cv.resize(frame, (800, 420), cv.INTER_AREA)
		if not processing:
			print("detected")
			t = Thread(target=detect_face)
			t.start()
		else:
			print("not detected")
		cv.imshow('frame', frame)
	
		if cv.waitKey(20) & 0xFF == ord('q'):
			break
	except:
		print("error")
		
cap.release()
cv.destroyAllWindows()

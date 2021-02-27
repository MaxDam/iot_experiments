#pip install paho-mqtt
import cv2 as cv
import paho.mqtt.client as mqtt
import base64
import numpy as np

RTSP_STREAM = "rtsp://localhost:8554/mystream"
MQTT_BROKER = "test.mosquitto.org"
MQTT_TOPIC_SEND = "myhome/mx/cserver/detect"
	
cap = cv.VideoCapture(RTSP_STREAM)
mqtt_client = mqtt.Client()
mqtt_client.connect(MQTT_BROKER, 1883)

faceCascade = cv.CascadeClassifier('model/haarcascade_frontalface_default.xml')
acquire_in_progress = False

def detect_and_send_face(acquiredFrame):
	global acquire_in_progress
	gray = cv.cvtColor(acquiredFrame, cv.COLOR_BGR2GRAY)
	faces = faceCascade.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5, minSize=(30, 30), flags=cv.CASCADE_SCALE_IMAGE)
	for (x, y, w, h) in faces:
		roi_color = acquiredFrame[y:y+h, x:x+w] 
		send_image(roi_color)
	acquire_in_progress = False
	
def send_image(faceFrame):
	_, buffer = cv.imencode('.jpg', faceFrame)
	jpg_as_text = base64.b64encode(buffer)
	#cv.imshow('frame', jpg_as_text)
	mqtt_client.publish(MQTT_TOPIC_SEND, jpg_as_text)
	
while(cap.isOpened()):
	ret, frame = cap.read()
	
	#cv.imshow('frame', frame)
	if not acquire_in_progress:
		detect_and_send_face(frame)
	
	if cv.waitKey(20) & 0xFF == ord('q'):
		break
		
cap.release()
cv.destroyAllWindows()
mqtt_client.disconnect()

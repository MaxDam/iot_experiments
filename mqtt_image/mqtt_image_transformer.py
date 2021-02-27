#pip install paho-mqtt

import base64
import cv2 as cv
import numpy as np
import paho.mqtt.client as mqtt

MQTT_BROKER = "test.mosquitto.org"
MQTT_RECEIVE = "myhome/mx/cserver"
MQTT_SEND = "myhome/mx/cserver/detect"
frame = np.zeros((240, 320, 3), np.uint8)
acquire_in_progress = False

def on_connect(client, userdata, flags, rc):
	print("Connected with result code "+str(rc))
	client.subscribe(MQTT_RECEIVE)
	
def on_message(client, userdata, msg):
	global frame
	img = base64.b64decode(msg.payload)
	npimg = np.frombuffer(img, dtype=np.uint8)
	frame = cv.imdecode(npimg, 1)
	
def detectFace(acquiredFrame):
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
	client.publish(MQTT_SEND, jpg_as_text)
			
faceCascade = cv.CascadeClassifier('haarcascade_frontalface_default.xml')

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(MQTT_BROKER, 1883)
client.loop_start()

while True:
	#cv.imshow("Stream", frame)
	if not acquire_in_progress:
		detectFace(frame)
		
	if cv.waitKey(1) & 0xFF == ord('q'):
		break
	
client.loop_stop()
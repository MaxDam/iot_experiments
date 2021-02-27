#pip install paho-mqtt

import cv2 as cv
import paho.mqtt.client as mqtt
import base64
import time

MQTT_BROKER = "test.mosquitto.org"
MQTT_SEND = "myhome/mx/cserver"

cap = cv.VideoCapture(0)
client = mqtt.Client()

client.connect(MQTT_BROKER, 1883)
try:
	while True:
		start = time.time()
		
		_, frame = cap.read()
		_, buffer = cv.imencode('.jpg', frame)
		jpg_as_text = base64.b64encode(buffer)
		
		client.publish(MQTT_SEND, jpg_as_text)
		end = time.time()
		t = end - start
		fps = 1/t
		print(fps)
except:
	cap.release()
	client.disconnect()
	
print("\nNow you can restart fresh")
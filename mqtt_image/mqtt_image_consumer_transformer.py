#pip install paho-mqtt

import base64
import cv2 as cv
import numpy as np
import paho.mqtt.client as mqtt

MQTT_BROKER = "test.mosquitto.org"
MQTT_RECEIVE = "myhome/mx/cserver/detect"
frame = np.zeros((240, 320, 3), np.uint8)

def on_connect(client, userdata, flags, rc):
	print("Connected with result code "+str(rc))
	client.subscribe(MQTT_RECEIVE)
	
def on_message(client, userdata, msg):
	global frame
	img = base64.b64decode(msg.payload)
	npimg = np.frombuffer(img, dtype=np.uint8)
	frame = cv.imdecode(npimg, 1)
	
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(MQTT_BROKER, 1883)
client.loop_start()

while True:
	cv.imshow("Stream", frame)
	if cv.waitKey(1) & 0xFF == ord('q'):
		break
	
client.loop_stop()
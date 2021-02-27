#pip install paho-mqtt
import cv2 as cv
import paho.mqtt.client as mqtt
import base64
import numpy as np

RTSP_STREAM = "rtsp://localhost:8554/mystream"
MQTT_BROKER = "test.mosquitto.org"
MQTT_TOPIC_SEND = "myhome/mx/cserver/detect"
	
DEBUG = False
if DEBUG:
	cap = cv.VideoCapture(0)
else:
	cap = cv.VideoCapture(RTSP_STREAM)
mqtt_client = mqtt.Client()
mqtt_client.connect(MQTT_BROKER, 1883)

net = cv.dnn.readNetFromCaffe("model/weights-prototxt.txt", "model/res_ssd_300Dim.caffeModel")
acquire_in_progress = False

def detect_and_send_face(acquiredFrame):
	(height, width) = acquiredFrame.shape[:2]
	blob = cv.dnn.blobFromImage(cv.resize(acquiredFrame, (300, 300)), 1.0, (300, 300), (104.0, 177.0, 123.0))
	net.setInput(blob)
	detections = net.forward()
	for i in range(0, detections.shape[2]):
		confidence = detections[0, 0, i, 2]
		if confidence < 0.5: continue
		box = detections[0, 0, i, 3:7] * np.array([width, height, width, height])
		(x1, y1, x2, y2) = box.astype("int")		
		if DEBUG:
			text = "{:.2f}%".format(confidence * 100) + " ( " + str(y2-y1) + ", " + str(x2-x1) + " )"
			y = y1 - 10 if y1 - 10 > 10 else y1 + 10
			cv.rectangle(acquiredFrame, (x1, y1), (x2, y2), (0, 0, 255), 2)
			cv.putText(acquiredFrame, text, (x1, y), cv.LINE_AA, 0.45, (0, 0, 255), 2)
			cv.imshow('frame', acquiredFrame)
		else:
			roi_color = acquiredFrame[y1:y2, x1:x2] 
			send_image(roi_color)
	
def send_image(faceFrame):
	_, buffer = cv.imencode('.jpg', faceFrame)
	jpg_as_text = base64.b64encode(buffer)
	mqtt_client.publish(MQTT_TOPIC_SEND, jpg_as_text)
	
while(cap.isOpened()):
	ret, frame = cap.read()
	
	if not acquire_in_progress:
		detect_and_send_face(frame)
	
	if cv.waitKey(20) & 0xFF == ord('q'):
		break
		
cap.release()
cv.destroyAllWindows()
mqtt_client.disconnect()

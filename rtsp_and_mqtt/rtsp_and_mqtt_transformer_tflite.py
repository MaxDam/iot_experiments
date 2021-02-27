#pip install tensorflow==2.3.0 --user
#pip install paho-mqtt
import cv2 as cv
import paho.mqtt.client as mqtt
import base64
import numpy as np
#from tflite_runtime.interpreter import Interpreter
from tensorflow.lite.python.interpreter import Interpreter
#print(tf.__version__)

RTSP_STREAM = "rtsp://localhost:8554/mystream"
MQTT_BROKER = "test.mosquitto.org"
MQTT_TOPIC_SEND = "myhome/mx/cserver/detect"
	
DEBUG = True
if DEBUG:
	cap = cv.VideoCapture(0)
else:
	cap = cv.VideoCapture(RTSP_STREAM)
mqtt_client = mqtt.Client()
mqtt_client.connect(MQTT_BROKER, 1883)

#interpreter = Interpreter(model_path="model/FaceMobileNet_Float32.tflite")
interpreter = Interpreter(model_path="model/ssd_mobilenet_v1_1_metadata_1.tflite")
interpreter.allocate_tensors()
input_details = interpreter.get_input_details()
height = input_details[0]['shape'][1]
width = input_details[0]['shape'][2]
output_details = interpreter.get_output_details()
acquire_in_progress = False

def detect_and_send_face(acquiredFrame):
	#acquiredFrame = cv.resize(acquiredFrame, (112, 112))
	#input_data = np.array(acquiredFrame, dtype=np.float32)
	acquiredFrame = cv.resize(acquiredFrame, (300, 300))
	input_data = np.array(acquiredFrame, dtype=np.uint8)
	interpreter.set_tensor(input_details[0]['index'], [input_data])
	interpreter.invoke()
	#print(output_details)
	boxes   = interpreter.get_tensor(output_details[0]['index'])[0]
	classes = interpreter.get_tensor(output_details[1]['index'])[0]
	scores  = interpreter.get_tensor(output_details[2]['index'])[0]
	#print(scores)
	for index, score in enumerate(scores):
		if score > 0.5:
			box = boxes[index]
			print(box)
			ymin = int(max(1, (box[0] * height)))
			xmin = int(max(1, (box[1] * width)))
			ymax = int(min(height, (box[2] * height)))
			xmax = int(min(width, (box[3] * width)))
			if DEBUG:
				cv.rectangle(acquiredFrame, (xmin,ymin), (xmax,ymax), (10, 255, 0), 2)
				cv.imshow('frame', acquiredFrame)
			else:
				roi_color = acquiredFrame[y_min:y_max, x_min:x_max] 
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

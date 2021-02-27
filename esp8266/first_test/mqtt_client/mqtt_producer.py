#pip install paho-mqtt

import paho.mqtt.client as mqtt
import sys

MQTT_BROKER = "test.mosquitto.org"
MQTT_SEND = "myhome/mx/cserver"

client = mqtt.Client()

client.connect(MQTT_BROKER, 1883)
try:
	client.publish(MQTT_SEND, sys.argv[1])
	print("message sent")
except:
	client.disconnect()
	
print("\nNow you can restart fresh")
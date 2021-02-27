#pip install paho-mqtt

import paho.mqtt.client as mqtt
import yaml
import logging
import requests
	
#callback - si connette al client
def on_connect(client, userdata, flags, rc):
	global mqtt_topic
	print("Connected with result code {0}".format(str(rc)))
	client.subscribe(mqtt_topic)
	#client.publish(mqtt_topic, "Hello world!");

#callback - ottiene il messaggio
def on_message(client, userdata, msg):
	global rest_url
	print("Message received-> " + msg.topic + " " + str(msg.payload))
	response = requests.post(rest_url, data=msg.payload)
	print("Rest response-> " + str(response.content))
	
#ottiene la configurazione
def getCfg(*keys):
	dct = cfg
	for key in keys:
		try:
			dct = dct[key]
		except KeyError:
			return None
	return dct

def mqtt_connect(name):
	global mqtt_ip, mqtt_port
	
	#crea il client mqtt
	client = mqtt.Client(name)
	client.on_connect = on_connect
	client.on_message = on_message
	client.connect(mqtt_ip, mqtt_port, 60)
	client.loop_forever()

	
try:	
	#legge le config
	with open("mqtt_adapter.yml", 'r') as ymlfile:
		cfg = yaml.safe_load(ymlfile)
	mqtt_ip    = getCfg('mqtt', 'ip')
	mqtt_port  = getCfg('mqtt', 'port')
	mqtt_topic = getCfg('mqtt', 'topic')
	rest_url   = getCfg('rest', 'url')

	#si connette al broker
	mqtt_connect('clientAdapter')
	
except Exception as e:
	print("A global exception occurred ", e)
	logging.exception("A global exception occurred ")
	logging.exception("message")


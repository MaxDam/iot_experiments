
//tutorial:
//https://theiotprojects.com/measure-pitch-roll-and-yaw-angles-using-mpu6050-and-arduino/
//https://www.youtube.com/watch?v=Q_eByFDvSC4

//set enviroment for esp8266:
//file->preferences->http://arduino.esp8266.com/stable/paoutput_topicckage_esp8266com_index.json

//update core esp8266:
//Tools->Board: XX>Boards Manager..->search: ESP8266 and update to the last version of library

//install libraries:
//Sketch->Include Library->Manage Libraries->
//search: Adafruit MPU6050, install last version of library and last version of the included libraries (Adafruit_BusIO, Adafruit_GFX_Library and Adafruit_Unified_Sensor)
//search: PubSubClient and install last version of library
//search: ArduinoJson and install last version of library

#include <ESP8266WiFi.h>
#include <Wire.h>

#include <ArduinoJson.h>
#include <PubSubClient.h>

#include <MPU6050_light.h>

#include <SparkFun_MAG3110.h>

#define LED 2

const char* ssid = "Vodafone-C01960075";
const char* password = "tgYsZkgHA4xhJLGy";
//const char* mqtt_server = "6f2bddbb318d4bc3b9496192a5073062.s1.eu.hivemq.cloud";
//const int   mqtt_port = 8883;
char* mqtt_server = "test.mosquitto.org";
//char* mqtt_server = "5.196.95.208";
//char* mqtt_server = "192.168.1.9";
const int   mqtt_port = 1883;
const char* output_topic = "esp8266/test-max";
const char *mqtt_username = "hivemax";
const char *mqtt_password = "HivePwd1";

WiFiClient espClient;
//WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);
#define MSG_BUFFER_SIZE  (128) //il server MQTT accetta messaggio fino a 128 caratteri, altrimenti li scarta senza dire nulla
char msg[MSG_BUFFER_SIZE];
bool ledState = 1;
String deviceId;
String clientId;

MPU6050 mpu(Wire);
unsigned long timer = 0;

int MIN_DEG = -90;
int MAX_DEG = +90;
int pitch  = 0;
int roll   = 0;
int yaw    = 0;
  
static const char *fingerprint PROGMEM = "44 14 9A 3F C3 E9 F1 F3 84 1A B4 9F B6 4D 19 8A B2 92 31 D6";                                                                                           
     
MAG3110 mag = MAG3110();
int mX, mY, mZ;

//Inizializza il sensore Gyroscopio+Accelerometro
void initMPU6050(){
  Serial.print("MPU6050 setup: ");
  digitalWrite(LED, HIGH);

  Wire.begin();
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while (status != 0) { } // stop everything if could not connect to MPU6050
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcOffsets(); // gyro and accelero
  Serial.println("Done!\n");
  
  digitalWrite(LED, LOW);
  delay(1000);
}

//Inizializza il sensore di Orientation
void initMAG3110() {
  Serial.print("MAG3110 setup: ");
  digitalWrite(LED, HIGH);

  mag.initialize();
  Serial.println("Done!\n");
  
  digitalWrite(LED, LOW);
  delay(1000);
}

//inizializza la WiFi
void initWiFi() {
  digitalWrite(LED, HIGH);
  
  // Connect to the network
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  // Wait for the Wi-Fi to connect
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  // Send the IP address of the ESP8266 to the computer
  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
  Serial.println("");
  
  digitalWrite(LED, LOW);
  delay(1000);
}

//inizializza l'mqtt
void initMQTT() {
  digitalWrite(LED, HIGH);
  
  Serial.println("init MQTT ....");

  //espClient.setFingerprint(fingerprint);
  //espClient.setInsecure();

  // Create a random client ID
  //i server mqtt ha un timeout di connessione legato allo stesso id, quindi meglio generarlo random
  clientId = String(random(3000))+String(random(3000))+String(random(3000))+String(random(3000))+String(random(3000))+String(random(3000));
  clientId += String(random(0xffff), HEX);
  //clientId += "-ESP8266Client"; 
  Serial.print("ClientId: ");
  Serial.print(clientId);
  Serial.println();
    
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);

  digitalWrite(LED, LOW);
  delay(1000);
}

//callback messaggio
void callback(char* topic, byte* payload, unsigned int length) {
  /*
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  */
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}

//riconnessione mqtt
void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      mqttClient.publish(output_topic, "hello world");
      // ... and resubscribe
      mqttClient.subscribe(output_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  deviceId = String(random(3000));
  //deviceId = String(WiFi.macAddress());
  initWiFi();
  initMPU6050();
  initMAG3110();
  initMQTT();
}

void loop() {

  //controlla la connessione MQTT
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
  
  //lampeggia il led
  digitalWrite(LED, ledState);
  ledState = !ledState;

  //ottiene i dati dal sensore
  mpu.update();
  
  pitch  = mpu.getAngleX();
  roll   = mpu.getAngleY();
  yaw    = mpu.getAngleZ();
  pitch  =  map(pitch,-90, +90, MIN_DEG, MAX_DEG);
  roll   = -map(roll, -90, +90, MIN_DEG, MAX_DEG);
  yaw    =  map(yaw,  -90, +90, MIN_DEG, MAX_DEG);

  //Orientation
  readMag();
  
  //inizializza e riempie il json di risposta
  DynamicJsonDocument readings(1024);
  
  //readings["id"] = deviceId;

  readings["gX"] = pitch;
  readings["gY"] = roll;
  readings["gZ"] = yaw;

  //readings["gX"] = mpu.getGyroX();
  //readings["gY"] = mpu.getGyroY();
  //readings["gZ"] = mpu.getGyroZ();
  
  readings["aX"] = mpu.getAccX();
  readings["aY"] = -mpu.getAccY();
  readings["aZ"] = mpu.getAccZ();
  
  readings["tp"] = mpu.getTemp();

  //prepara il messaggio
  String telemetry = "";
  serializeJson(readings, telemetry);
  //Serial.println(telemetry);

  //invia i dati in mqtt
  snprintf (msg, MSG_BUFFER_SIZE, telemetry.c_str());
  //Serial.print("Publish message: ");
  Serial.println(msg);
  mqttClient.publish(output_topic, msg);
  
  //attende
  //delay(100);
}

//read orientation from magnetometer
void readMag() {
	// If we are calibrated, display heading
  if(mag.isCalibrated() ) {
    //Serial.println(mag.readHeading());
    delay(100);
  } 
  else {
	Serial.println("Calibratin");
    Serial.println("Rotate me!");
    // Calibrate or enter into calibration mode
    if(mag.isCalibrating()) {
      mag.calibrate();
    } else {
      mag.enterCalMode();
    }
    delay(10);
  }
  
  
  if(!mag.isCalibrated()) //If we're not calibrated
  {
    if(!mag.isCalibrating()) //And we're not currently calibrating
    {
      Serial.println("Entering calibration mode");
      mag.enterCalMode(); //This sets the output data rate to the highest possible and puts the mag sensor in active mode
    }
    else
    {
      //Must call every loop while calibrating to collect calibration data
      //This will automatically exit calibration
      //You can terminate calibration early by calling mag.exitCalMode();
      mag.calibrate(); 
    }
  }
  else //We are calibrated
  {
    mag.readMag(&mX, &mY, &mZ);
    float magnitude = sqrt(pow(mX*mag.x_scale, 2) + pow(mY*mag.y_scale, 2));
    
    mX = mX/magnitude;
    mY = mY/magnitude;
    mZ = mZ/magnitude;
    
	Serial.print("X: ");
    Serial.print(mX/1000);
    Serial.print(", Y: ");
    Serial.print(mY/1000);
    Serial.print(", Z: ");
    Serial.print(mZ/1000);
    Serial.print(", mag: ");
    Serial.println(magnitude);
	
	/*
	pitch  = mY/1000;
	roll   = mZ/1000;
	yaw    = mX/1000;
	pitch  =  map(pitch,??, ??, MIN_DEG, MAX_DEG);
	roll   = -map(roll, ??, ??, MIN_DEG, MAX_DEG);
	yaw    =  map(yaw,  ??, ??, MIN_DEG, MAX_DEG);
	*/
  }
  delay(20);
}

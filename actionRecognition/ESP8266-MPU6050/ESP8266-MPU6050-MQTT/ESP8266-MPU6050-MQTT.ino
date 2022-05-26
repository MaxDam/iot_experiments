
//tutorial:
//https://randomnerdtutorials.com/esp8266-nodemcu-mpu-6050-accelerometer-gyroscope-arduino/

//set enviroment for esp8266:
//file->preferences->http://arduino.esp8266.com/stable/package_esp8266com_index.json

//update core esp8266:
//Tools->Board: XX>Boards Manager..->search: ESP8266 and update to the last version of library

//install libraries:
//Sketch->Include Library->Manage Libraries->
//search: Adafruit MPU6050, install last version of library and last version of the included libraries (Adafruit_BusIO, Adafruit_GFX_Library and Adafruit_Unified_Sensor)
//search: PubSubClient and install last version of library
//search: ArduinoJson and install last version of library

#include <ESP8266WiFi.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <PubSubClient.h>

#define LED 2

const char* ssid = "Vodafone-C01960075";
const char* password = "tgYsZkgHA4xhJLGy";
//const char* mqtt_server = "broker.hivemq.com";
const char* mqtt_server = "52.59.17.149";
//const char* mqtt_server = "broker.emqx.io";
const int   mqtt_port = 1883;
const char* output_topic = "esp8266/test-max";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";

WiFiClient espClient;
PubSubClient client(espClient);
#define MSG_BUFFER_SIZE  (1024)
char msg[MSG_BUFFER_SIZE];
bool ledState = 1;

// Timer variables
unsigned long lastTime = 0;  
unsigned long lastTimeTemperature = 0;
unsigned long lastTimeAcc = 0;
unsigned long gyroDelay = 10;
unsigned long temperatureDelay = 1000;
unsigned long accelerometerDelay = 200;

// Create a sensor object
Adafruit_MPU6050 mpu;

sensors_event_t a, g, temp;

float gyroX, gyroY, gyroZ;
float accX, accY, accZ;
float temperature;

//Gyroscope sensor deviation
float gyroXerror = 0.07;
float gyroYerror = 0.03;
float gyroZerror = 0.01;

//Inizializza il sensore
void initMPU6050(){
  digitalWrite(LED, HIGH);
  
  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }
  Serial.println("");

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
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  digitalWrite(LED, LOW);
  delay(1000);
}

//callback messaggio
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

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
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    //clientId += String(random(0xffff), HEX);
    clientId += String(WiFi.macAddress());
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("myhome/mx/cserver", "hello world");
      // ... and resubscribe
      client.subscribe("myhome/mx/cserver");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  initWiFi();
  initMPU6050();
  initMQTT();
}

unsigned long timer = 0;
float timeStep = 0.01;


void loop() {

  //controlla la connessione MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  //lampeggia il led
  digitalWrite(LED, ledState);
  ledState = !ledState;

  //ottiene i dati dal sensore
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  //calibra il giroscopio
  float gyroX_temp = g.gyro.x;
  if(abs(gyroX_temp) > gyroXerror)  {
    gyroX += gyroX_temp/50.00;
  }
  float gyroY_temp = g.gyro.y;
  if(abs(gyroY_temp) > gyroYerror) {
    gyroY += gyroY_temp/70.00;
  }
  float gyroZ_temp = g.gyro.z;
  if(abs(gyroZ_temp) > gyroZerror) {
    gyroZ += gyroZ_temp/90.00;
  }
  
  //inizializza e riempie il json di risposta
  DynamicJsonDocument readings(1024);
  
  readings["gyroX"] = gyroX;
  readings["gyroY"] = gyroY;
  readings["gyroZ"] = gyroZ;
  
  readings["accX"] = a.acceleration.x;
  readings["accY"] = a.acceleration.y;
  readings["accZ"] = a.acceleration.z;
  
  readings["temperature"] = temp.temperature;

  //prepara il messaggio
  String telemetry = "";
  serializeJson(readings, telemetry);
  Serial.println(telemetry);

  //invia i dati in mqtt
  snprintf (msg, MSG_BUFFER_SIZE, telemetry.c_str());
  Serial.print("Publish message: ");
  Serial.println(msg);
  client.publish(output_topic, msg);

  //attende
  delay(500);
}

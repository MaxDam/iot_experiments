//from: https://randomnerdtutorials.com/esp32-mpu-6050-web-server/

//LIBS:
//adafruit mpu6050
//Adafruit Unified Sensor
//Adafruit Bus IO
//Arduino_JSON
//https://github.com/me-no-dev/ESPAsyncWebServer
//https://github.com/me-no-dev/AsyncTCP

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <L3G4200D.h>
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
L3G4200D gyroscope;

float gyroX, gyroY, gyroZ;
float accX, accY, accZ;
float temperature;

//Gyroscope sensor deviation
float gyroXerror = 0.07;
float gyroYerror = 0.03;
float gyroZerror = 0.01;

// Init L3G4200D
void initL3G4200D(){
  // Initialize L3G4200D
  Serial.println("Initialize L3G4200D");

  // Set scale 2000 dps and 400HZ Output data rate (cut-off 50)
  while(!gyroscope.begin(L3G4200D_SCALE_2000DPS, L3G4200D_DATARATE_400HZ_50))
  {
    Serial.println("Could not find a valid L3G4200D sensor, check wiring!");
    delay(500);
  }
 
  // Calibrate gyroscope. The calibration must be at rest.
  // If you don't want calibrate, comment this line.
  gyroscope.calibrate(100);
}

// Initialize WiFi
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
}

void initMQTT() {
  Serial.println("init MQTT ....");
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

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
  initL3G4200D();
  initMQTT();
}

unsigned long timer = 0;
float timeStep = 0.01;

// Pitch, Roll and Yaw values
float pitch = 0;
float roll = 0;
float yaw = 0;

void loop() {
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  digitalWrite(LED, ledState);
  ledState = !ledState;

  DynamicJsonDocument readings(1024);
  
  Vector norm = gyroscope.readNormalize();
  readings["gyroX"] = norm.XAxis;
  readings["gyroY"] = norm.YAxis;
  readings["gyroZ"] = norm.ZAxis;
  
  readings["accX"] = 0;
  readings["accY"] = 0;
  readings["accZ"] = 0;
  
  readings["temperature"] = 0;
  
  String telemetry = "";
  serializeJson(readings, telemetry);
  Serial.println(telemetry);

  snprintf (msg, MSG_BUFFER_SIZE, telemetry.c_str());
  Serial.print("Publish message: ");
  Serial.println(msg);
  client.publish(output_topic, msg);
  
  delay(1000);
}

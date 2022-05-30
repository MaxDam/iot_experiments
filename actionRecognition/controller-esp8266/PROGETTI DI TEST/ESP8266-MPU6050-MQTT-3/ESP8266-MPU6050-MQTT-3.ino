
//tutorial:
//https://www.giuseppecaccavale.it/arduino/roll-pitch-e-yaw-con-mpu6050-arduino/

//set enviroment for esp8266:
//file->preferences->http://arduino.esp8266.com/stable/paoutput_topicckage_esp8266com_index.json

//update core esp8266:
//Tools->Board: XX>Boards Manager..->search: ESP8266 and update to the last version of library

//install libraries:
//Sketch->Include Library->Manage Libraries->
//install from zip MPU library
//search: PubSubClient and install last version of library
//search: ArduinoJson and install last version of library

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <MPU6050.h>

#define LED 2

const char* ssid = "Vodafone-C01960075";
const char* password = "tgYsZkgHA4xhJLGy";
//char* mqtt_server = "192.168.1.9";
char* mqtt_server = "test.mosquitto.org";
const int   mqtt_port = 1883;
const char* output_topic = "esp8266/test-max";
const char *mqtt_username = "test";
const char *mqtt_password = "test";

WiFiClient espClient;
//WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);
#define MSG_BUFFER_SIZE  (128) //il server MQTT accetta messaggio fino a 128 caratteri, altrimenti li scarta senza dire nulla
char msg[MSG_BUFFER_SIZE];
bool ledState = 1;
String deviceId;
MPU6050 mpu;

int pitch = 0;
int roll = 0;
float yaw = 0;

static const char *fingerprint PROGMEM = "44 14 9A 3F C3 E9 F1 F3 84 1A B4 9F B6 4D 19 8A B2 92 31 D6";                                                                                           
     
//Inizializza il sensore
void initMPU6050(){
  digitalWrite(LED, HIGH);
  
  Serial.println("Adafruit MPU6050 test!");

 while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G)) {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }

  // Calibrate gyroscope. The calibration must be at rest.
  // If you don't want calibrate, comment this line.
  mpu.calibrateGyro();

  // Set threshold sensivty. Default 3.
  // If you don't want use threshold, comment this line or set 0.
  mpu.setThreshold(1);

  // Check settings
  checkSettings();
  
  digitalWrite(LED, LOW);
  delay(1000);
}

void checkSettings() {
  Serial.println();

  Serial.print(" * Sleep Mode:        ");
  Serial.println(mpu.getSleepEnabled() ? "Enabled" : "Disabled");

  Serial.print(" * Clock Source:      ");
  switch (mpu.getClockSource()) {
    case MPU6050_CLOCK_KEEP_RESET:     Serial.println("Stops the clock and keeps the timing generator in reset"); break;
    case MPU6050_CLOCK_EXTERNAL_19MHZ: Serial.println("PLL with external 19.2MHz reference"); break;
    case MPU6050_CLOCK_EXTERNAL_32KHZ: Serial.println("PLL with external 32.768kHz reference"); break;
    case MPU6050_CLOCK_PLL_ZGYRO:      Serial.println("PLL with Z axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_YGYRO:      Serial.println("PLL with Y axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_XGYRO:      Serial.println("PLL with X axis gyroscope reference"); break;
    case MPU6050_CLOCK_INTERNAL_8MHZ:  Serial.println("Internal 8MHz oscillator"); break;
  }

  Serial.print(" * Gyroscope:         ");
  switch (mpu.getScale()) {
    case MPU6050_SCALE_2000DPS:        Serial.println("2000 dps"); break;
    case MPU6050_SCALE_1000DPS:        Serial.println("1000 dps"); break;
    case MPU6050_SCALE_500DPS:         Serial.println("500 dps"); break;
    case MPU6050_SCALE_250DPS:         Serial.println("250 dps"); break;
  }

  Serial.print(" * Gyroscope offsets: ");
  Serial.print(mpu.getGyroOffsetX());
  Serial.print(" / ");
  Serial.print(mpu.getGyroOffsetY());
  Serial.print(" / ");
  Serial.println(mpu.getGyroOffsetZ());

  Serial.println();
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
  
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);

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
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    //i server mqtt ha un timeout di connessione legato allo stesso id, quindi meglio generarlo random
    String clientId = String(random(3000))+String(random(3000))+String(random(3000))+String(random(3000))+String(random(3000))+String(random(3000));
    clientId += String(random(0xffff), HEX);
    //clientId += "-ESP8266Client"; 
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
  //printDNSServers();
  //printIPAddressOfHost("test.mosquitto.org");
  initMPU6050();
  initMQTT();
}

void printDNSServers() {
  Serial.print("DNS #1, #2 IP: ");
  WiFi.dnsIP().printTo(Serial);
  Serial.print(", ");
  WiFi.dnsIP(1).printTo(Serial);
  Serial.println();
}

void printIPAddressOfHost(const char* host) {
  IPAddress resolvedIP;
  if (!WiFi.hostByName(host, resolvedIP)) {
    Serial.println("DNS lookup failed.  Rebooting...");
    Serial.flush();
    ESP.reset();
  }
  Serial.print(host);
  Serial.print(" IP: ");
  Serial.println(resolvedIP);
}

unsigned long timer = 0;
float timeStep = 0.01;


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
  // Read normalized values
  Vector normAccel = mpu.readNormalizeAccel();
  Vector normGyro = mpu.readNormalizeGyro();
  // Calculate Pitch & Roll
  pitch = -(atan2(normAccel.XAxis, sqrt(normAccel.YAxis * normAccel.YAxis + normAccel.ZAxis * normAccel.ZAxis)) * 180.0) / M_PI;
  roll = (atan2(normAccel.YAxis, normAccel.ZAxis) * 180.0) / M_PI;
  //Ignore the gyro if our angular velocity does not meet our threshold
  if (normGyro.ZAxis > 1 || normGyro.ZAxis < -1) {
    normGyro.ZAxis /= 100;
    yaw += normGyro.ZAxis;
  }
  //Keep our angle between 0-359 degrees
  if (yaw < 0)
    yaw += 360;
  else if (yaw > 359)
    yaw -= 360;
  
  //inizializza e riempie il json di risposta
  DynamicJsonDocument readings(1024);
  
  //readings["id"] = deviceId;
  
  readings["gX"] = pitch;
  readings["gY"] = roll;
  readings["gZ"] = yaw;
  
  readings["aX"] = 0;
  readings["aY"] = 0;
  readings["aZ"] = 0;
  
  readings["tp"] = 0;

  //prepara il messaggio
  String telemetry = "";
  serializeJson(readings, telemetry);
  Serial.println(telemetry);

  //invia i dati in mqtt
  snprintf (msg, MSG_BUFFER_SIZE, telemetry.c_str());
  Serial.print("Publish message: ");
  Serial.println(msg);
  mqttClient.publish(output_topic, msg);
  
  //attende
  delay(100);
}


//tutorial:
//https://howtomechatronics.com/tutorials/arduino/arduino-and-mpu6050-accelerometer-and-gyroscope-tutorial/

//set enviroment for esp8266:
//file->preferences->http://arduino.esp8266.com/stable/paoutput_topicckage_esp8266com_index.json

//update core esp8266:
//Tools->Board: XX>Boards Manager..->search: ESP8266 and update to the last version of library

//install libraries:
//Sketch->Include Library->Manage Libraries->
//search: PubSubClient and install last version of library
//search: ArduinoJson and install last version of library

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <PubSubClient.h>

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

const int MPU = 0x68; // MPU6050 I2C address
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime;
int c = 0;

static const char *fingerprint PROGMEM = "44 14 9A 3F C3 E9 F1 F3 84 1A B4 9F B6 4D 19 8A B2 92 31 D6";                                                                                           
     
//Inizializza il sensore
void initMPU6050(){
  digitalWrite(LED, HIGH);
  
  Serial.println("Adafruit MPU6050 test!");

  Serial.begin(19200);
  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission
  /*
  // Configure Accelerometer Sensitivity - Full Scale Range (default +/- 2g)
  Wire.beginTransmission(MPU);
  Wire.write(0x1C);                  //Talk to the ACCEL_CONFIG register (1C hex)
  Wire.write(0x10);                  //Set the register bits as 00010000 (+/- 8g full scale range)
  Wire.endTransmission(true);
  // Configure Gyro Sensitivity - Full Scale Range (default +/- 250deg/s)
  Wire.beginTransmission(MPU);
  Wire.write(0x1B);                   // Talk to the GYRO_CONFIG register (1B hex)
  Wire.write(0x10);                   // Set the register bits as 00010000 (1000deg/s full scale)
  Wire.endTransmission(true);
  delay(20);
  */
  // Call this function if you need to get the IMU error values for your module
  calculate_IMU_error();
  delay(20);
  
  digitalWrite(LED, LOW);
  delay(1000);
}

void calculate_IMU_error() {
  // We can call this funtion in the setup section to calculate the accelerometer and gyro data error. From here we will get the error values used in the above equations printed on the Serial Monitor.
  // Note that we should place the IMU flat in order to get the proper values, so that we then can the correct values
  // Read accelerometer values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    // Sum all readings
    AccErrorX = AccErrorX + ((atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI));
    AccErrorY = AccErrorY + ((atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI));
    c++;
  }
  //Divide the sum by 200 to get the error value
  AccErrorX = AccErrorX / 200;
  AccErrorY = AccErrorY / 200;
  c = 0;
  // Read gyro values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    GyroX = Wire.read() << 8 | Wire.read();
    GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();
    // Sum all readings
    GyroErrorX = GyroErrorX + (GyroX / 131.0);
    GyroErrorY = GyroErrorY + (GyroY / 131.0);
    GyroErrorZ = GyroErrorZ + (GyroZ / 131.0);
    c++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX = GyroErrorX / 200;
  GyroErrorY = GyroErrorY / 200;
  GyroErrorZ = GyroErrorZ / 200;
  // Print the error values on the Serial Monitor
  Serial.print("AccErrorX: ");
  Serial.println(AccErrorX);
  Serial.print("AccErrorY: ");
  Serial.println(AccErrorY);
  Serial.print("GyroErrorX: ");
  Serial.println(GyroErrorX);
  Serial.print("GyroErrorY: ");
  Serial.println(GyroErrorY);
  Serial.print("GyroErrorZ: ");
  Serial.println(GyroErrorZ);
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
    String clientId = String(random(3000));
    clientId += String(random(0xffff), HEX);
    clientId += "-ESP8266Client"; 
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
  // === Read acceleromter data === //
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value
  // Calculating Roll and Pitch from the accelerometer data
  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - 0.58; // AccErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) + 1.58; // AccErrorY ~(-1.58)
  // === Read gyroscope data === //
  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
  elapsedTime = 0;
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
  // Correct the outputs with the calculated error values
  GyroX = GyroX + 0.56; // GyroErrorX ~(-0.56)
  GyroY = GyroY - 2; // GyroErrorY ~(2)
  GyroZ = GyroZ + 0.79; // GyroErrorZ ~ (-0.8)
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX = gyroAngleX + GyroX * elapsedTime; // deg/s * s = deg
  gyroAngleY = gyroAngleY + GyroY * elapsedTime;
  yaw =  yaw + GyroZ * elapsedTime;
  // Complementary filter - combine acceleromter and gyro angle values
  roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
  
  //inizializza e riempie il json di risposta
  DynamicJsonDocument readings(1024);
  
  //readings["id"] = deviceId;
  
  readings["gX"] = roll;
  readings["gY"] = pitch;
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
  //delay(100);
}

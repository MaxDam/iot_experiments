
//tutorial:
//https://randomnerdtutorials.com/esp8266-nodemcu-mpu-6050-accelerometer-gyroscope-arduino/

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
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <PubSubClient.h>

#define LED 2

const char* ssid = "Vodafone-C01960075";
const char* password = "tgYsZkgHA4xhJLGy";
//const char* mqtt_server = "6f2bddbb318d4bc3b9496192a5073062.s1.eu.hivemq.cloud";
//const int   mqtt_port = 8883;
char* mqtt_server = "test.mosquitto.org";
//char* mqtt_server = "5.196.95.208";
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

// Timer variables
unsigned long lastTime = 0;  
unsigned long lastTimeTemperature = 0;
unsigned long lastTimeAcc = 0;
unsigned long gyroDelay = 10;
unsigned long temperatureDelay = 1000;
unsigned long accelerometerDelay = 200;

// Create a sensor object
Adafruit_MPU6050 mpu;

String deviceId;
float accX, accY, accZ;
float gyroX, gyroY, gyroZ; 
float temperature;

//Gyroscope sensor deviation
float gyroXerror = 0.07;
float gyroYerror = 0.03;
float gyroZerror = 0.01;

//codice sensor fusion
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float accErrorX, accErrorY, gyroErrorX, gyroErrorY, gyroErrorZ;
float elapsedTime, currentTime, previousTime;
int c = 0;

//codice rotation
float rotationX, rotationY, rotationZ;
float rotationX_error = 0.05;
float rotationY_error = 0.02;
float rotationZ_error = 0.01;

static const char *fingerprint PROGMEM = "44 14 9A 3F C3 E9 F1 F3 84 1A B4 9F B6 4D 19 8A B2 92 31 D6";                                                                                           
     
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

  /*
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

  calculate_IMU_error();
  */
  
  digitalWrite(LED, LOW);
  delay(1000);
}

void calculate_IMU_error() {
  // We can call this funtion in the setup section to calculate the accelerometer and gyro data error. From here we will get the error values used in the above equations printed on the Serial Monitor.
  // Note that we should place the IMU flat in order to get the proper values, so that we then can the correct values
  // Read accelerometer values 200 times
  while (c < 200) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    
    accX = a.acceleration.x;
    accY = a.acceleration.y;
    accZ = a.acceleration.z;
    // Sum all readings
    accErrorX = accErrorX + ((atan((accY) / sqrt(pow((accX), 2) + pow((accZ), 2))) * 180 / PI));
    accErrorY = accErrorY + ((atan(-1 * (accX) / sqrt(pow((accY), 2) + pow((accZ), 2))) * 180 / PI));
    c++;
  }
  //Divide the sum by 200 to get the error value
  accErrorX = accErrorX / 200;
  accErrorY = accErrorY / 200;
  c = 0;
  // Read gyro values 200 times
  while (c < 200) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    gyroX = g.gyro.x;
    gyroY = g.gyro.y;
    gyroZ = g.gyro.z;
    // Sum all readings
    gyroErrorX = gyroErrorX + (gyroX / 131.0);
    gyroErrorY = gyroErrorY + (gyroY / 131.0);
    gyroErrorZ = gyroErrorZ + (gyroZ / 131.0);
    c++;
  }
  //Divide the sum by 200 to get the error value
  gyroErrorX = gyroErrorX / 200;
  gyroErrorY = gyroErrorY / 200;
  gyroErrorZ = gyroErrorZ / 200;
  // Print the error values on the Serial Monitor
  Serial.print("AccErrorX: ");
  Serial.println(accErrorX);
  Serial.print("AccErrorY: ");
  Serial.println(accErrorY);
  Serial.print("GyroErrorX: ");
  Serial.println(gyroErrorX);
  Serial.print("GyroErrorY: ");
  Serial.println(gyroErrorY);
  Serial.print("GyroErrorZ: ");
  Serial.println(gyroErrorZ);
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
  printDNSServers();
  printIPAddressOfHost("test.mosquitto.org");
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

  gyroX = g.gyro.x;
  gyroY = g.gyro.y;
  gyroZ = g.gyro.z;




  //-----------------------------------------------------
  /*
  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
  accX = a.acceleration.x;
  accY = a.acceleration.y;
  accZ = a.acceleration.z;
  accAngleX = (atan(accY / sqrt(pow(accX, 2) + pow(accZ, 2))) * 180 / PI) - 0.58; // accErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
  accAngleY = (atan(-1 * accX / sqrt(pow(accY, 2) + pow(accZ, 2))) * 180 / PI) + 1.58; // accErrorY ~(-1.58)
  gyroX = gyroX + 0.56; // gyroErrorX ~(-0.56)
  gyroY = gyroY - 2; // gyroErrorY ~(2)
  gyroZ = gyroZ + 0.79; // gyroErrorZ ~ (-0.8)
  gyroAngleX = gyroAngleX + gyroX * elapsedTime; // deg/s * s = deg
  gyroAngleY = gyroAngleY + gyroY * elapsedTime;
  yaw =  yaw + gyroZ * elapsedTime;
  // Complementary filter - combine acceleromter and gyro angle values
  roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
  gyroX = pitch;
  gyroY = roll;
  gyroZ = yaw;
  */
  //------------------------------------------------------- 
  /*
  float rotationX_temporary = g.gyro.x;
  if(abs(rotationX_temporary) > rotationX_error)  {
    rotationX += rotationX_temporary*0.01;
  }
  float rotationY_temporary = g.gyro.y;
  if(abs(rotationY_temporary) > rotationY_error) {
    rotationY += rotationY_temporary*0.01;
  }
  float rotationZ_temporary = g.gyro.z;
  if(abs(rotationZ_temporary) > rotationZ_error) {
    rotationZ += rotationZ_temporary*0.01;
  }
  gyroX = rotationX;
  gyroY = rotationY;
  gyroZ = rotationZ;
  */
  //------------------------------------------------------- 
  
  
  //inizializza e riempie il json di risposta
  DynamicJsonDocument readings(1024);
  
  //readings["id"] = deviceId;
  
  readings["gX"] = gyroX;
  readings["gY"] = gyroY;
  readings["gZ"] = gyroZ;
  
  readings["aX"] = a.acceleration.x;
  readings["aY"] = a.acceleration.y;
  readings["aZ"] = a.acceleration.z;

/*
  readings["x"] = pitch;
  readings["y"] = roll;
  readings["z"] = yaw;
*/
  
  readings["tp"] = temp.temperature;

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
  delay(500);
}

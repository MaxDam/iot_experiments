//from: https://randomnerdtutorials.com/esp32-mpu-6050-web-server/

//LIBS:
//adafruit mpu6050
//Adafruit Unified Sensor
//Adafruit Bus IO
//Arduino_JSON
//https://github.com/me-no-dev/ESPAsyncWebServer
//https://github.com/me-no-dev/AsyncTCP

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <L3G4200D.h>

#define LED 2

// Replace with your network credentials
//const char* ssid = "REPLACE_WITH_YOUR_SSID";
//const char* password = "REPLACE_WITH_YOUR_PASSWORD";
const char* ssid = "Vodafone-C01960075";
const char* password = "tgYsZkgHA4xhJLGy";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create an Event Source on /events
AsyncEventSource events("/events");

// Json Variable to Hold Sensor Readings
JsonObject readings;

// Timer variables
unsigned long lastTime = 0;  
unsigned long lastTimeTemperature = 0;
unsigned long lastTimeAcc = 0;
unsigned long gyroDelay = 10;
unsigned long temperatureDelay = 1000;
unsigned long accelerometerDelay = 200;

// Create a sensor object
L3G4200D gyroscope;

//sensors_event_t a, g, temp;

float gyroX, gyroY, gyroZ;

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

void initSPIFFS() {
  Serial.println("mount SPIFFS ....");
  if (!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
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

String getGyroReadings(){
  Vector norm = gyroscope.readNormalize();

  readings["gyroX"] = norm.XAxis;
  readings["gyroY"] = norm.YAxis;
  readings["gyroZ"] = norm.ZAxis;

  String jsonString = "";
  serializeJson(readings, jsonString);
  //String jsonString = JSON.stringify(readings);
  
  return jsonString;
}

String getAccReadings() {
  readings["accX"] = 0;
  readings["accY"] = 0;
  readings["accZ"] = 0;
  String accString = "";
  serializeJson(readings, accString);
  //String accString = JSON.stringify (readings);
  
  return accString;
}

String getTemperature(){
  int temperature = 0;
  return String(temperature);
}

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  initWiFi();
  initSPIFFS();
  //initL3G4200D();

  // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.serveStatic("/", SPIFFS, "/");

  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request){
    gyroX=0;
    gyroY=0;
    gyroZ=0;
    request->send(200, "text/plain", "OK");
  });

  server.on("/resetX", HTTP_GET, [](AsyncWebServerRequest *request){
    gyroX=0;
    request->send(200, "text/plain", "OK");
  });

  server.on("/resetY", HTTP_GET, [](AsyncWebServerRequest *request){
    gyroY=0;
    request->send(200, "text/plain", "OK");
  });

  server.on("/resetZ", HTTP_GET, [](AsyncWebServerRequest *request){
    gyroZ=0;
    request->send(200, "text/plain", "OK");
  });

  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);

  server.begin();
}

void loop() {
  //digitalWrite(LED, HIGH);
  
  if ((millis() - lastTime) > gyroDelay) {
    // Send Events to the Web Server with the Sensor Readings
    events.send(getGyroReadings().c_str(),"gyro_readings",millis());
    lastTime = millis();
  }
  if ((millis() - lastTimeAcc) > accelerometerDelay) {
    // Send Events to the Web Server with the Sensor Readings
    events.send(getAccReadings().c_str(),"accelerometer_readings",millis());
    lastTimeAcc = millis();
  }
  if ((millis() - lastTimeTemperature) > temperatureDelay) {
    // Send Events to the Web Server with the Sensor Readings
    events.send(getTemperature().c_str(),"temperature_reading",millis());
    lastTimeTemperature = millis();
  }
  
  //digitalWrite(LED, LOW);
  //delay(1000);
}

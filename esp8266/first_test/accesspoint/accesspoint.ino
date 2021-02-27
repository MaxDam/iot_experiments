#include <ESP8266WiFi.h>

const char *ssid = "SONO_UN_ESP8266";
const char *password = "123456";

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println('\n');

  // Start the access point
  WiFi.softAP(ssid, password);
  Serial.print("Access Point \"");
  Serial.print(ssid);
  Serial.println("\" started");

  // Send the IP address of the ESP8266 to the computer
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());
}

void loop() { }

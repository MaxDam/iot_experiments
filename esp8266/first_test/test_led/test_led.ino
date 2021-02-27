
//http://arduino.esp8266.com/stable/package_esp8266com_index.json
//ESP8266 by ESP8266 Community
//LOLIN(WEMOS) D1 mini Lite

int pin = 2;
//int pin = D2;

void setup() {
  pinMode(pin, OUTPUT);
}

void loop() {
  digitalWrite(pin, HIGH);
  delay(1000);
  digitalWrite(pin, LOW);
  delay(1000);
}

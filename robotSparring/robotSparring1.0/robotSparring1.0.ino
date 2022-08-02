//ESP-WROOM-32 38 PIN Develeopment
//http://arduino.esp8266.com/stable/package_esp8266com_index.json
//https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

#include "WiFi.h"
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>

char* ssid = "Vodafone-C01960075";
char* password = "tgYsZkgHA4xhJLGy";
WiFiClient espClient;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
#define SERVOMIN  125 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  575 // this is the 'maximum' pulse length count (out of 4096)
		
//joint
#define LEFT_ARM		0
#define LEFT_SHOULDER   1
#define RIGHT_SHOULDER	14
#define RIGHT_ARM	  	15

/*
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define NUMFLAKES     10 // Number of snowflakes in the animation example
*/

int angleToPulse(int ang){
   int pulse = map(ang, 0, 180, SERVOMIN,SERVOMAX);// map angle of 0 to 180 to Servo min and Servo max 
   Serial.print("Angle: ");Serial.print(ang);
   Serial.print(" pulse: ");Serial.println(pulse);
   return pulse;
}


//inizializza il servo driver
void initServoDriver() {
	pwm.begin();
	pwm.setPWMFreq(60);
}
/*
//inizializza il display
void initDisplay() {
	display.display();
	delay(2000);
	display.clearDisplay();
	
	display.setTextSize(1);      // Normal 1:1 pixel scale
	display.setTextColor(WHITE); // Draw white text
	display.setCursor(0, 0);     // Start at top-left corner
	display.cp437(true); 		 // Use full 256 char 'Code Page 437' font
}*/

//inizializza la WiFi
void initWiFi() {
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
  //display.write("Hello!");
  //display.write(WiFi.localIP());
  
  delay(1000);
}

void setup() {
  Serial.begin(115200);
  initServoDriver();
  //initDisplay();
  //initWiFi();
}

bool start = true;

void startPosition() {
	pwm.setPWM(RIGHT_ARM, 0, angleToPulse(0));
	pwm.setPWM(LEFT_ARM, 0, angleToPulse(0));
	pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(45));
	pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(45));
}

void straightRight() {
	pwm.setPWM(RIGHT_ARM, 0, angleToPulse(110));
	pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(90));
	pwm.setPWM(RIGHT_ARM, 0, angleToPulse(110));
	delay(600);
	pwm.setPWM(RIGHT_ARM, 0, angleToPulse(0));
	pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(45));
}

void straightLeft() {
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(110));
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(90));
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(110));
  delay(600);
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(0));
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(45));
}

void hookRight() {
	pwm.setPWM(RIGHT_ARM, 0, angleToPulse(140));
	pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(0));
	pwm.setPWM(RIGHT_ARM, 0, angleToPulse(140));
	delay(600);
	pwm.setPWM(RIGHT_ARM, 0, angleToPulse(0));
	pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(45));
}

void hookLeft() {
	pwm.setPWM(LEFT_ARM, 0, angleToPulse(140));
	pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(0));
	pwm.setPWM(LEFT_ARM, 0, angleToPulse(140));
	delay(600);
	pwm.setPWM(LEFT_ARM, 0, angleToPulse(0));
	pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(45));
}

void doubleStraight() {
  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(90));
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(90));
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(110));
  delay(600);
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(0));
  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(45));
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(110));
  delay(600);
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(0));
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(45));
}

void doubleStraightInverse() {
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(90));
  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(90));
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(110));
  delay(600);
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(0));
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(45));
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(110));
  delay(600);
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(0));
  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(45));
}

int getRandomAction() {
    int n = random(101);
    if (n < 60) // 60% straight
        return 0;
    if (n < (60 + 20)) // 20% hook (we exclude the 30% above)
        return 1;
    if (n < (60 + 20 + 20))  // 20% doubleStraight (we exclude the ones above)
        return 2;
    return 0; 
}

int getRandomSide() {
    int n = random(101);
    if (n < 50) // 50%
        return 0;
    return 1; 
}

void loop() {
	if(start) {
		startPosition();
		delay(1000);
		straightRight();
		delay(1000);
		straightLeft();
		delay(1000);
		hookRight();
		delay(1000);
		hookLeft();
		delay(2000);
		
		startPosition();
		start = false;
		delay(1000);
	}
	
	//random shot
	switch(getRandomAction()) {
    case 0: //straight
         (getRandomSide()==0 ? straightRight() : straightLeft());
         break;
    case 1: //hook
         (getRandomSide()==0 ? hookRight() : hookLeft());
         break;
    case 2: //doubleStraight
         (getRandomSide()==0 ? doubleStraight() : doubleStraightInverse());
         break;
    default:
         startPosition();
         break;
   }
   delay(1000);
}

//ESP-WROOM-32 38 PIN Develeopment
//http://arduino.esp8266.com/stable/package_esp8266com_index.json
//https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

#include "WiFi.h"
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//wifi
char* ssid = "Vodafone-C01960075";
char* password = "tgYsZkgHA4xhJLGy";
WiFiClient espClient;

//servo driver
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
#define SERVO_MIN_PULSE_WIDTH 125 // this is the 'minimum' pulse length count (out of 4096)
#define SERVO_MAX_PULSE_WIDTH 575 // this is the 'maximum' pulse length count (out of 4096)
#define SERVO_FREQUENCY 60
//#define SERVO_MIN_PULSE_WIDTH 600
//#define SERVO_MAX_PULSE_WIDTH 2600
//#define SERVO_FREQUENCY 50

//joint
#define LEFT_ARM		0
#define LEFT_SHOULDER   1
#define RIGHT_SHOULDER	14
#define RIGHT_ARM	  	15

//display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//vars
bool start = true;
bool rightShot = true;
unsigned long shotDuration = 700;
unsigned long shotPause    = 1000;

int angleToPulse(int ang){
   int pulse = map(ang, 0, 180, SERVO_MIN_PULSE_WIDTH, SERVO_MAX_PULSE_WIDTH);// map angle of 0 to 180 to Servo min and Servo max 
   Serial.print("Angle: ");Serial.print(ang);
   Serial.print(" pulse: ");Serial.println(pulse);
   return pulse;
}

/*
int pulseWidth(int angle){
   int pulse_wide, analog_value;
   pulse_wide = map(angle, 0, 180, SERVO_MIN_PULSE_WIDTH, SERVO_MAX_PULSE_WIDTH);
   analog_value = int(float(pulse_wide) / 1000000 * SERVO_FREQUENCY * 4096);
   Serial.print("Angle: ");Serial.print(angle);
   Serial.print(" pulse: ");Serial.println(pulse_wide);
   Serial.print(" analog value: ");Serial.println(analog_value);
   return analog_value;
}
*/

//inizializza il servo driver
void initServoDriver() {
	pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQUENCY);
  delay(10);
}

//inizializza il display
void initDisplay() {
	// initialize OLED display with I2C address 0x3C
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("failed to start SSD1306 OLED"));
    while (1);
  }

  delay(2000);        
  display.clearDisplay(); 

  display.setTextSize(1);         
  display.setTextColor(WHITE);    
  display.setCursor(0, 24);       
  display.println("Hello!");     
  display.display();              
}

//display string text on the center
void displayText(String text) {
  display.clearDisplay(); 
  display.setCursor(0, 24);
  display.println(text);     
  display.display(); 
}

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
  //display.write(WiFi.localIP());
  
  delay(1000);
}

void setup() {
  Serial.begin(115200);
  initServoDriver();
  initDisplay();
  //initWiFi();
}

void startPosition() {
	pwm.setPWM(RIGHT_ARM, 0, angleToPulse(0));
	pwm.setPWM(LEFT_ARM, 0, angleToPulse(0));
	pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(45));
	pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(45));
}

void straightRight() {
  displayText("straight right");
	pwm.setPWM(RIGHT_ARM, 0, angleToPulse(120));
	pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(90));
	pwm.setPWM(RIGHT_ARM, 0, angleToPulse(120));
	delay(shotDuration);
	pwm.setPWM(RIGHT_ARM, 0, angleToPulse(0));
	pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(45));
}

void straightLeft() {
  displayText("straight left");
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(110));
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(90));
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(110));
  delay(shotDuration);
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(0));
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(45));
}

void hookRight() {
  displayText("hook right");
	pwm.setPWM(RIGHT_ARM, 0, angleToPulse(140));
	pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(0));
	pwm.setPWM(RIGHT_ARM, 0, angleToPulse(140));
	delay(shotDuration);
	pwm.setPWM(RIGHT_ARM, 0, angleToPulse(0));
	pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(45));
}

void hookLeft() {
  displayText("hook left");
	pwm.setPWM(LEFT_ARM, 0, angleToPulse(120));
	pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(0));
	pwm.setPWM(LEFT_ARM, 0, angleToPulse(120));
	delay(shotDuration);
	pwm.setPWM(LEFT_ARM, 0, angleToPulse(0));
	pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(45));
}

void doubleStraightRightLeft() {
  displayText("straight right");
  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(90));
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(90));
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(120));
  delay(shotDuration);
  displayText("straight left");
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(0));
  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(45));
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(110));
  delay(shotDuration);
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(0));
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(45));
}

void doubleStraightLeftRight() {
  displayText("straight left");
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(90));
  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(90));
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(110));
  delay(shotDuration);
  displayText("straight right");
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(0));
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(45));
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(120));
  delay(shotDuration);
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(0));
  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(45));
}

void doubleStraightRightRight() {
  displayText("straight right");
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(90));
  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(90));
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(120));
  delay(shotDuration);
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(0));
  delay(shotDuration);
  displayText("straight right");
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(120));
  delay(shotDuration);
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(0));
  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(45));
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(45));
}

void doubleStraightLeftLeft() {
  displayText("straight left");
  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(90));
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(90));
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(110));
  delay(shotDuration);
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(0));
  delay(shotDuration);
  displayText("straight left");
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(110));
  delay(shotDuration);
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(0));
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(45));
  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(45));
}

void straightRightAndHookLeft() {
  displayText("straight right");
  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(90));
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(0));
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(120));
  delay(shotDuration);
  displayText("hook left");
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(0));
  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(45));
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(120));
  delay(shotDuration);
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(0));
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(45));
}

void straightLeftAndHookRight() {
  displayText("straight left");
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(90));
  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(0));
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(110));
  delay(shotDuration);
  displayText("hook right");
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(0));
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(45));
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(140));
  delay(shotDuration);
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(0));
  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(45));
}

void hookRightAndStraightLeft() {
  displayText("hook right");
  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(0));
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(90));
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(140));
  delay(shotDuration);
  displayText("straight left");
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(0));
  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(45));
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(110));
  delay(shotDuration);
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(0));
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(45));
}

void hookLeftAndStraightRight() {
  displayText("hook left");
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(0));
  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(90));
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(120));
  delay(shotDuration);
  displayText("straight right");
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(0));
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(45));
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(120));
  delay(shotDuration);
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(0));
  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(45));
}

void test() {
  /*pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(90));
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(45));
  for (int pos = 0; pos <= 110; pos += 30) {
    pwm.setPWM(RIGHT_ARM, 0, angleToPulse(pos));           
    delay(15);
  }
  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(0));
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(0));
  delay(shotPause);

   pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(90));
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(45));
  for (int pos = 0; pos <= 110; pos += 30) {
    pwm.setPWM(RIGHT_ARM, 0, angleToPulse(pos));           
    delay(15);
  }
  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(0));
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(0));
  delay(shotPause);

   pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(90));
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(45));
  for (int pos = 0; pos <= 110; pos += 30) {
    pwm.setPWM(RIGHT_ARM, 0, angleToPulse(pos));           
    delay(20);
  }
  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(0));
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(0));*/

  for (uint16_t pulselen = SERVO_MIN_PULSE_WIDTH; pulselen < SERVO_MAX_PULSE_WIDTH; pulselen++) {
    pwm.setPWM(RIGHT_ARM, 0, pulselen);
  }
  for (uint16_t pulselen = SERVO_MAX_PULSE_WIDTH; pulselen > SERVO_MIN_PULSE_WIDTH; pulselen--) {
    pwm.setPWM(RIGHT_ARM, 0, pulselen);
  }
  delay(500);
  for (uint16_t pulselen = SERVO_MIN_PULSE_WIDTH; pulselen < SERVO_MAX_PULSE_WIDTH; pulselen++) {
    pwm.setPWM(RIGHT_ARM, 0, pulselen);
  }
  for (uint16_t pulselen = SERVO_MAX_PULSE_WIDTH; pulselen > SERVO_MIN_PULSE_WIDTH; pulselen--) {
    pwm.setPWM(RIGHT_ARM, 0, pulselen);
  }
  delay(500);
  for (uint16_t pulselen = SERVO_MIN_PULSE_WIDTH; pulselen < SERVO_MAX_PULSE_WIDTH; pulselen++) {
    pwm.setPWM(RIGHT_ARM, 0, pulselen);
  }
  for (uint16_t pulselen = SERVO_MAX_PULSE_WIDTH; pulselen > SERVO_MIN_PULSE_WIDTH; pulselen--) {
    pwm.setPWM(RIGHT_ARM, 0, pulselen);
  }
  delay(500);

  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(90));
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(90));
  
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(90));
  delay(shotDuration);
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(0));
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(90));
  delay(shotDuration);
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(0));

  delay(shotPause);

  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(90));
  delay(shotDuration);
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(0));
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(90));
  delay(shotDuration);
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(0));

  delay(shotPause);

  pwm.setPWM(LEFT_ARM, 0, angleToPulse(90));
  delay(shotDuration);
  pwm.setPWM(LEFT_ARM, 0, angleToPulse(0));
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(90));
  delay(shotDuration);
  pwm.setPWM(RIGHT_ARM, 0, angleToPulse(0));
  
  pwm.setPWM(RIGHT_SHOULDER, 0, angleToPulse(0));
  pwm.setPWM(LEFT_SHOULDER, 0, angleToPulse(0));
}

//decides the action based on probability
int getRandomAction() {
    int n = random(101);
    if (n < 30) // 30% straight
        return 0;
    if (n < (30 + 10)) // 10% hook (we exclude the 30% above)
        return 1;
    if (n < (30 + 10 + 15))  // 15% doubleStraight (we exclude the ones above)
        return 2;
    if (n < (30 + 10 + 15 + 15))  // 15% doubleStraightSameSide (we exclude the ones above)
        return 3;
    if (n < (30 + 10 + 15 + 15 + 15))  // 15% straightAndHook (we exclude the ones above)
        return 4;
    if (n < (30 + 10 + 15 + 15 + 15 +15))  // 15% hookAndStraight (we exclude the ones above)
        return 5;
    return 0; 
}

//decides the side based on probability
int changeSidePropability() {
    int n = random(101);
    if (n < 80) // 80%
        return 1;
    return 0; 
}


void loop() {
	if(start) {
		startPosition();
		delay(shotPause);
    test();
    delay(shotPause);
		straightRight();
		delay(shotPause);
		straightLeft();
		delay(shotPause);
		hookRight();
		delay(shotPause);
		hookLeft();
		delay(2000);
		
		startPosition();
		start = false;
		delay(shotPause);
	}

  //change shot side based on probability
  if(changeSidePropability() == 1) {
    rightShot = !rightShot;
  }
  
	//random shot
	switch(getRandomAction()) {
    case 0: //straight
         (rightShot ? straightRight() : straightLeft());
         break;
    case 1: //hook
         (rightShot ? hookRight() : hookLeft());
         break;
    case 2: //doubleStraight
         (rightShot ? doubleStraightRightLeft() : doubleStraightLeftRight());
         break;
    case 3: //doubleStraightSameSide
         (rightShot ? doubleStraightRightRight() : doubleStraightLeftLeft());
         break;
    case 4: //straightAndHook()
         (rightShot ? straightRightAndHookLeft() : straightLeftAndHookRight());
         break;
    case 5: //hookAndStraight()
         (rightShot ? hookRightAndStraightLeft() : hookLeftAndStraightRight());
         break;
    default:
         startPosition();
         break;
   }
   delay(shotPause);
}

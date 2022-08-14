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

//servo driver calibration
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
#define SERVO_MIN_PULSE_WIDTH 125 // this is the 'minimum' pulse length count (out of 4096)
#define SERVO_MAX_PULSE_WIDTH 575 // this is the 'maximum' pulse length count (out of 4096)
#define SERVO_FREQUENCY 60
//#define SERVO_MIN_PULSE_WIDTH 600
//#define SERVO_MAX_PULSE_WIDTH 2600
//#define SERVO_FREQUENCY 50

//joint
#define LEFT_STRAIGHT		0
#define LEFT_HOOK       1
#define RIGHT_HOOK	    14
#define RIGHT_STRAIGHT  15

//display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//vars
bool start    = true;
bool southpaw = true;

//time calibration
#define SHOT_DURATION   500
#define SHOT_PAUSE      1000

//right stight calibration
#define RIGHT_STRIGHT_START_DEGREE 20
#define RIGHT_STRIGHT_END_DEGREE   90

//right hook calibration
#define RIGHT_HOOK_START_DEGREE    20
#define RIGHT_HOOK_END_DEGREE      120

//left straight calibration
#define LEFT_STRIGHT_START_DEGREE  20
#define LEFT_STRIGHT_END_DEGREE    80

//left hook calibration
#define LEFT_HOOK_START_DEGREE     20
#define LEFT_HOOK_END_DEGREE       110

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
  displayText("start");
	pwm.setPWM(RIGHT_STRAIGHT, 0, angleToPulse(RIGHT_STRIGHT_START_DEGREE));
	pwm.setPWM(LEFT_STRAIGHT, 0, angleToPulse(LEFT_STRIGHT_START_DEGREE));
	pwm.setPWM(RIGHT_HOOK, 0, angleToPulse(RIGHT_HOOK_START_DEGREE));
	pwm.setPWM(LEFT_HOOK, 0, angleToPulse(LEFT_HOOK_START_DEGREE));
}

void straightRight() {
  //displayText("straight right");
	pwm.setPWM(RIGHT_STRAIGHT, 0, angleToPulse(RIGHT_STRIGHT_END_DEGREE));
	delay(SHOT_DURATION);
	pwm.setPWM(RIGHT_STRAIGHT, 0, angleToPulse(RIGHT_STRIGHT_START_DEGREE));
}

void straightLeft() {
  //displayText("straight left");
  pwm.setPWM(LEFT_STRAIGHT, 0, angleToPulse(LEFT_STRIGHT_END_DEGREE));
  delay(SHOT_DURATION);
  pwm.setPWM(LEFT_STRAIGHT, 0, angleToPulse(LEFT_STRIGHT_START_DEGREE));
}

void hookRight() {
  //displayText("hook right");
	pwm.setPWM(RIGHT_HOOK, 0, angleToPulse(RIGHT_HOOK_END_DEGREE));
  delay(SHOT_DURATION);
  pwm.setPWM(RIGHT_HOOK, 0, angleToPulse(RIGHT_HOOK_START_DEGREE));
}

void hookLeft() {
  //displayText("hook left");
	pwm.setPWM(LEFT_HOOK, 0, angleToPulse(LEFT_HOOK_END_DEGREE));
  delay(SHOT_DURATION);
  pwm.setPWM(LEFT_HOOK, 0, angleToPulse(LEFT_HOOK_START_DEGREE));
}

void shot_1_2(bool southpaw) {
  displayText("one - two");
  if(!southpaw) {
    straightLeft();
    straightRight();
  } else {
    straightRight();
    straightLeft();
  }
}

void shot_2_3(bool southpaw) {
  displayText("two - three");
  if(!southpaw) {
    straightRight();
    hookLeft();
  } else {
    straightLeft();
    hookRight();
  }
}

void shot_1_1(bool southpaw) {
  displayText("jab - jab");
  if(!southpaw) {
    straightLeft();
    delay(SHOT_DURATION);
    straightLeft();
  } else {
    straightRight();
    delay(SHOT_DURATION);
    straightRight();
  }
}

void shot_3_2(bool southpaw) {
  displayText("three - two");
  if(!southpaw) {
    hookLeft();
    straightRight();
  } else {
    hookRight();
    straightLeft();
  }
}

void shot_1_3(bool southpaw) {
  displayText("one - three");
  if(!southpaw) {
    straightLeft();
    hookLeft();
  } else {
    straightRight();
    hookRight();
  }
}

void shot_1_2_3(bool southpaw) {
  displayText("one - two - three");
  if(!southpaw) {
    straightLeft();
    straightRight();
    hookLeft();
  } else {
    straightRight();
    straightLeft();
    hookRight();
  }
}

void shot_2_3_2(bool southpaw) {
  displayText("two - three - two");
  if(!southpaw) {
    straightRight();
    hookLeft();
    straightRight();
  } else {
    straightLeft();
    hookRight();
    straightLeft();
  }
}

void shot_1_3_1(bool southpaw) {
  displayText("one - three - one");
  if(!southpaw) {
    straightLeft();
    hookLeft();
    straightLeft();
  } else {
    straightRight();
    hookRight();
    straightRight();
  }
}

void shot_1_2_2(bool southpaw) {
  displayText("one - two - two");
  if(!southpaw) {
    straightLeft();
    straightRight();
    delay(SHOT_DURATION);
    straightRight();
  } else {
    straightRight();
    straightLeft();
    delay(SHOT_DURATION);
    straightLeft();
  }
}

void shot_1_1_2(bool southpaw) {
  displayText("one - one - two");
  if(!southpaw) {
    straightLeft();
    delay(SHOT_DURATION);
    straightLeft();
    straightRight();
  } else {
    straightRight();
    delay(SHOT_DURATION);
    straightRight();
    straightLeft();
  }
}

void shot_1_2_1(bool southpaw) {
  displayText("one - two - one");
  if(!southpaw) {
    straightLeft();
    straightRight();
    straightLeft();
  } else {
    straightRight();
    straightLeft();
    straightRight();
  }
}

void shot_1_4_3(bool southpaw) {
  displayText("one - four - three");
  if(!southpaw) {
    straightLeft();
    hookRight();
    hookLeft();
  } else {
    straightRight();
    hookLeft();
    hookRight();
  }
}

void shot_3_4_2(bool southpaw) {
  displayText("three - four - two");
  if(!southpaw) {
    hookLeft();
    hookRight();
    straightRight();
  } else {
    hookRight();
    hookLeft();
    straightLeft();
  }
}

void shot_1_2_3_4(bool southpaw) {
  displayText("one - two - three - four");
  if(!southpaw) {
    straightLeft();
    straightRight();
    hookLeft();
    hookRight();
  } else {
    straightRight();
    straightLeft();
    hookRight();
    hookLeft();
  }
}

void shot_1_2_1_2(bool southpaw) {
  displayText("one - two - one - two");
  if(!southpaw) {
    straightLeft();
    straightRight();
    straightLeft();
    straightRight();
  } else {
    straightRight();
    straightLeft();
    straightRight();
    straightLeft();
  }
}

void shot_1_2_3_2(bool southpaw) {
  displayText("one - two - three - two");
  if(!southpaw) {
    straightLeft();
    straightRight();
    hookLeft();
    straightRight();
  } else {
    straightRight();
    straightLeft();
    hookRight();
    straightLeft();
  }
}

void shot_3_4_2_1(bool southpaw) {
  displayText("three - four - two - one");
  if(!southpaw) {
    hookLeft();
    hookRight();
    straightRight();
    straightLeft();
  } else {
    hookRight();
    hookLeft();
    straightLeft();
    straightRight();
  }
}

//decides the action based on probability
int getRandomActionFromProbability(int probs[], int probSize) {
    int randomNumber = random(101);
    int threshold = 0;
    for(int i = 0 ; i < probSize ; i++) {
      threshold += probs[i];
      if(randomNumber < threshold) {
        return i;
      }
    }
    return 0;
}

void loop() {
	if(start) {
		startPosition();
		delay(SHOT_PAUSE);
		straightRight();
		delay(SHOT_PAUSE);
		straightLeft();
		delay(SHOT_PAUSE);
		hookRight();
		delay(SHOT_PAUSE);
		hookLeft();
		delay(2000);
		
		startPosition();
		start = false;
		delay(SHOT_PAUSE);
	}

  //change shot side based on probability
  int changeSideProbs[2] = {80, 20};
  if(getRandomActionFromProbability(changeSideProbs, 2) == 0) {
    southpaw = !southpaw;
  }

  //random shot
	int shotNumberProbs[4] = {20, 30, 40, 10};
  switch(getRandomActionFromProbability(shotNumberProbs, 4)) {
    case 0: { //single shot
      int singleShotProbs[2] = {70, 30};
       switch(getRandomActionFromProbability(singleShotProbs, 2)) {
          case 0: (southpaw ? straightRight() : straightLeft()); break;
          case 1: (southpaw ? hookRight()     : hookLeft());     break;
       }
       break;
    }
    case 1: { //double shot
       int doubleShotProbs[5] = {20, 20, 20, 20, 20};
       switch(getRandomActionFromProbability(doubleShotProbs, 5)) {
          case 0: shot_1_2(southpaw);  break;
          case 1: shot_2_3(southpaw);  break;
          case 2: shot_1_1(southpaw);  break;
          case 3: shot_3_2(southpaw);  break;
          case 4: shot_1_3(southpaw);  break;
       }
       break;
    }
    case 2: { //triple shot
       int tripleShotProbs[8] = {13, 13, 13, 13, 12, 12, 12, 12};
       switch(getRandomActionFromProbability(tripleShotProbs, 8)) {
          case 0: shot_1_2_3(southpaw);  break;
          case 1: shot_2_3_2(southpaw);  break;
          case 2: shot_1_3_1(southpaw);  break;
          case 3: shot_1_2_2(southpaw);  break;
          case 4: shot_1_1_2(southpaw);  break;
          case 5: shot_1_2_1(southpaw);  break;
          case 6: shot_1_4_3(southpaw);  break;
          case 7: shot_3_4_2(southpaw);  break;
       }
       break;
    }
    case 3: { //quadruple shot
       int quadrupleShotProbs[4] = {25, 25, 25, 25};
       switch(getRandomActionFromProbability(quadrupleShotProbs, 4)) {
          case 0: shot_1_2_3_4(southpaw);  break;
          case 1: shot_1_2_1_2(southpaw);  break;
          case 2: shot_1_2_3_2(southpaw);  break;
          case 3: shot_3_4_2_1(southpaw);  break;
       }
       break;
    }
    default: startPosition();
   }
   
   delay(SHOT_PAUSE);
}

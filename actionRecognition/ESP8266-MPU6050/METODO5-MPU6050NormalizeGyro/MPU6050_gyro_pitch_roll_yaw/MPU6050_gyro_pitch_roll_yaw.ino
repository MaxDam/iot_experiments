#include <LiquidCrystal.h>
LiquidCrystal lcd(8,9,10,11,12,13);
#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

// Timers
unsigned long timer = 0;
float timeStep = 0.01;

// Pitch, Roll and Yaw values
float pitch = 0;
float roll = 0;
float yaw = 0;

void setup() 
{
  lcd.begin(16,2);
  Serial.begin(9600);

  // Initialize MPU6050
  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
 
  lcd.clear();
  lcd.print("    Hi-Tech ");
  lcd.setCursor(0,1);
  lcd.print("  Electronics ");
  delay(3000);
  lcd.clear();
  lcd.print("    Gyroscope  ");
  lcd.setCursor(0,1);
  lcd.print("  Measurements");
  delay(2000);
  // Calibrate gyroscope. The calibration must be at rest.
  // If you don't want calibrate, comment this line.
  mpu.calibrateGyro();

  // Set threshold sensivty. Default 3.
  // If you don't want use threshold, comment this line or set 0.
  mpu.setThreshold(3);
}

void loop()
{
  timer = millis();
  lcd.clear();

  // Read normalized values
  Vector norm = mpu.readNormalizeGyro();

  // Calculate Pitch, Roll and Yaw
  pitch = pitch + norm.YAxis * timeStep;
  roll = roll + norm.XAxis * timeStep;
  yaw = yaw + norm.ZAxis * timeStep;

  // Output raw
  lcd.print("X =");
  lcd.print(pitch);
  lcd.print("  Y =");
  lcd.print(roll);  
  lcd.setCursor(0,1);
  lcd.print("         Z =");
  lcd.println(yaw);
  delay(250);
}

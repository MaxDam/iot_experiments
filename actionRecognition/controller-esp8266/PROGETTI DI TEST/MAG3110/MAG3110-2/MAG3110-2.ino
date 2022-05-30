#include <SparkFun_MAG3110.h>

MAG3110 mag = MAG3110();

void setup() {
  Serial.begin(9600);
  mag.initialize();
}

void loop() {

  // If we are calibrated, display heading
  if(mag.isCalibrated() ) {
    uView.setFontType(1);
    Serial.println(mag.readHeading());
    delay(100);
  } 
  else {
	Serial.println("Calibratin");
    Serial.println("Rotate me!");
    // Calibrate or enter into calibration mode
    if(mag.isCalibrating()) {
      mag.calibrate();
    } else {
      mag.enterCalMode();
    }
    delay(10);
  }
  
  
  int mX, mY, mZ;

  if(!mag.isCalibrated()) //If we're not calibrated
  {
    if(!mag.isCalibrating()) //And we're not currently calibrating
    {
      Serial.println("Entering calibration mode");
      mag.enterCalMode(); //This sets the output data rate to the highest possible and puts the mag sensor in active mode
    }
    else
    {
      //Must call every loop while calibrating to collect calibration data
      //This will automatically exit calibration
      //You can terminate calibration early by calling mag.exitCalMode();
      mag.calibrate(); 
    }
  }
  else //We are calibrated
  {
    mag.readMag(&mX, &mY, &mZ);
    float magnitude = sqrt(pow(x*mag.x_scale, 2) + pow(y*mag.y_scale, 2)); //x_scale and y_scale are set by calibration! Do not modify them!
    
	Serial.print("X: ");
    Serial.print(mX);
    Serial.print(", Y: ");
    Serial.print(mY);
    Serial.print(", Z: ");
    Serial.print(mZ);
    Serial.print(", mag: ");
    Serial.println(magnitude);
  }
  delay(20);
}
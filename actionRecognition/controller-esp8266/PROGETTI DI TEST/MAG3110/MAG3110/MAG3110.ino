#include <Wire.h>
 
#define MAG_ADDR  0x0E //7-bit address for the MAG3110, doesn't change
 
void setup()
{
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  config();            // turn the MAG3110 on
}
 
void loop()
{
  print_values();
  delay(5);
}
 
void config(void)
{
  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(0x11);             // cntrl register2
  Wire.write(0x80);             // write 0x80, enable auto resets
  Wire.endTransmission();       // stop transmitting
  delay(15); 
  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(0x10);             // cntrl register1
  Wire.write(1);                // write 0x01, active mode
  Wire.endTransmission();       // stop transmitting
}
 
void print_values(void)
{
  Serial.print("x=");
  Serial.print(read_x());
  Serial.print(",");  
  Serial.print("y=");    
  Serial.print(read_y());
  Serial.print(",");       
  Serial.print("z=");    
  Serial.println(read_z());
}
 
int mag_read_register(int reg)
{
  int reg_val;
 
  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(reg);              // x MSB reg
  Wire.endTransmission();       // stop transmitting
  delayMicroseconds(2); //needs at least 1.3us free time between start and stop
 
  Wire.requestFrom(MAG_ADDR, 1); // request 1 byte
  while(Wire.available())    // slave may write less than requested
  { 
    reg_val = Wire.read(); // read the byte
  }
 
  return reg_val;
}
 
int mag_read_value(int msb_reg, int lsb_reg)
{
  int val_low, val_high;  //define the MSB and LSB
  val_high = mag_read_register(msb_reg);
  delayMicroseconds(2); //needs at least 1.3us free time between start and stop
  val_low = mag_read_register(lsb_reg);
  int out = (val_low|(val_high << 8)); //concatenate the MSB and LSB
  return out;
}
 
int read_x(void)
{
  return mag_read_value(0x01, 0x02);
}
 
int read_y(void)
{
  return mag_read_value(0x03, 0x04);
}
 
int read_z(void)
{
  return mag_read_value(0x05, 0x06);
}
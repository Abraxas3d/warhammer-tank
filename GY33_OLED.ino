//  Read RGB from AMS TCS34725 sensor
//  Print values to Serial Monitor
//  Display sensed color to WS2812B RGB LED on pin 11
//       requires FastLED Library in Arduino IDE
//  RGB values also displayed to 128x64 OLED
//       requires SSD1306 Library in Arduino IDE
//  Wire sensor and OLED to I2C (A4/A5) in parallel

#include <Wire.h>
#include <Math.h>
#include <FastLED.h>
#include "lcdgfx.h"
#include "lcdgfx_gui.h"

DisplaySSD1306_128x64_I2C display(-1); // or (-1,{busId, addr, scl, sda, frequency})


#define NUM_LEDS 4
#define LED_PIN 11
#define WIRE_HAS_TIMEOUT
CRGB leds[NUM_LEDS];

byte i2cWriteBuffer[10];
byte i2cReadBuffer[10];

char txtout[20];
char banner[20];
char slogan0[20];
char slogan1[40];
char slogan2[20];
char slogan3[20];
char slogan4[20];
char slogan5[20];




#define SensorAddressWrite 0x29 //
#define SensorAddressRead 0x29 // 
#define EnableAddress 0xa0 // register address + command bits
#define ATimeAddress 0xa1 // register address + command bits
#define WTimeAddress 0xa3 // register address + command bits
#define ConfigAddress 0xad // register address + command bits
#define ControlAddress 0xaf // register address + command bits
#define IDAddress 0xb2 // register address + command bits
#define ColorAddress 0xb4 // register address + command bits

unsigned int clear_color = 0;
unsigned int red_color = 0;
unsigned int green_color = 0;
unsigned int blue_color = 0;

void Writei2cRegisters(byte numberbytes, byte command)
{
    byte i = 0;

    Wire.beginTransmission(SensorAddressWrite);   // Send address with Write bit set
    Wire.write(command);                          // Send command, normally the register address 
    for (i=0;i<numberbytes;i++)                       // Send data 
      Wire.write(i2cWriteBuffer[i]);
    byte error = Wire.endTransmission();
    if (error) {
      Serial.println("Error occured when writing to initialize the sensor.");
      if (error == 5)
      Serial.println("It was a timeout");
      }
      
      delayMicroseconds(100);      // allow some time for bus to settle      
}

byte Readi2cRegisters(int numberbytes, byte command)
{
   byte i = 0;

    Wire.beginTransmission(SensorAddressWrite);   // Write address of read to sensor
    Wire.write(command);
    byte error = Wire.endTransmission();
    if (error) {
      Serial.println("Error occurred when writing in Readi2cRegisters");
      if (error == 5)
      Serial.println("It was a timeout");
    }
    
    delayMicroseconds(100);      // allow some time for bus to settle      

    #if defined(WIRE_HAS_TIMEOUT)
    Wire.clearWireTimeoutFlag();
    #endif
    byte len = Wire.requestFrom(SensorAddressRead,numberbytes);   // read data
    for(i=0;i<numberbytes;i++)
      i2cReadBuffer[i] = Wire.read();
    if (len == 0) {
    Serial.println("Error occured when reading in Readi2cRegisters");
    #if defined(WIRE_HAS_TIMEOUT)
    if (Wire.getWireTimeoutFlag())
      Serial.println("It was a timeout");
    #endif
    }
    Wire.endTransmission();
    
    delayMicroseconds(100);      // allow some time for bus to settle      
}  

void init_TCS34725(void)
{
  i2cWriteBuffer[0] = 0x00;
  //i2cWriteBuffer[0] = 0xff;
  Writei2cRegisters(1,ATimeAddress);    // RGBC timing is 256 - contents x 2.4mS =  
  i2cWriteBuffer[0] = 0x00;
  Writei2cRegisters(1,ConfigAddress);   // Can be used to change the wait time
  i2cWriteBuffer[0] = 0x00;
  Writei2cRegisters(1,ControlAddress);  // RGBC gain control
  i2cWriteBuffer[0] = 0x03;
  Writei2cRegisters(1,EnableAddress);    // enable ADs and oscillator for sensor  
}

void get_TCS34725ID(void)
{
  Readi2cRegisters(1,IDAddress); 
  if (i2cReadBuffer[0] = 0x44)
    Serial.println("TCS34725 is present");    
  else
    Serial.println("TCS34725 not responding");    
}

void get_Colors(void)
{

  Readi2cRegisters(8,ColorAddress);
  clear_color = (unsigned int)(i2cReadBuffer[1]<<8) + (unsigned int)i2cReadBuffer[0];
  red_color = (unsigned int)(i2cReadBuffer[3]<<8) + (unsigned int)i2cReadBuffer[2];
  green_color = (unsigned int)(i2cReadBuffer[5]<<8) + (unsigned int)i2cReadBuffer[4];
  blue_color = (unsigned int)(i2cReadBuffer[7]<<8) + (unsigned int)i2cReadBuffer[6];
  

  sprintf(txtout, "R=%03d G=%03d B=%03d", red_color, green_color, blue_color);
  sprintf(banner, "Scanning Heretics");
  sprintf(slogan0, "Heretics crave the");
  sprintf(slogan1, "cleansing fire of    absolution.");
  sprintf(slogan2, "");
  sprintf(slogan3, "They should not fear");
  sprintf(slogan4, "for we shall give it");
  sprintf(slogan5, "to them.");

  display.clear();
  display.printFixed (0,8, txtout, STYLE_BOLD);
  display.printFixed (0, 24, banner, STYLE_ITALIC);

  //display.printFixed (0, 40, slogan0, STYLE_NORMAL);
  //display.printFixed (0, 48, slogan1, STYLE_NORMAL);
  //display.printFixed (0, 56, slogan2, STYLE_NORMAL);
  //display.printFixed (0, 48, slogan3, STYLE_ITALIC);
  //display.printFixed (0, 56, slogan5, STYLE_ITALIC);

  
  /*
  Serial.print("clear color=");
  Serial.print(clear_color, DEC);    
  Serial.print(" red color=");
  Serial.print(red_color, DEC);    
  Serial.print(" green color=");
  Serial.print(green_color, DEC);    
  Serial.print(" blue color=");
  Serial.println(blue_color, DEC);
  */
  

  leds[0] = CRGB(red_color,green_color,blue_color);
  leds[1] = CRGB(red_color,green_color,blue_color);
  leds[2] = CRGB(red_color,green_color,blue_color);
  leds[3] = CRGB(red_color,green_color,blue_color);

  FastLED.show();
  
}  

void setup() {
  display.begin();
  display.setFixedFont(ssd1306xled_font6x8);
  display.clear();
  display.drawWindow(0,0,0,0,"Heretics?",true);
  
  Wire.begin();
  #if defined(WIRE_HAS_TIMEOUT)
    Wire.setWireTimeout(3000 /* us */, true /* reset_on_timeout */);
  #endif


  Serial.begin(9600);  // start serial for output
  init_TCS34725();
  get_TCS34725ID();     // get the device ID, this is just a test to see if we're connected
  FastLED.addLeds<WS2812B, LED_PIN, RGB>(leds, NUM_LEDS);
  lcd_delay(1000);
}

void loop() {
    get_Colors();
    
    lcd_delay(1000);
}

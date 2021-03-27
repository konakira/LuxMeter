/*

  Example of BH1750 library usage.

  This example initialises the BH1750 object using the default high resolution
  one shot mode and then makes a light level reading every five seconds.

  After the measurement the MTreg value is changed according to the result:
  lux > 40000 ==> MTreg =  32
  lux < 40000 ==> MTreg =  69  (default)
  lux <    10 ==> MTreg = 138
  Remember to test your specific sensor! Maybe the MTreg value range from 32
  up to 254 is not applicable to your unit.

  Connection:

    VCC -> 3V3 or 5V
    GND -> GND
    SCL -> SCL (A5 on Arduino Uno, Leonardo, etc or 21 on Mega and Due, on esp8266 free selectable)
    SDA -> SDA (A4 on Arduino Uno, Leonardo, etc or 20 on Mega and Due, on esp8266 free selectable)
    ADD -> (not connected) or GND

  ADD pin is used to set sensor I2C address. If it has voltage greater or equal to
  0.7VCC voltage (e.g. you've connected it to VCC) the sensor address will be
  0x5C. In other case (if ADD voltage less than 0.7 * VCC) the sensor address will
  be 0x23 (by default).

*/

#if defined(ARDUINO_M5Stick_C_Plus) || defined(ARDUINO_M5Stick_C)
#include <M5StickCPlus.h>
#define M5STACK
#else // !ARDUINO_M5Stick_C_Plus
#ifdef ARDUINO_M5STACK_Core2
#include <M5Core2.h>
#define M5STACK
#endif // !ARDUINO_M5STACK_Core2
#endif // M5StickC


#include <Wire.h>
#include <BH1750.h>

#ifndef M5STACK
#include <TM1637Display.h>
TM1637Display display(CLK, DIO);
#endif

#define CLK D5
#define DIO D6

BH1750 lightMeter;

void setup(){

#ifdef M5STACK
  // initialize the M5StickC object
  M5.begin();
  delay(500);

  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  Wire.begin(0, 26);
#else
  // initialize TFT screen
  Serial.begin(115200);
  Serial.println("");

  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  Wire.begin();
#endif

  // On esp8266 you can select SCL and SDA pins using Wire.begin(D4, D3);

  lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE);
  //lightMeter.setMTreg(69);  // not needed, only mentioning it

  Serial.println(F("BH1750 Test begin"));

#ifndef M5STACK
  display.setBrightness(0x0f);

  uint8_t colon[] = {0, 0b10000000, 0, 0};
  display.setSegments(colon);
#endif
}

void loop() {
  //we use here the maxWait option due fail save
  static int prevlux = -1;
  int lux = (int)lightMeter.readLightLevel(true);
  //Serial.print(F("Light: "));
  //Serial.print(lux);
  //Serial.println(F(" lx"));

  if (prevlux != lux) {
#ifdef M5STACK
#else
    display.showNumberDec((int)lux);
#endif
    prevlux = lux;
  }

  if (lux < 0) {
    Serial.println(F("Error condition detected"));
  }
  else {
    if (lux > 40000.0) {
      // reduce measurement time - needed in direct sun light
      if (lightMeter.setMTreg(32)) {
        //Serial.println(F("Setting MTReg to low value for high light environment"));
      }
      else {
        //Serial.println(F("Error setting MTReg to low value for high light environment"));
      }
    }
    else {
        if (lux > 10.0) {
          // typical light environment
          if (lightMeter.setMTreg(69)) {
            //Serial.println(F("Setting MTReg to default value for normal light environment"));
          }
          else {
            //Serial.println(F("Error setting MTReg to default value for normal light environment"));
          }
        }
        else {
          if (lux <= 10.0) {
            //very low light environment
            if (lightMeter.setMTreg(138)) {
              //Serial.println(F("Setting MTReg to high value for low light environment"));
            }
            else {
              //Serial.println(F("Error setting MTReg to high value for low light environment"));
            }
          }
       }
    }

  }
  //Serial.println(F("--------------------------------------"));
  delay(5);
}

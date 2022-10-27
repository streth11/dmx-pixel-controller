// - - - - -
// DmxSerialNeoPixels.ino: Sample DMX application for retrieving 3 DMX values:
//
// Copyright (c) 2016 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// Documentation and samples are available at http://www.mathertel.de/Arduino
// 06.09.2016 Creation of DmxSerialNeoPixels sample.
// 27.08.2017 working with new DMXSerial DMXProbe mode.
//            cleanup.

// - - - - -
#include <Arduino.h>
#include <DMXSerial.h>
// #include "ws2812.h"
#include "re_write_ws2812.h"


const int dipPins[10]{A0,A1,A2,A3,A4,A5,5,4,3,2};
#define DIPTIME 10000
uint16_t dipValue;

uint16_t newDipValue;
long dipTimeout;
uint16_t checkDip(){
  uint16_t value=0;
  for (int pin=0;pin<10;pin++){
    value+=((!digitalRead(dipPins[pin]))<<(9-pin));
  }
  return(value);
}



#define PIXEL_PORT  PORTD  // Port of the pin the pixels are connected to
#define PIXEL_DDR   DDRD   // Port of the pin the pixels are connected to
#define PIXEL_BIT   6      // Bit of the pin the pixels are connected to
#define PIXEL2_PORT  PORTB  // Port of the pin the pixels are connected to
#define PIXEL2_DDR   DDRB   // Port of the pin the pixels are connected to
#define PIXEL2_BIT   1      // Bit of the pin the pixels are connected to

void setupNeopixel() {
  bitSet( PIXEL_DDR , PIXEL_BIT );
//  bitSet( PIXEL2_DDR , PIXEL2_BIT );
} 

//const uint8_t* pixel_port = _SFR_IO_ADDR(PORTD);
//const uint8_t* pixel_ddr = DDRD;
//const int pixel_bit = 6;

//PixelSet <_SFR_IO_ADDR(PORTD),DDRD,6> forwardsNeopixels(1);
//PixelSet <PIXEL2_PORT,PIXEL2_DDR,PIXEL2_BIT> backwardsNeopixels(0);


// Constants for demo program

#define RedDefaultLevel   5 // 100
#define GreenDefaultLevel 0 // 200
#define BlueDefaultLevel  0 // 255

// number of RGB neopixels, RGB channels are transfered
// warning: try with 12 first and scale up carefully.
#define PIXELS 50

// first DMX start address
#define DMXSTART dipValue+1 

// number of DMX channels used
#define DMXLENGTH (PIXELS*6)



// Initialize DMXSerial and neo pixel output
void setup () {
  for(int pin=0;pin<10;pin++){
    pinMode(dipPins[pin],INPUT_PULLUP);
  }   

  
  int n;
  DMXSerial.init(DMXProbe);

  // enable pwm outputs

  
  DMXSerial.maxChannel(DMXLENGTH); // after 3 * pixel channels, the onUpdate will be called when new data arrived.

  // setup the neopixel output

   setupNeopixel();
  for (int p = 0; p < PIXELS*2; p++) {
    DMXSerial.write(n++, 5);
    DMXSerial.write(n++, 10);
    DMXSerial.write(n++, 20);
  }
  updateNeopixel<_SFR_IO_ADDR(PORTD),6>(DMXSerial.getBuffer() + DMXSTART, PIXELS,1);
//  backwardsNeopixels.updateNeopixel(DMXSerial.getBuffer() + DMXSTART+150, PIXELS);

  // give them a decent color...

} // setup ()


// do constantly fetch DMX data and update the neopixels.
void loop() {
    if(millis()-dipTimeout>DIPTIME){
      newDipValue=checkDip();
      if(newDipValue<511){
        if(dipValue!=newDipValue){
          dipValue=newDipValue;
        }
      }
      dipTimeout=millis();
    }
  
  // wait for an incomming DMX packet.
  if (DMXSerial.receive()) {
    updateNeopixel<_SFR_IO_ADDR(PORTD),6>(DMXSerial.getBuffer() +DMXSTART, 50,1);
//    backwardsNeopixels.updateNeopixel(DMXSerial.getBuffer()+ DMXSTART+300, 50);

  } // if
  
} // loop()


// The End.

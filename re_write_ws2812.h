// test class declarations
//
// DOES NOT COMPILE - ISSUE HERE https://forum.arduino.cc/t/assembly-impossible-constraint-why-and-how-to-fix/564229/2
//
//
//



#define T1H  900    // Width of a 1 bit in ns
#define T1L  600    // Width of a 1 bit in ns

#define T0H  400    // Width of a 0 bit in ns
#define T0L  900    // Width of a 0 bit in ns

#define RES 6000    // Width of the low gap between bits to cause a frame to latch

// Here are some convience defines for using nanoseconds specs to generate actual CPU delays

#define NS_PER_SEC (1000000000L)          // Note that this has to be SIGNED since we want to be able to check for negative values of derivatives
#define CYCLES_PER_SEC (F_CPU)
#define NS_PER_CYCLE ( NS_PER_SEC / CYCLES_PER_SEC )
#define NS_TO_CYCLES(n) ( (n) / NS_PER_CYCLE )
#define DELAY_CYCLES(n) ( ((n)>0) ? __builtin_avr_delay_cycles( n ) :  __builtin_avr_delay_cycles( 0 ) )  // Make sure we never have a delay less than zero


// template <uint8_t* pixel_port, uint8_t* pixel_ddr, uint8_t pixel_bit> 
// class PixelSet {
//     public:
//         bool pixel_direction;
//         PixelSet(bool direction) {
//             pixel_direction = direction;
//             bitSet(pixel_ddr, pixel_bit);
//         };
// };

template <uint8_t* pixel_port, uint8_t pixel_bit> 
void updateNeopixel(uint8_t *ptr, uint8_t pixels, bool pixel_direction) {
    uint8_t  r, g, b;

    // no interrupt is welcome.
    cli();

    if (pixel_direction) {
        for (int p = 0; p < pixels; p++ ) {
            r = *ptr++;
            b = *ptr++;
            g = *ptr++;
            // send to Neopixels
            // sendPixel(r, g , b);
            sendPixel<pixel_port, pixel_bit>(r >> 2, g >> 2, b >> 2);
        } // for
    } else {
          for (int p = pixels; p >-1 ; p-- ) {
            g = *ptr--;
            b = *ptr--;
            r = *ptr--;
            // send to Neopixels
            // sendPixel(r, g , b);
            sendPixel<pixel_port, pixel_bit>(r >> 2, g >> 2, b >> 2);
        } // for
    } // if
        
    // interrupt may come.
    sei();

    // Just wait long enough without sending any bots to cause the pixels to latch and display the last sent frame
    _delay_us((RES / 1000UL) + 1);
}; // updateNeopixel()  

template <uint8_t* pixel_port, uint8_t pixel_bit> 
void sendPixel(uint8_t r, uint8_t g, uint8_t b) {
    sendByte<pixel_port, pixel_bit>(g);          // Neopixel wants colors in green then red then blue order
    sendByte<pixel_port, pixel_bit>(r);
    sendByte<pixel_port, pixel_bit>(b);
}; // sendPixel

template <uint8_t* pixel_port, uint8_t pixel_bit> 
inline void sendByte(uint8_t byte) {
  for (uint8_t bit = 0; bit < 8; bit++) {
    sendBit<pixel_port, pixel_bit>(byte & 0x80);
    byte <<= 1; // and then shift left so bit 6 moves into 7, 5 moves into 6, etc
  } // for
}; // sendByte()

template <uint8_t* pixel_port, uint8_t pixel_bit> 
inline void sendBit( bool bitVal ) {
  if (bitVal) {        // 0 bit
    asm volatile (
      "sbi %[port], %[bit] \n\t"        // Set the output bit
      ".rept %[onCycles] \n\t"          // Execute NOPs to delay exactly the specified number of cycles
      "nop \n\t"
      ".endr \n\t"
      "cbi %[port], %[bit] \n\t"        // Clear the output bit
      ".rept %[offCycles] \n\t"         // Execute NOPs to delay exactly the specified number of cycles
      "nop \n\t"
      ".endr \n\t"
      ::
      [port]    "I" (pixel_port),
      [bit]   "I" (pixel_bit),
      [onCycles]  "I" (NS_TO_CYCLES(T1H) - 2),    // 1-bit width less overhead  for the actual bit setting, note that this delay could be longer and everything would still work
      [offCycles]   "I" (NS_TO_CYCLES(T1L) - 2)     // Minimum interbit delay. Note that we probably don't need this at all since the loop overhead will be enough, but here for correctness
    );
  } else {          // 1 bit
    // **************************************************************************
    // This line is really the only tight goldilocks timing in the whole program!
    // **************************************************************************
    asm volatile (
      "sbi %[port], %[bit] \n\t"        // Set the output bit
      ".rept %[onCycles] \n\t"        // Now timing actually matters. The 0-bit must be long enough to be detected but not too long or it will be a 1-bit
      "nop \n\t"                        // Execute NOPs to delay exactly the specified number of cycles
      ".endr \n\t"
      "cbi %[port], %[bit] \n\t"        // Clear the output bit
      ".rept %[offCycles] \n\t"         // Execute NOPs to delay exactly the specified number of cycles
      "nop \n\t"
      ".endr \n\t"
      ::
      [port]    "I" (pixel_port),
      [bit]   "I" (pixel_bit),
      [onCycles]  "I" (NS_TO_CYCLES(T0H) - 2),
      [offCycles] "I" (NS_TO_CYCLES(T0L) - 2)
    );
  } // if
  // Note that the inter-bit gap can be as long as you want as long as it doesn't exceed the 5us reset timeout (which is A long time)
  // Here I have been generous and not tried to squeeze the gap tight but instead erred on the side of lots of extra time.
  // This has thenice side effect of avoid glitches on very long strings becuase
}; // sendBit()

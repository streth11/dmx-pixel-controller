
#define PIXEL_PORT  PORTD  // Port of the pin the pixels are connected to
#define PIXEL_DDR   DDRD   // Port of the pin the pixels are connected to
#define PIXEL_BIT   6      // Bit of the pin the pixels are connected to

#define PIXEL2_PORT  PORTB  // Port of the pin the pixels are connected to
#define PIXEL2_DDR   DDRB   // Port of the pin the pixels are connected to
#define PIXEL2_BIT   1      // Bit of the pin the pixels are connected to



// Actually send a bit to the string. We turn off optimizations to make sure the compile does
// not reorder things and make it so the delay happens in the wrong place.
inline void sendBit( bool bitVal )
{
  if (bitVal) {        // 0 bit
    asm volatile (
      "sbi %[port], %[bit] \n\t"        // Set the output bit
      ".rept %[onCycles] \n\t"                                // Execute NOPs to delay exactly the specified number of cycles
      "nop \n\t"
      ".endr \n\t"
      "cbi %[port], %[bit] \n\t"                              // Clear the output bit
      ".rept %[offCycles] \n\t"                               // Execute NOPs to delay exactly the specified number of cycles
      "nop \n\t"
      ".endr \n\t"
      ::
      [port]    "I" (_SFR_IO_ADDR(PIXEL_PORT)),
      [bit]   "I" (PIXEL_BIT),
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
      "nop \n\t"                                              // Execute NOPs to delay exactly the specified number of cycles
      ".endr \n\t"
      "cbi %[port], %[bit] \n\t"                              // Clear the output bit
      ".rept %[offCycles] \n\t"                               // Execute NOPs to delay exactly the specified number of cycles
      "nop \n\t"
      ".endr \n\t"
      ::
      [port]    "I" (_SFR_IO_ADDR(PIXEL_PORT)),
      [bit]   "I" (PIXEL_BIT),
      [onCycles]  "I" (NS_TO_CYCLES(T0H) - 2),
      [offCycles] "I" (NS_TO_CYCLES(T0L) - 2)
    );
  } // if
  
  // Note that the inter-bit gap can be as long as you want as long as it doesn't exceed the 5us reset timeout (which is A long time)
  // Here I have been generous and not tried to squeeze the gap tight but instead erred on the side of lots of extra time.
  // This has thenice side effect of avoid glitches on very long strings becuase
} // sendBit()

// Neopixel wants bit in highest-to-lowest order
// so send highest bit (bit #7 in an 8-bit byte since they start at 0)
inline void sendByte(uint8_t byte)
{
  for (uint8_t bit = 0; bit < 8; bit++) {
    sendBit(byte & 0x80);
    byte <<=
        1; // and then shift left so bit 6 moves into 7, 5 moves into 6, etc
  } // for
} // sendByte()

/*
  The following three functions are the public API:
  ledSetup() - set up the pin that is connected to the string. Call once at the begining of the program.
  sendPixel( r g , b ) - send a single pixel to the string. Call this once for each pixel in a frame.
  show() - show the recently sent pixel on the LEDs . Call once per frame.
*/

// Set the specified pin up as digital out

void sendPixel(uint8_t r, uint8_t g, uint8_t b)  {
  sendByte(g);          // Neopixel wants colors in green then red then blue order
  sendByte(r);
  sendByte(b);
} // sendPixel


// ----- defines and routines from josh - End -----

// read data from the DMX buffer (RGB) and send it to the neopixels...
void updateNeopixel(uint8_t *ptr, uint8_t pixels) {
  uint8_t  r, g, b;

  // no interrupt is welcome.
  cli();

  for (int p = 0; p < pixels; p++ ) {
    r = *ptr++;
    b = *ptr++;
    g = *ptr++;
    // send to Neopixels
    // sendPixel(r, g , b);
    sendPixel(r >> 2, g >> 2, b >> 2);
  } // for

  // interrupt may come.
  sei();

  // Just wait long enough without sending any bots to cause the pixels to latch and display the last sent frame
  _delay_us((RES / 1000UL) + 1);
} // updateNeopixel()

// End






















inline void sendBit2( bool bitVal )
{
  if (bitVal) {        // 0 bit
    asm volatile (
      "sbi %[port], %[bit] \n\t"        // Set the output bit
      ".rept %[onCycles] \n\t"                                // Execute NOPs to delay exactly the specified number of cycles
      "nop \n\t"
      ".endr \n\t"
      "cbi %[port], %[bit] \n\t"                              // Clear the output bit
      ".rept %[offCycles] \n\t"                               // Execute NOPs to delay exactly the specified number of cycles
      "nop \n\t"
      ".endr \n\t"
      ::
      [port]    "I" (_SFR_IO_ADDR(PIXEL2_PORT)),
      [bit]   "I" (PIXEL2_BIT),
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
      "nop \n\t"                                              // Execute NOPs to delay exactly the specified number of cycles
      ".endr \n\t"
      "cbi %[port], %[bit] \n\t"                              // Clear the output bit
      ".rept %[offCycles] \n\t"                               // Execute NOPs to delay exactly the specified number of cycles
      "nop \n\t"
      ".endr \n\t"
      ::
      [port]    "I" (_SFR_IO_ADDR(PIXEL2_PORT)),
      [bit]   "I" (PIXEL2_BIT),
      [onCycles]  "I" (NS_TO_CYCLES(T0H) - 2),
      [offCycles] "I" (NS_TO_CYCLES(T0L) - 2)
    );
  } // if
  
  // Note that the inter-bit gap can be as long as you want as long as it doesn't exceed the 5us reset timeout (which is A long time)
  // Here I have been generous and not tried to squeeze the gap tight but instead erred on the side of lots of extra time.
  // This has thenice side effect of avoid glitches on very long strings becuase
} // sendBit()

// Neopixel wants bit in highest-to-lowest order
// so send highest bit (bit #7 in an 8-bit byte since they start at 0)
inline void sendByte2(uint8_t byte)
{
  for (uint8_t bit = 0; bit < 8; bit++) {
    sendBit2(byte & 0x80);
    byte <<=
        1; // and then shift left so bit 6 moves into 7, 5 moves into 6, etc
  } // for
} // sendByte()

/*
  The following three functions are the public API:
  ledSetup() - set up the pin that is connected to the string. Call once at the begining of the program.
  sendPixel( r g , b ) - send a single pixel to the string. Call this once for each pixel in a frame.
  show() - show the recently sent pixel on the LEDs . Call once per frame.
*/

// Set the specified pin up as digital out

void sendPixel2(uint8_t r, uint8_t g, uint8_t b)  {
  sendByte2(g);          // Neopixel wants colors in green then red then blue order
  sendByte2(r);
  sendByte2(b);
} // sendPixel


// ----- defines and routines from josh - End -----

// setupNeopixel()


// read data from the DMX buffer (RGB) and send it to the neopixels...
void updateNeopixel2(uint8_t *ptr, uint8_t pixels) {
  uint8_t  r, g, b;

  // no interrupt is welcome.
  cli();

  for (int p = pixels; p >-1 ; p-- ) {
    g = *ptr--;
    b = *ptr--;
    r = *ptr--;
    // send to Neopixels
    // sendPixel(r, g , b);
    sendPixel2(r >> 2, g >> 2, b >> 2);
  } // for

  // interrupt may come.
  sei();

  // Just wait long enough without sending any bots to cause the pixels to latch and display the last sent frame
  _delay_us((RES / 1000UL) + 1);
} // updateNeopixel()

// End

/*
NeoPixel Ring goggles sketch -- for steampunk, rave or Burning Man fashion!
Welding or costume goggles using 50mm round lenses can be outfitted with
a pair of Adafruit NeoPixel Rings: http://www.adafruit.com/product/1463

Please exercise common sense.  These goggles emit a LOT of stray light and
should NOT BE WORN ON YOUR EYES.  They're for fashion and costuming only,
for display on a hat or on your forehead.

Draws a spinning rainbow on both eyepieces.  Not a Mac beachball, honest.
"Eyes" glance around and blink at random.

For 'reflected' colors (rainbows rotate in opposite directions, while eyes
look in same direction), connect the output of the first ring to the input
of the second.  Or you can connect the inputs of both rings to the same
Arduino pin if that's easier -- the rainbows will both twirl in the same
direction in that case.

By default, pixel #0 (the first LED) on both rings should be at the TOP of
the goggles.  Looking at the BACK of the board, pixel #0 is immediately
clockwise from the OUT connection.  If a different pixel is at the top,
that's OK, the code can compensate (TOP_LED_FIRST and TOP_LED_SECOND below).

IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
and minimize distance between Arduino and first pixel.  Avoid connecting
on a live circuit...if you must, connect GND first.
*/

#include <Adafruit_NeoPixel.h>

#define OUTPIN 12
#define INPIN 11
#define DEBOUNCELIMIT 100
#define NUMPIXELS 4

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, OUTPIN, NEO_GRB + NEO_KHZ800);

const uint16_t SinuCycle[] PROGMEM = {
    0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1,   1,   2,   2,   2,
    3,   3,   4,   4,   4,   4,   5,   5,   6,   7,   7,   8,   8,   8,   9,  10,
   11,  11,  12,  13,  13,  15,  15,  17,  19,  20,  22,  24,  28,  31,  34,  38,
   43,  48,  54,  60,  66,  73,  83,  91,  99, 108, 118, 130, 141, 153, 165, 177,
  194, 208, 222, 238, 254, 275, 292, 311, 330, 350, 371, 398, 420, 443, 468, 493,
  519, 546, 566, 595, 617, 639, 662, 678, 694, 718, 726, 743, 751, 768, 777, 794,
  803, 821, 830, 839, 848, 867, 876, 885, 895, 904, 914, 924, 933, 943, 953, 963,
  963, 973, 983, 983, 993, 993,1003,1003,1014,1014,1014,1023,1023,1023,1023,1023,
 1023,1023,1023,1023,1023,1014,1014,1014,1003,1003, 993, 993, 983, 983, 973, 963,
  963, 953, 943, 933, 924, 914, 904, 895, 885, 876, 867, 848, 839, 830, 821, 803,
  794, 777, 768, 751, 743, 726, 718, 694, 678, 662, 639, 617, 595, 566, 546, 519,
  493, 468, 443, 420, 398, 371, 350, 330, 311, 292, 275, 254, 238, 222, 208, 194,
  177, 165, 153, 141, 130, 118, 108,  99,  91,  83,  73,  66,  60,  54,  48,  43,
   38,  34,  31,  28,  24,  22,  20,  19,  17,  15,  15,  13,  13,  12,  11,  11,
   10,   9,   8,   8,   8,   7,   7,   6,   5,   5,   4,   4,   4,   4,   3,   3,
    2,   2,   2,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0
};

void setup() {
  pinMode(INPIN, INPUT);
  digitalWrite(INPIN, HIGH);
  randomSeed(analogRead(A0));
  pixels.begin();
  pixels.show();
}

void loop() {
  static uint8_t mode = 4;
  static unsigned int debouncer = 0;
  if(!digitalRead(INPIN)){
    if(debouncer==DEBOUNCELIMIT){
      mode++;
      debouncer++;
    }
    else debouncer++;
  }
  else debouncer = 0;
  
  switch(mode%5){
    case 0:
      spinning_rainbow(10*1000/256);
      break;
    case 1:
      solid_color_breathe(42, 25);
      break;
    case 2:
      solid_color_breathe(116, 25);
      break;
    case 3:
      solid_color_breathe(221, 25);
      break;
    case 4:
      candle_flicker(random(50,200));
      break;
    default:
      for(uint8_t n = 0; n<NUMPIXELS; n++) pixels.setPixelColor(n,20,20,20);
      break;
  }
  
  pixels.show();

}

void spinning_rainbow(uint8_t framelength){
  static uint8_t cyclepos = 0;
  static uint8_t ditherlev = 0;
  static unsigned long int periodstart=millis();
  
  uint8_t pixelOffsets[] = {0, 64, 128, 192};
  uint8_t colorOffsets[] = {0, 85, 170};
  
  uint16_t r,g,b;
  
  for(uint8_t n = 0; n<NUMPIXELS; n++){
    r=pgm_read_word(&SinuCycle[(cyclepos+pixelOffsets[n]+colorOffsets[0])%256]);
    g=pgm_read_word(&SinuCycle[(cyclepos+pixelOffsets[n]+colorOffsets[1])%256]);
    b=pgm_read_word(&SinuCycle[(cyclepos+pixelOffsets[n]+colorOffsets[2])%256]);
    r=r/4+((ditherlev<(r%4))?1:0);
    g=g/4+((ditherlev<(g%4))?1:0);
    b=b/4+((ditherlev<(b%4))?1:0);
    if(r>255)r=255;
    if(g>255)g=255;
    if(b>255)b=255;
    pixels.setPixelColor(n,(uint8_t)r,(uint8_t)g,(uint8_t)b);
  }
  
  if (ditherlev == 3) ditherlev = 0;
  else ditherlev++;
  
  if(millis() > periodstart+framelength){
    cyclepos++;
    periodstart=millis();
  }
}

void candle_flicker(uint8_t framelength){
  static int level = 1000;
  static int fuel = 1000;
  static int oxygen = random(3,7);
  static unsigned long int periodstart=millis();
  
  if(millis() > periodstart+framelength){
    oxygen = random(3,7);
    periodstart = millis();
  }
  
  level += (fuel*oxygen-level/2)/4000;
  level -= 1;
  fuel -= level/3000;
  fuel += 1;
  
  if(level<400)level=400;
  if(level>(127*120))level=(127*120);
  
  for(uint8_t n = 0; n<NUMPIXELS; n++){
    int bright = level;
    if(n==2) bright+=(oxygen-5)*200;
    else if(n==0) bright-=(oxygen-5)*200;
    else if(n==1) bright+=(abs(oxygen-5)*200 - 400);
    pixels.setPixelColor(n,bright/120+3,bright/160,bright/400-((bright<800)?0:2));
  }
}

void solid_color_breathe(unsigned int hue, unsigned int framelength){
  static uint8_t cyclepos = 0;
  static uint8_t ditherlev = 0;
  static unsigned long int periodstart=millis();
  
  uint8_t pixelOffsets[] = {10, 60, 50, 0};

  unsigned long R,G,B;
  uint16_t r,g,b;
  R = pgm_read_word(&SinuCycle[(hue+128)%256]);
  G = pgm_read_word(&SinuCycle[(hue+42)%256]);
  B = pgm_read_word(&SinuCycle[(hue+212)%256]);
  
  for(uint8_t n = 0; n<NUMPIXELS; n++){
    r=pgm_read_word(&SinuCycle[(cyclepos+pixelOffsets[n])%256]);
    g=pgm_read_word(&SinuCycle[(cyclepos+pixelOffsets[n])%256]);
    b=pgm_read_word(&SinuCycle[(cyclepos+pixelOffsets[n])%256]);
    R=R*(683+(r+1)/3)/1024;//+((ditherlev<(r%4))?1:0);
    G=G*(683+(g+1)/3)/1024;//+((ditherlev<(g%4))?1:0);
    B=B*(683+(b+1)/3)/1024;//+((ditherlev<(b%4))?1:0);
    if(R>255)R=255;
    if(G>255)G=255;
    if(B>255)B=255;
    pixels.setPixelColor(n,(uint8_t)R,(uint8_t)G,(uint8_t)B);
  }
  
  if (ditherlev == 3) ditherlev = 0;
  else ditherlev++;
  
  if(millis() > periodstart+framelength){
    cyclepos++;
    periodstart=millis();
  }
}

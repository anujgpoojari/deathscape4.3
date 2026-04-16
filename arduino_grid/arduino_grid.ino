#include <FastLED.h>

#define LED_PIN_1     11 // Vertical Strip
#define LED_PIN_2     10 // Horizontal Strip
#define NUM_LEDS      600
#define BRIGHTNESS    100

CRGB leds1[NUM_LEDS];
CRGB leds2[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812, LED_PIN_1, GRB>(leds1, NUM_LEDS);
  FastLED.addLeds<WS2812, LED_PIN_2, GRB>(leds2, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  
  FastLED.clear();
  FastLED.show();
  
  Serial.begin(9600);
}

// -------------------------------------------------------------
// MANUAL HARDWARE MAPPERS
// -------------------------------------------------------------
void setdaV(CRGB color, int startPos, int endPos) {
  if (startPos < 0) startPos = 0;
  if (endPos >= NUM_LEDS) endPos = NUM_LEDS - 1;
  for (int i = startPos; i <= endPos; i++) {
    leds1[i] = color;
  }
}

void setdaH(CRGB color, int startPos, int endPos) {
  if (startPos < 0) startPos = 0;
  if (endPos >= NUM_LEDS) endPos = NUM_LEDS - 1;
  for (int i = startPos; i <= endPos; i++) {
    leds2[i] = color;
  }
}

void setCellColor(int x, int y, CRGB c) {
  // Coordinates are Cartesian:
  // x = 0 is far-left.
  // y = 0 is the absolute bottom row climbing up to y = 5.
  
  // ==========================================
  // ROW 0 : BOTTOM ROW
  // ==========================================
  if (y == 0) {
    if (x == 0)      { setdaV(c,0,12);      setdaV(c,145,157);   setdaH(c,0,11);      setdaH(c,146,158); }   // Cell 1 
    else if (x == 1) { setdaV(c,13,24);     setdaV(c,134,145);   setdaH(c,13,23);     setdaH(c,134,146); }   // Cell 2 
    else if (x == 2) { setdaV(c,171,182);   setdaV(c,317,328);   setdaH(c,24,35);     setdaH(c,122,133); }   // Cell 3 
    else if (x == 3) { setdaV(c,317,329);   setdaV(c,341,352);   setdaH(c,36,47);     setdaH(c,110,121); }   // Cell 4 
    else if (x == 4) { setdaV(c,341,352);   setdaV(c,488,500);   setdaH(c,49,60);     setdaH(c,98,109); }    // Cell 5 
    else if (x == 5) { setdaV(c,488,500);   setdaV(c,513,524);   setdaH(c,62,73);     setdaH(c,87,98); }     // Cell 6 
  }
  // ==========================================
  // ROW 1 : 2nd ROW FROM BOTTOM
  // ==========================================
  else if (y == 1) {
    if (x == 0)      { setdaV(c,13,24);     setdaV(c,134,145);   setdaH(c,146,159);   setdaH(c,172,183); }   // Cell 7 
    else if (x == 1) { setdaV(c,134,145);   setdaV(c,183,194);   setdaH(c,135,146);   setdaH(c,184,195); }   // Cell 8 
    else if (x == 2) { setdaV(c,183,194);   setdaV(c,305,316);   setdaH(c,122,134);   setdaH(c,196,207); }   // Cell 9 
    else if (x == 3) { setdaV(c,305,316);   setdaV(c,353,364);   setdaH(c,110,121);   setdaH(c,208,219); }   // Cell 10
    else if (x == 4) { setdaV(c,353,364);   setdaV(c,476,487);   setdaH(c,98,109);    setdaH(c,220,231); }   // Cell 11
    else if (x == 5) { setdaV(c,475,487);   setdaV(c,525,536);   setdaH(c,86,97);     setdaH(c,232,244); }   // Cell 12
  }
  // ==========================================
  // ROW 2 : 3rd ROW FROM BOTTOM
  // ==========================================
  else if (y == 2) {
    if (x == 0)      { setdaV(c,24,36);     setdaV(c,122,133);   setdaH(c,172,183);   setdaH(c,318,330); }   // Cell 13
    else if (x == 1) { setdaV(c,122,133);   setdaV(c,195,206);   setdaH(c,184,195);   setdaH(c,306,317); }   // Cell 14
    else if (x == 2) { setdaV(c,194,206);   setdaV(c,292,304);   setdaH(c,196,207);   setdaH(c,294,305); }   // Cell 15
    else if (x == 3) { setdaV(c,0,0);       setdaV(c,0,0);       setdaH(c,0,0);       setdaH(c,0,0); }       // Cell 16  
    else if (x == 4) { setdaV(c,0,0);       setdaV(c,0,0);       setdaH(c,0,0);       setdaH(c,0,0); }       // Cell 17  
    else if (x == 5) { setdaV(c,0,0);       setdaV(c,0,0);       setdaH(c,0,0);       setdaH(c,0,0); }       // Cell 18  
  }
  // ==========================================
  // ROW 3 : 4th ROW FROM BOTTOM
  // ==========================================
  else if (y == 3) {
    if (x == 0)      { setdaV(c,0,0);       setdaV(c,0,0);       setdaH(c,0,0);       setdaH(c,0,0); }       // Cell 19  
    else if (x == 1) { setdaV(c,109,121);   setdaV(c,207,219);   setdaH(c,306,317);   setdaH(c,355,367); }   // Cell 20
    else if (x == 2) { setdaV(c,207,219);   setdaV(c,280,291);   setdaH(c,294,305);   setdaH(c,368,379); }   // Cell 21
    else if (x == 3) { setdaV(c,280,291);   setdaV(c,378,389);   setdaH(c,282,293);   setdaH(c,380,391); }   // Cell 22
    else if (x == 4) { setdaV(c,377,389);   setdaV(c,451,463);   setdaH(c,270,281);   setdaH(c,392,403); }   // Cell 23
    else if (x == 5) { setdaV(c,451,462);   setdaV(c,549,560);   setdaH(c,258,269);   setdaH(c,404,415); }   // Cell 24
  }
  // ==========================================
  // ROW 4 : 5th ROW FROM BOTTOM
  // ==========================================
  else if (y == 4) {
    if (x == 0)      { setdaV(c,0,0);       setdaV(c,0,0);       setdaH(c,0,0);       setdaH(c,0,0); }       // Cell 25  
    else if (x == 1) { setdaV(c,98,108);    setdaV(c,219,231);   setdaH(c,355,367);   setdaH(c,477,488); }   // Cell 26
    else if (x == 2) { setdaV(c,219,231);   setdaV(c,267,279);   setdaH(c,368,379);   setdaH(c,465,476); }   // Cell 27
    else if (x == 3) { setdaV(c,268,279);   setdaV(c,390,401);   setdaH(c,380,391);   setdaH(c,453,464); }   // Cell 28
    else if (x == 4) { setdaV(c,390,401);   setdaV(c,439,450);   setdaH(c,392,403);   setdaH(c,441,452); }   // Cell 29
    else if (x == 5) { setdaV(c,439,450);   setdaV(c,562,572);   setdaH(c,404,415);   setdaH(c,429,440); }   // Cell 30
  }
  // ==========================================
  // ROW 5 : TOP ROW
  // ==========================================
  else if (y == 5) {
    if (x == 0)      { setdaV(c,0,0);       setdaV(c,0,0);       setdaH(c,0,0);       setdaH(c,0,0); }       // Cell 31  
    else if (x == 1) { setdaV(c,85,97);     setdaV(c,231,243);   setdaH(c,477,488);   setdaH(c,527,538); }   // Cell 32
    else if (x == 2) { setdaV(c,231,243);   setdaV(c,256,267);   setdaH(c,465,476);   setdaH(c,539,550); }   // Cell 33
    else if (x == 3) { setdaV(c,255,267);   setdaV(c,402,414);   setdaH(c,453,464);   setdaH(c,551,562); }   // Cell 34
    else if (x == 4) { setdaV(c,402,414);   setdaV(c,427,438);   setdaH(c,441,452);   setdaH(c,563,575); }   // Cell 35
    else if (x == 5) { setdaV(c,427,438);   setdaV(c,573,584);   setdaH(c,429,440);   setdaH(c,576,587); }   // Cell 36
  }
}

void loop() {
  if (Serial.available() > 0) {
    String payload = Serial.readStringUntil('\n');
    payload.trim(); 

    if(payload.length() == 0) return;

    char cmd = payload.charAt(0);
    
    // System-wide structural commands
    if (cmd == 'R') {
      FastLED.clear();
      FastLED.show();
      return;
    } 
    else if (cmd == 'D') {
      fill_solid(leds1, NUM_LEDS, CRGB::Green);
      fill_solid(leds2, NUM_LEDS, CRGB::Green);
      FastLED.show();
      return;
    }

    // Active path tracing commands parsing structures from SFML (e.g. "C 2 5")
    if (cmd == 'C' || cmd == 'W' || cmd == 'U') {
      int space1 = payload.indexOf(' ');
      if (space1 == -1) return; 
      int space2 = payload.indexOf(' ', space1 + 1);
      if (space2 == -1) return;

      int graphic_x = payload.substring(space1 + 1, space2).toInt();
      int graphic_y = payload.substring(space2 + 1).toInt();
      
      if(graphic_x >= 0 && graphic_x < 6 && graphic_y >= 0 && graphic_y < 6) {
        CRGB targetColor = CRGB::Black;
        
        if (cmd == 'C') targetColor = CRGB::Green;
        else if (cmd == 'W') targetColor = CRGB::Red;
        else if (cmd == 'U') targetColor = CRGB::Black; 
        
        // --- CARTESIAN CONVERSION ---
        // C++ Windows graphics strictly render Y=0 as Top natively.
        // We explicitly invert it here on the Arduino side (5 - y) so that your 
        // local file definitions remain grounded with Y=0 operating physically as the VERY BOTTOM!
        int physical_y = 5 - graphic_y;
        
        setCellColor(graphic_x, physical_y, targetColor);
        FastLED.show();
      }
    }
  }
}

#include <Adafruit_NeoPixel.h>

// You can change this pin based on your exact UNO/MEGA wiring
#define PIN        6 
// Assuming a mapped 6x6 grid size exactly matching the software
#define NUMPIXELS 36 

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // Sync baud rate exactly with the C++ CreateFile logic
  Serial.begin(9600);
  
  pixels.begin();
  pixels.clear();
  pixels.show(); // Initialize all pixels to 'off'
}

void loop() {
  if (Serial.available() > 0) {
    // Rely on newline terminating strings pushed by the C++ engine \n
    String payload = Serial.readStringUntil('\n');
    payload.trim(); 

    if(payload.length() == 0) return;

    // Grab the first letter signature flag
    char cmd = payload.charAt(0);
    
    // Commands without payload coordinates
    if (cmd == 'R') {
      pixels.clear();
      pixels.show();
      return;
    } 
    else if (cmd == 'D') {
      // Victory: Flash everything green
      for(int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 255, 0));
      }
      pixels.show();
      return;
    }

    // Dynamic Commands with payloads (Requires parsing X and Y spaces)
    // Structure: "C 3 2" -> cmd=C, x=3, y=2
    if (cmd == 'C' || cmd == 'W' || cmd == 'U') {
      int space1 = payload.indexOf(' ');
      if (space1 == -1) return; // Invalid formatting
      int space2 = payload.indexOf(' ', space1 + 1);
      if (space2 == -1) return;

      int x = payload.substring(space1 + 1, space2).toInt();
      int y = payload.substring(space2 + 1).toInt();
      
      // Calculate 1D index from 2D coordinates
      // Y is the row from the top, X is column index
      int index = y * 6 + x;
      
      // Bounds protection
      if(index >= 0 && index < NUMPIXELS) {
        if (cmd == 'C') {
          // Standard green tracking step
          pixels.setPixelColor(index, pixels.Color(0, 255, 0));
        } else if (cmd == 'W') {
          // Failure red lockout step
          pixels.setPixelColor(index, pixels.Color(255, 0, 0));
        } else if (cmd == 'U') {
          // Undo a step mapping it back to empty (0,0,0)
          pixels.setPixelColor(index, pixels.Color(0, 0, 0));
        }
        pixels.show();
      }
    }
  }
}

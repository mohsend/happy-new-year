/*
 * These functions help you use a 4-digit sevensegment 
 * with a couple of 74595 serial-to-prallel daisy-chained together
 * to display ASCII strings
 */

#ifndef SEVENSEGMENT_DISPLAY_H
#define SEVENSEGMENT_DISPLAY_H 1

#include <Arduino.h>


// Pin connected to latch pin (ST_CP) of 74HC595
int ss_latchPin;
// Pin connected to clock pin (SH_CP) of 74HC595
int ss_clockPin;
// Pin connected to Data in (DS) of 74HC595
int ss_dataPin;
// set to true if your seven segment in common anode and to false
// if it's common cathode
byte ss_anode;

void setupDisplay(int l, int c, int d, bool a = false);
inline void writePattern(byte pattern, byte place);
inline void writeChar(byte place, char ch);
inline void clearDisplay();
inline void displayASCII(char ch, byte place);
void updateDisplay();
inline void displayString(char* str);
void displayTicker(char* str, int sp = 200, int st = 1);

// each byte is a seven segment digit pattern
// this array is a segment of ASCII encoding.
// from 0x20 to 0x60
const byte ss_font[] = { 
  0x00, //  
  0x92, // !
  0x92, // "
  0x92, // #
  0x92, // $
  0x4a, // %
  0x92, // &
  0x04, // '
  0x9c, // (
  0xf0, // )
  0xc6, // *
  0x62, // +
  0x30, // ,
  0x02, // -
  0x01, // .
  0x4a, // /
  0xfc, // 0
  0x60, // 1
  0xda, // 2
  0xf2, // 3
  0x66, // 4
  0xb6, // 5
  0xbe, // 6
  0xe0, // 7
  0xfe, // 8
  0xf6, // 9
  0x12, // :
  0x12, // ;
  0x86, // <
  0x12, // =
  0xc2, // >
  0xca, // ?
  0x92, // @
  0xee, // A
  0x3e, // B
  0x1a, // C
  0x7a, // D
  0x9e, // E
  0x8e, // F
  0xf6, // G
  0x6e, // H
  0x0c, // I
  0x7c, // J
  0x0e, // K
  0x1c, // L
  0x2a, // M
  0x2a, // N
  0xfc, // O
  0xce, // P
  0xe6, // Q
  0x0a, // R
  0xb6, // S
  0x1e, // T
  0x38, // U
  0x38, // V
  0x38, // W
  0x6e, // X
  0x76, // Y
  0xda, // Z
  0x9c, // [
  0x26, // \
  0xf0, // ]
  0xc4, // ^
  0x80, // _ 
  0x04 // `
  };

// assign the byte pattern to each digit on 7segment to be displayed
byte ss_digit[4];

void setupDisplay(int l, int c, int d, bool a) {
  // set global variables
  // Pin connected to latch pin (ST_CP) of 74HC595
  ss_latchPin = l;
  // Pin connected to clock pin (SH_CP) of 74HC595
  ss_clockPin = c;
  // Pin connected to Data in (DS) of 74HC595
  ss_dataPin = d;
  // set to true if your seven segment in common anode and to false
  // if it's common cathode
  ss_anode = a ? 0xff : 0x00;
  
  // set pin modes to output
  pinMode(ss_latchPin, OUTPUT);
  pinMode(ss_dataPin, OUTPUT);  
  pinMode(ss_clockPin, OUTPUT);
  
  // attach updateDisplay function to timer1 interrupt
  Timer1.initialize();
  Timer1.attachInterrupt(updateDisplay, 4000);
  // clear display if there's leftover data in shift registers
  clearDisplay();
}

inline void writePattern(byte pattern, byte place) {
  // shift the first byte that determines the digit should light up
  shiftOut(ss_dataPin, ss_clockPin, LSBFIRST, (ss_anode ^ 0x7f) >> place);
  // shift the next byte with the pattern
  shiftOut(ss_dataPin, ss_clockPin, LSBFIRST, pattern);
}

inline void writeChar(byte place, char ch) {
  // set latch to low, data won't show while shifting
  digitalWrite(ss_latchPin, LOW);
  // shift the font pattern to 'place' digit
  writePattern(ss_anode ^ ss_font[ch], place);
  // set latch to high, show new data
  digitalWrite(ss_latchPin, HIGH);
}


inline void clearDisplay() {
  for (byte i = 0; i < 4; i++)
    ss_digit[i] = 0x00;
}

inline void displayASCII(char ch, byte place) {
  if (ch >= 0x20 && ch <= 0x60) {
    // map ASCII charachters to the font array
    ss_digit[place] = ch - 0x20;
  } else if (ch > 0x60 && ch <= 0x7A) {
    // reuse lowercase characters for uppercase
    ss_digit[place] = ch - 0x40;
  } else {
    // if the charecter is not in the array display nothing
    ss_digit[place] = 0x00;
  }
}

// this functions gets called by time interrupt
void updateDisplay() {
  // show pattern of a digit each time the function runs
  static byte placeNow = 0;
  writeChar(placeNow, ss_digit[placeNow]);
  // add 1 to placeNow and keep the lower 2 bits
  // this assigns 0, 1, 2, 3 to placeNow each time
  placeNow = (placeNow + 1) & 0x03;
}

inline void displayString(char* str) {
  for (int i = 3; i >= 0; i--)
    displayASCII(str[-(i-3)], i);
}

void displayTicker(char* str, int sp, int st) {
  for (int index = 0; str[index + 3]; index = index + st) {
    displayString(str + index);
    // wait before showing the next 4 characters
    delay(sp);
  }
}

#endif

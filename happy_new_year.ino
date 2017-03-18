#include <TimerOne.h>
#include "display.h"

void setup() {
  // setup Display with shift register pins: 
  // latch: 10
  // clock: 12
  // data: 11
  // and seven the seven segment is common anode (true)
  setupDisplay(10, 12, 11, true);
}

void loop() {
  displayTicker("      happy    new,    year      ");
  displayString("1396");
  delay(2000);
  displayString(" :) ");
  delay(1000);
}


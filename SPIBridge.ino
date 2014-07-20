/*
   USB to SPI bridge
   Copyright (C) Paul Brook <paul@nowt.org>
   Released under the terms of the GNU General Public License version 3
 */
 
#include <SPI.h>

static const uint8_t cs_pin = 10;
static const uint8_t led_pin = SS;

#define NUM_BUTTONS 10
static bool button_state[NUM_BUTTONS];

void setup()
{
  int i;
  Serial.begin(9600);
  pinMode(cs_pin, OUTPUT);
  digitalWrite(cs_pin, 1);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.setDataMode(SPI_MODE3);
  SPI.begin();
  for (i = 0; i < NUM_BUTTONS; i++) {
      pinMode(i, INPUT_PULLUP);
  }
  Keyboard.begin();
}

static void
check_buttons(void)
{
  int i;
  static long next_check;
  long now;
  bool new_state;
  now = millis();
  if (next_check) {
      if (now - next_check < 0)
	return;
      next_check = 0;
  }
  for (i = 0; i < NUM_BUTTONS; i++) {
      new_state = (digitalRead(i) == 0);
      if (new_state != button_state[i]) {
	  button_state[i] = new_state;
	  if (new_state) {
	      Keyboard.press('0' + i);
	  } else {
	      Keyboard.release('0' + i);
	  }
	  next_check = now + 100;
      }
  }
}

void loop()
{
  int c;
  bool new_connection = true;
  SPCR &= ~_BV(SPIE);
  // Access SPSR followed by SPDR to clear SPIF
  SPSR;
  SPDR = 0xff;
  while ((SPSR & _BV(SPIF)) == 0)
    /* no-op */;
  while (true) {
      while (!Serial.available())
	check_buttons();
      if (new_connection) {
         Serial.write(0xfe);
         Serial.write('L');
         Serial.write('E');
         Serial.write('D');
         new_connection = false;
      }
      digitalWrite(cs_pin, 0);
      TXLED1;
      while (true) {
	  c = Serial.read();
	  // The Arduino SPI code waits for the transmit to complete before returning.
	  // Poke data directly at the SPI hardware so that we can be reading more USB
	  // data while that is happening
	  while ((SPSR & _BV(SPIF)) == 0)
	    /* no-op */;
	  if (c == -1)
	    break;
	  SPDR = c;
      }
      TXLED0;
      digitalWrite(cs_pin, 1);
      if (!Serial)
        new_connection = true;
      delayMicroseconds(10);
  }
}

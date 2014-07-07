/*
   USB to SPI bridge
   Copyright (C) Paul Brook <paul@nowt.org>
   Released under the terms of the GNU General Public License version 3
 */
 
#include <SPI.h>

static const uint8_t cs_pin = 10;

void setup()
{
  Serial.begin(9600);
  pinMode(cs_pin, OUTPUT);
  digitalWrite(cs_pin, 1);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPI.setDataMode(SPI_MODE3);
  SPI.begin();
}

void loop()
{
  int c;
  SPCR &= ~_BV(SPIE);
  // Clear SPIF
  SPSR = SPSR;
  SPDR = 0xff;
  while (true) {
      digitalWrite(cs_pin, 0);
      while (Serial.available()) {
	  c = Serial.read();
	  // The Arduino SPI code waits for the transmit to complete before returning.
	  // Poke data directly at the SPI hardware so that we can be reading more USB
	  // data while that is happening
	  while ((SPSR & _BV(SPIF)) == 0)
	    /* no-op */;
	  SPDR = c;
      }
      digitalWrite(cs_pin, 1);
  }
}

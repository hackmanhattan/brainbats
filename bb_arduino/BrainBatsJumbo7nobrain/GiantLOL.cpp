/*
  GiantLOL.h - Using timer2 with 1ms resolution
  
  Alex Wenger <a.wenger@gmx.de> http://arduinobuch.wordpress.com/
  
  Timer init code from MsTimer2 - Javier Valencia <javiervalencia80@gmail.com>
  Misc functions from Benjamin Sonnatg <benjamin@sonntag.fr>
  
  History:
    2009-12-30 - V0.0 wrote the first version at 26C3/Berlin
    2010-01-01 - V0.1 adding misc utilitary functions 
      (Clear, Vertical,  Horizontal) comment are doxygen compliants now

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#define DEBUG 0

#include "Arduino.h"
#include <inttypes.h>
#include <avr/interrupt.h>
#include "GiantLOL.h"


volatile unsigned int LedSign::tcnt2;
LedSign *LedSign::_inst = NULL;

//uint8_t clock_pins[ALL_COLUMNS] = {22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48};
//uint8_t data_pins[ALL_ROWS] = {23, 25, 27, 29, 31, 33, 39, 41, 43};

uint8_t clock_pins[ALL_COLUMNS] = {52, 50, 48, 46, 44, 42, 40, 38, 36, 34, 32, 30, 28, 26, 24, 22};
uint8_t data_pins[ALL_ROWS] = {53, 51, 49, 47, 45, 43, 37, 35, 33, 31, 29};


/* -----------------------------------------------------------------  */
/** Constructor : Initialize the interrupt code. 
 * should be called in setup();
 */
 
void LedSign::Clock(short col)
{
#if DEBUG
  Serial.print("clocking ");
  Serial.print(col);
  Serial.print(" with pin ");
  Serial.println(clock_pins[col]);
#endif
    delayMicroseconds(1);
    digitalWrite(clock_pins[col], HIGH);
    digitalWrite(clock_pins[col], LOW);
}

LedSign::LedSign()
{
  Serial.println("constructor");
    _inst = this;
    memset(_buffer, 0, COLUMNS * ROWS);
    memset(_dirty, 1, COLUMNS);

    for (uint8_t i=0; i < ALL_ROWS; i++) {
      pinMode(data_pins[i], OUTPUT);
      digitalWrite(data_pins[i], LOW);
    }
    for (uint8_t i=0; i < ALL_COLUMNS; i++) {
      pinMode(clock_pins[i], OUTPUT);
      digitalWrite(clock_pins[i], LOW);
      Clock(i);
    }
    
	
return;
}

LedSign::~LedSign() {
    _inst = NULL;
    free(_buffer);
    free(_dirty);
}


/* -----------------------------------------------------------------  */
/** Clear the screen completely
 * @param set if 1 : make all led ON, if not set or 0 : make all led OFF
 */
void LedSign::Clear(int set) {
    if (LedSign::_inst) {
        LedSign::_inst->_Clear(set);
    }
}

static LedSign inst();

void LedSign::Init(uint8_t bufferType) {
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  _inst = new LedSign();
}

void LedSign::_Clear(int set) {
  for(int x=0;x<COLUMNS;x++)  
    for(int y=0;y<ROWS;y++) 
      Set(x,y,set);
}


/* -----------------------------------------------------------------  */
/** Clear an horizontal line completely
 * @param y is the y coordinate of the line to clear/light [0-8]
 * @param set if 1 : make all led ON, if not set or 0 : make all led OFF
 */
void LedSign::Horizontal(int y, int set) {
  for(int x=0;x<COLUMNS;x++)  
      Set(x,y,set);
}


/* -----------------------------------------------------------------  */
/** Clear a vertical line completely
 * @param x is the x coordinate of the line to clear/light [0-13]
 * @param set if 1 : make all led ON, if not set or 0 : make all led OFF
 */
void LedSign::Vertical(int x, int set) {
  for(int y=0;y<ROWS;y++)  
      Set(x,y,set);
}


/* -----------------------------------------------------------------  */
/** Set : switch on and off the leds. All the position 
 * calculations are done here, so we don't need to do in the
 * interrupt code
 */
void LedSign::Set(uint16_t x, uint16_t y, uint8_t c) {
    if (LedSign::_inst) {
        LedSign::_inst->_Set(x,y,c);
    }
}

void LedSign::_Set(uint16_t x, uint16_t y, uint8_t c)
{
#if DEBUG
  Serial.print("setting ");
  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  Serial.print(" to ");
  Serial.println(c);
#endif
  if (x >= COLUMNS || y >= ROWS) return;

  uint8_t oldvalue = _buffer[x][y];
  _buffer[x][y] = c;
  _dirty[x] = 1;
  
  return;
}

void LedSign::Flip(bool d) {
  if (LedSign::_inst) {
    LedSign::_inst->_Flip();
  }
}

void LedSign::_Flip() {
  #if DEBUG
  Serial.println("flip");
  #endif
  for (uint16_t j = 0; j < COLUMNS; j++) {
    if (!_dirty[j]) continue;
    for (uint16_t i = 0; i < ROWS; i++) {
      #if DEBUG
      Serial.print("setting data ");
      Serial.print(i);
      Serial.print(" with pin ");
      Serial.print(data_pins[i]);
      Serial.print(" and data ");
      Serial.println(_buffer[j][i]);
      #endif
      digitalWrite(data_pins[i], _buffer[j][i]);
    }
    Clock(j);
  }
}



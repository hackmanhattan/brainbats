/*
  GiantLOL.h - Library for controlling the charliplexed led board
  from JimmiePRodgers.com
  Created by Alex Wenger, December 30, 2009.
  Released into the public domain.
*/

#ifndef GiantLOL_h
#define GiantLOL_h

#include <inttypes.h>
#include <stdint.h>

#define SINGLE_BUFFER 0
#define DOUBLE_BUFFER 1
#define GRAYSCALE 2
#define SHADES 2

#define ALL_COLUMNS 16
#define ALL_ROWS 11
#define COLUMNS 16
#define ROWS 11


class LedSign
{
    private:
        uint8_t _buffer[COLUMNS][ROWS];
        uint8_t _dirty[COLUMNS];
        uint8_t _dirtycount;
    public:
        static volatile unsigned int tcnt2;
        void _irq();
        static LedSign *_inst;
        
        LedSign();
        ~LedSign();
        static void Init(uint8_t bufferType);
        void _Set(uint16_t x, uint16_t y, uint8_t c);
        static void Set(uint16_t x, uint16_t y, uint8_t c);
        static void Clear(int set=0);
        void _Clear(int set=0);
        static void Flip(bool d=true);
        void _Flip();
        void Clock(short col);
        static void Horizontal(int y, int set=0);
        static void Vertical(int x, int set=0);
        uint8_t width() const { return COLUMNS; }
        uint8_t height() const { return ROWS; }
};

#endif

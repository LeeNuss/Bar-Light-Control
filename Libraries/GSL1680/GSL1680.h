/**************************************************************************/
/*!
    @file     GSL1680.h
    @author   ESTBLC
    @section  HISTORY
    
    v1.0 - First release

    @thanks to wolfmanjm for the source code this lib is bassed on : https://github.com/wolfmanjm/GSL1680
*/
#include <Arduino.h>
#include <Wire.h>
#include "gslX680firmware_480x272.h"
#ifndef _GSL1680
#define _GSL1680

#define GET_FAR_ADDRESS(var) \
({ \
uint_farptr_t tmp; \
\
__asm__ __volatile__( \
\
"ldi %A0, lo8(%1)" "\n\t" \
"ldi %B0, hi8(%1)" "\n\t" \
"ldi %C0, hh8(%1)" "\n\t" \
"clr %D0" "\n\t" \
: \
"=d" (tmp) \
: \
"p" (&(var)) \
); \
tmp; \
})

class GSL1680 {
 public:
  GSL1680 ();
  
  void      begin(uint8_t WAKE, uint8_t INTRPT);
  void      clear_reg();
  void      reset();
  void      loadfw();
  void      startchip();
  void      sleep();
  void      datasend(uint8_t REG, uint8_t DATA[], uint16_t NB);
  uint8_t   dataread();
  uint8_t   readFingerID(int NB);
  uint32_t  readFingerX(int NB);
  uint32_t  readFingerY(int NB);
};

struct Scoords {
    uint8_t fingerID;
    uint32_t X,Y;
};

struct Sts_event {
    uint8_t NBfingers;
    struct Scoords coords[5];
};

#endif
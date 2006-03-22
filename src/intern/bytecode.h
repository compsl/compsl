#ifndef BYTECODE_H_
#define BYTECODE_H_

#include <inttypes.h>

typedef struct
{
  uint16_t code;
  union 
  {
    struct
    {
      uint8_t a1;
      uint8_t a2;
    };
    uint16_t a; // look up wether this works as expected
    int16_t sa;
  };
} bytecode;
#endif /*BYTECODE_H_*/

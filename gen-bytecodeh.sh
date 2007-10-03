#!/bin/bash

#1>$2 

cat <<EOF
#ifndef COMPSL_BYTECODE_H_
#define COMPSL_BYTECODE_H_

#include "compsl.h"


// all instructions working with variables take address in a1 unless otherwise
// stated

COMPSL_INTERN typedef enum  {
EOF

cut -d '#' -f1 $1 | cut -d: -f1 |
while read line; do
	if [ -n "$line" ]; then
		echo -ne "\tBC_${line},\n"
	fi
done
echo BC_NO_BYTECODE

cat <<EOF
} BC_DEF;

typedef struct
{
  BC_DEF code;
  union 
  {
    struct
    {
      uint8_t a1 /*__deprecated__*/; // uncomment depreciated after first relase
      uint8_t a2 /*__deprecated__*/; // clean up all refernces to it
    };
    uint16_t a; // look up wether this works as expected
    int16_t sa;
  };
} bytecode;

#endif /*BYTECODE_H_*/
EOF
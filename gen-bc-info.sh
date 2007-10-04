#!/bin/bash

#1>$2 

cat <<EOF
#ifndef BC_CLASS_H_
#define BC_CLASS_H_

// defines an array that can be used to classify bytecodes by their effect on the
// stack

// struct to hold info about bytecodes
typedef struct {
	int consumes; // uses this many stack elements
	int produces; // pushes this many stack elements
	
	//net effect on stack is produces - consumes
	
	enum _bc_class_t { //what kind of bytecode is it?
		NORMAL_BC, PUSH_BC, POP_BC, ARITH_BC, CALL_BC, JUMP_BC, BUILTIN_BC, END_BC, OTHER_BC, INVALID_BC
	} bc_class;
}bc_info;

static bc_info bcinfos[] = {
EOF

cut -d '#' -f1 $1 | cut -d: -f2 |
while read line; do
	if [ -n "$line" ]; then
		echo -ne "\t${line},\n"
	fi
done
echo -ne "\t{0,0,INVALID_BC}\n"

cat <<EOF
};
#endif
EOF
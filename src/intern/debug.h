

// Where xxx is the module's name
// In a modules common header file or in the c file itself: 

// #include "intern/debug.h"
// #define DPRINTF(...) xxx_DPRINTF(__VA_ARGS__) 

// In this file

// #ifdef DEBUG_xxx
// #define xxx_DPRINTF(...) fprintf(stderr, __VA_ARGS__)
// #else 
// #define xxx_DPRINTF(...) dbg_noop()
// #endif







#ifndef DEBUG_H
#define DEBUG_H 


#ifdef DEBUG_COMP
#define COMP_DPRINTF(...) fprintf(stderr, __VA_ARGS__)
#else 
#define COMP_DPRINTF(...) 	
#endif




void dbg_noop(void);

#endif


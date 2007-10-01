// $Id$

/*
    CompSL scripting language 
    Copyright (C) 2007  Thomas Jones & John Peberdy

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


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
#define COMP_DPRINTF(...) fprintf(stderr,"%24s,%4d:\t",__FILE__,__LINE__); fprintf(stderr, __VA_ARGS__)
#else 
#define COMP_DPRINTF(...) 	
#endif

#ifdef DEBUG
#define COMPSL_DPRINTF(...) printf("%24s,%4d:\t",__FILE__,__LINE__); printf(__VA_ARGS__)
#else 
#define COMPSL_DPRINTF(...) 	
#endif

#ifdef COMPSL_TRACE_COMPILE
#define COMPSL_CTRAC_PRNT(...) printf(__VA_ARGS__)
#else
#define COMPSL_CTRAC_PRNT(...)
#endif

#endif


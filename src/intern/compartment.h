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

#ifndef COMPARTMENT_H_
#define COMPARTMENT_H_

#include "../extern/compsl.h"
#include "vars.h"
#ifdef __cplusplus
extern "C" {
#endif

 
///< Maximum # of local variables, note cannot be increased since variables 
///< are internaly addressed by bytes currently
#define COMPART_MAX_VARS 256

///< Maximum # of constant values, note cannot be increased since values 
///< are internaly addressed by bytes currently
#define COMPART_MAX_CONSTS 256

///< Maximum # of cubbyholes per compartment
#define COMPART_MAX_CUBBYS 16

struct _COMPART_t
{
    varTable vt; ///< local variables
    var cons[COMPART_MAX_CONSTS]; ///< constants
    uint16_t numConst; ///< current # of constants
    uint16_t numCubbys; ///< number of cubbyholes 
    
    ///< represents one cubbyhole
    struct _CUBBY_t
    {
    	void *code; ///< pointer to the bytecode of this cubby
    	char *name; ///< name of this cubby
    } cubbys[COMPART_MAX_CUBBYS];
    
    VM *vm; ///< the vm this compartment was compiled with
    
    COMPSL_ERROR errorno; ///< error code of last error produced by this compartment
};
	
/* Takes compartment and adds a cubby, with bytecode
 * pointed at by code, and named name
 * return true on success, false otherwise
 */
COMPSL_INTERN int com_addCubby(compart *, void *code, const char *name);
COMPSL_LOCAL void com_prStats(compart *);
COMPSL_INTERN int16_t com_addConst(compart *com, intfloat val);

#ifdef __cplusplus
}
#endif

#endif /*COMPART_H_*/

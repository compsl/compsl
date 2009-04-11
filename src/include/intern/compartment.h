// $Id:compartment.h 541 2007-10-01 01:19:09Z tomj $

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

#ifndef COMPSL_COMPARTMENT_H_
#define COMPSL_COMPARTMENT_H_

#include "compsl.h"
#include <stdbool.h>
#include "bytecode.h"
#include "syms.h"
#include "vm.h"
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
    var cons[COMPART_MAX_CONSTS]; ///< constants
    varTable vt; ///< local variables

    ///< represents one cubbyhole
    struct _CUBBY_t
    {
    	bytecode *code; ///< pointer to the bytecode of this cubby
    	char *name; ///< name of this cubby
    } cubbys[COMPART_MAX_CUBBYS];

    VM *vm; ///< the vm this compartment was compiled with

    uint16_t numConst; ///< current # of constants
    uint16_t numCubbys; ///< number of cubbyholes

    bool compiled; ///< Weather or not it's been compiled

    COMPSL_ERROR errorno; ///< error code of last error produced by this compartment
};

/* Takes compartment and adds a cubby, with bytecode
 * pointed at by code, and named name
 * return true on success, false otherwise
 */
COMPSL_INTERN int com_addCubby(compart *, void *code, const char *name) COMPSL_NONNULL;
COMPSL_LOCAL void com_prStats(compart *) COMPSL_NONNULL;
COMPSL_INTERN int16_t com_addConst(compart *com, intfloat val) COMPSL_NONNULL;

#ifdef __cplusplus
}
#endif

#endif /*COMPART_H_*/

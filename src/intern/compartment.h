#ifndef COMPARTMENT_H_
#define COMPARTMENT_H_

#include "../extern/compsl.h"

#ifdef __cplusplus
extern "C" {
#endif
/* Takes compartment and adds a cubby, with bytecode
 * pointed at by code, and named name
 * return true on success, false otherwise
 */
int com_addCubby(compart *, void *code, const char *name);
void com_prStats(compart *);
int16_t com_addConst(compart *com, intfloat val);

#ifdef __cplusplus
}
#endif

#endif /*COMPART_H_*/

#ifndef COMPARTMENT_H_
#define COMPARTMENT_H_

#include "../extern/compart.h"

#ifdef __cplusplus
extern "C" {
#endif
/* Takes compartment and adds a cubby, with bytecode
 * pointed at by code, and named name
 * return true on success, false otherwise
 */
int com_addCubby(compart *, void *code, const char *name);
void com_prStats(compart *);

#ifdef __cplusplus
}
#endif

#endif /*COMPART_H_*/

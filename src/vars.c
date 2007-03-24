#include "intern/gen.h"
#include "extern/var.h"
#include "intern/vars.h"
#include <inttypes.h>
#include <malloc.h>
#include <string.h>
#include <float.h>
#include <math.h>

void varTableCreate(varTable *vt, uint16_t size)//FIXME uh what did I put this here for
{
	vt->cnt = 0;
	vt->capacity = size;
	vt->symbols = malloc(sizeof(struct SYMTABLE_T) * size);
	vt->vars = malloc(sizeof(var) * size);
}

void varTableDestroy(varTable *vt)
{
	int i;
	for(i = 0; i < vt->cnt; i++)
	{
		free(vt->symbols[i].name);// free the name string
		if(vt->symbols[i].typeflags & IS_ARRAY)
		{
			free(vt->vars[i].p);// free the array
			vt->vars[i].p = NULL;
		}
	}
	
	free(vt->symbols); vt->symbols = NULL;
	free(vt->vars); vt->vars = NULL;
}

__attribute__ ((pure)) int16_t findVar(const varTable *vt, const char *name)
{
	for(int i = 0; i < vt->cnt; i++)
	{
		if(strcmp(vt->symbols[i].name, name) == 0)// is this the one?
			return i;
	}
	
	return -1; //didn't find it
}

var *addVar(varTable *vt, uint8_t typeflags, const char *name)
{	
	if(vt->cnt >= vt->capacity) return NULL;
	
	int16_t tmp = findVar(vt, name);
	
	if(tmp == -1)
	{
		vt->vars[vt->cnt].size = -1;
		vt->vars[vt->cnt].v.i = 0;
			
		vt->symbols[vt->cnt].id = vt->cnt;
		vt->symbols[vt->cnt].name = malloc(sizeof(char)*(strlen(name) + 1));
		if(vt->symbols[vt->cnt].name == NULL) return NULL;
		strcpy(vt->symbols[vt->cnt].name, name);
		vt->symbols[vt->cnt].typeflags = typeflags;
			
		return &(vt->vars[vt->cnt++]);
	}
	else if(typeflags == vt->symbols[tmp].typeflags)
	{
		return &(vt->vars[tmp]);
	}

	return NULL;
}

bool var_testEqi(var* a, var* b) {
  if(a->size!=b->size) return false;

  // TODO: test array equality
  return (0==bcmp(a,b,sizeof(var)));
}
bool var_testEqf(var* a, var* b) {
  if(a->size!=b->size) return false;

  // TODO: test array equality
  return (fabs(a->v.f-b->v.f) < FLT_EPSILON);
}

#include "../extern/compsl.h"
#include "../extern/compart.h"
#include "../extern/var.h"
#include "../extern/vm.h"

#include "../intern/gen.h"
#include "../intern/vars.h"


#include <stdio.h>
#include <assert.h>

int main()
{
	int ret = 0;
	{	//varTable tests 
		varTable vt;
		var *vs[6];
		varTableCreate(&vt, 6);
		vs[0] = addVar(&vt,INT_VAR, "foo");
		vs[1] = addVar(&vt,IS_GLBL | FLOAT_VAR, "bar");
		vs[2] = addVar(&vt, FLOAT_VAR | IS_ARRAY, "foobar");
		vs[3] = addVar(&vt, FLOAT_VAR, "baz");
		vs[4] = addVar(&vt, FLOAT_VAR, "foo");
		vs[5] = addVar(&vt, FLOAT_VAR | IS_ARRAY, "foobar");
		
		assert(vs[2] == vs[5]);
		for(int i=0;i< 5; i++)
			for(int j=0; j<5;j++)
				if(j != i) assert(vs[i] != vs[j]);
		
		varTableDestroy(&vt);
	}
	{
	}
	return ret;
}

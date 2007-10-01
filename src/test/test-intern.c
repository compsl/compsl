#include "compsl.h"

#include "intern/gen.h"
#include "intern/vars.h"
#include "intern/compartment.h"

#include <stdio.h>
#include <assert.h>

int main()
{
	puts("Running internal tests");
	
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
        
        int16_t ind;
        
        //int16_t findVar(const varTable *vt,const char *name)
        ind = findVar(&vt, "foo");
        assert(vs[0] ==  &(vt.vars[ind])); 
		
		varTableDestroy(&vt);
	}
    
	{ // comparment constant testing
        
        //int16_t com_addConst(compart *com, intfloat val);
	}
    
    {
        //test searchSym
//        typedef struct
//        {
//            int16_t id;
//            bool isvar;// if this is false then symbol is a native or built in function
//            bool local;
//            bool array;
//            bool isfloat;
//        }symbolinfo; // used for symbol table search (for compiler)
//        
//        /**
//         * search all symbol tables and return info on a symbol
//         * 
//         * Post: if symbol doesn't exist returns a symbolinfo with id=-1
//         *       else returns info on symbol
//         */
//        symbolinfo searchSym(const char *name, compart *com) __attribute__ ((pure));
        VM *vm = createVM();
        compart *com = createComp(vm);
        
        //int16_t com_addConst(compart *com, intfloat val);
        com_addConst(com,(intfloat)0);
        
        
        destroyComp(com);
        destroyVM(vm);
    }
    
    
	return ret;
}

#include "../extern/compsl.h"
#include "../extern/compart.h"
#include "../extern/var.h"
#include "../extern/vm.h"

#include <stdio.h>


int main()
{
    int ret = 0;
    
    VM *vm = createVM();
    
    {
        int *i = vm_addInt(vm,"foo");
        if(i != vm_getInt(vm,"foo"))
        {
            ret++;
            puts("VM get/add Int FAIL");
            puts("\tget pulled out dif addr");
        }   
        else
        {
            puts("VM get/add Int PASS");
        }
        
        if(i != vm_addInt(vm,"foo"))
        {
            ret++;
            puts("VM addInt FAIL");
            puts("\tseems to allow dups");
        }
        else
        {
            puts("VM addInt PASS");
        }
        
        if(vm_addFloat(vm,"foo") != NULL)
        {
            ret++;
            puts("VM addFloat FAIL");
            puts("\tseems to allow dups (w/int)");
        }
        else
        {
            puts("VM addFloat PASS");
        }
        
        float *f = vm_addFloat(vm,"bar");
        
        if(f != vm_getFloat(vm,"bar"))
        {
            ret++;
            puts("VM get/add Float FAIL");
            puts("\tget pulled out dif addr");
        }   
        else
        {
            puts("VM get/add Float PASS");
        }
        
        if(vm_addInt(vm,"bar") != NULL)
        {
            ret++;
            puts("VM addInt FAIL");
            puts("\tseems to allow dups (w/float)");
        }
        else
        {
            puts("VM addInt PASS");
        }
        
    }
    
    destroyVM(vm);
    
	return ret;
}

#ifndef _UTILS_
#define _UTILS_

#include <tiny_stdlib.h>
#include "mem.h"
#include "file.h"
#include <slib.h>


char internalflags=0;


#define end() _end(__func__)
void _end(const char *from){
    Dict_delete(virtualmemory);
    file_end();
    printf("Main    :Exit    :Called By %s\n",from);
    print("Main    :Process :Exit Successfully\n");
    exit(0);
}


#endif
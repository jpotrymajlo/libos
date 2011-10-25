#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <stdio.h>


#ifdef DEBUG
#define DBG(A,...) printk("[%s %d] "A,__FILE__,__LINE__,## __VA_ARGS__)
#define DBGK(A,...) printk("[%s %d] "A,__FILE__,__LINE__,## __VA_ARGS__)
#define DBGN(A,...) printk("[%s %d] "A"\n",__FILE__,__LINE__,## __VA_ARGS__)
#else
#define DBG(A,...)
#define DBGK(A,...)
#define DBGN(A,...)
#endif

#endif

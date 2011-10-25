#ifndef __MALLOC_H__
#define __MALLOC_H__


typedef enum {
  MEM_OK=0,
  MEM_ERROR,
}mem_result_t;

extern void*        malloc(size_t size);
extern void         free(void* ptr);
extern mem_result_t MEM_Init(void);


#endif

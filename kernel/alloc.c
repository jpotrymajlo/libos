#include <libos.h>
#define DEBUG
#include <debug.h>
#include "alloc.h"

#define DYNAMIC_MEMORY_LIMIT 0x100000

static UINT8 dynamic_memory[DYNAMIC_MEMORY_LIMIT];

static UINT8* dynamic_memory_ptr;
static size_t free_memory_size;


/*****************************************************************************\
\*****************************************************************************/ 
mem_result_t MEM_Init(void)
{
  dynamic_memory_ptr = dynamic_memory;
  free_memory_size = DYNAMIC_MEMORY_LIMIT;
  return MEM_OK;
}

/*****************************************************************************\
\*****************************************************************************/ 
void* malloc(size_t size)
{
  UINT8* mem_ptr = dynamic_memory_ptr;
  if (free_memory_size >= size ) {
    dynamic_memory_ptr += size;
    free_memory_size -= size;
    return mem_ptr;
  }
  return NULL;  
}

/*****************************************************************************\
\*****************************************************************************/ 
void free(void* ptr)
{
  ptr = ptr;
}

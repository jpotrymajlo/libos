
#include <libos.h>
#include <arch.h>



#include <errno.h>
#include "alloc.h"
#include "queue.h"




void os_start(void)
{

  pCurrTask = queue_pop(&ReadyQueue);
  pCurrTask->bTaskState = TSK_STATE_ACTIVE;
  pCurrStack = pCurrTask->pdwTaskStk;

  lb_init_hw();
  lb_cpu_init_counter();
}


void os_init(void)
{

    dwTskInd             = 0;
    dwSysTime            = 0;
    pCurrTask            = NULL;
    pCurrStack           = NULL;
    errno                = ENOERROR;
    arch_os_init();

    MEM_Init();

    queue_init(&ReadyQueue);
    queue_init(&SuspQueue);
    queue_init(&WaitQueue);
    queue_init(&SleepQueue);
    queue_init(&EndQueue);
    
}

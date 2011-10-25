
/**
 * \defgroup przelaczanie Prze��czanie zada�
 * @{
 *  Funkcje s�u��ce do zarz�dzania zadaniami
 */



#include <libos.h>
#include <arch.h>
#include "queue.h"



/**
 * Funkcja obs�uguj�ca "tick" zegara systemowego
 *
 * \notes Funkcja powinna by� pod��czona pod przerwanie zegara
 * Zadaniem funkcji jest przejrzenie wszystkich kolejek, w kt�rych czekaj� zadania bad� na zas�b (komunikat lub semafor),
 * b�d� s� u�pione
 */

void irq_timer(void)
{
  tsk_dscr_t* pTask;
  tsk_dscr_t* pReadyTask;
  synch_t*  pEmptySynch;
  synch_t*     pSynch;

  pCurrTask->wClkCounter += lb_get_diff();
    
  pCurrTask->pdwTaskStk = pCurrStack;

  dwSysTime++;


  pSynch = (synch_t*)WaitQueue.pFirst;
  while (pSynch){
    pTask = (tsk_dscr_t*)pSynch->WaitQueue.pFirst;
    while (pTask){
          
      if (pTask->dwWaitTime != WAIT_FOREVER && !(--(pTask->dwWaitTime))){
        pReadyTask = pTask;
        pTask = (tsk_dscr_t*)pTask->node.pNext;
        queue_remove(&(pSynch->WaitQueue),(node_t*)pReadyTask);
        queue_insert(&ReadyQueue,(node_prior_t*)pReadyTask);
        pReadyTask->bTaskState = TSK_STATE_READY;
      }else{
        pTask = (tsk_dscr_t*)pTask->node.pNext;
      }            
    }
    if (!(pSynch->WaitQueue.pFirst)){
      pEmptySynch = pSynch;
      pSynch = (synch_t*)pSynch->node.pNext;
      queue_remove(&WaitQueue,(node_t*)pEmptySynch);
    }else{
      pSynch = (synch_t*)pSynch->node.pNext;
    }
    }

    pTask = (tsk_dscr_t*)SleepQueue.pFirst;
    while (pTask){
      if (pTask->dwWaitTime != WAIT_FOREVER && !(--(pTask->dwWaitTime))){
        pReadyTask = pTask;
        pTask = (tsk_dscr_t*)pTask->node.pNext;
        queue_remove(&(SleepQueue),(node_t*)pReadyTask);
        queue_insert(&ReadyQueue,(node_prior_t*)pReadyTask);
        pReadyTask->bTaskState = TSK_STATE_READY;
      }else{
        pTask = (tsk_dscr_t*)pTask->node.pNext;
      }
    }

    if (pCurrTask->dwTaskPrior >= ((tsk_dscr_t*)(ReadyQueue.pFirst))->dwTaskPrior){
      pCurrTask->bTaskState = TSK_STATE_READY;
      queue_insert((queue_t*)&ReadyQueue,(node_prior_t*)pCurrTask);
    }else{      
      return;
    }

    pCurrTask = queue_pop(&ReadyQueue);
    pCurrStack = pCurrTask->pdwTaskStk;
    pCurrTask->bTaskState = TSK_STATE_ACTIVE;

}

/**
 * Wyb�r nast�pnego zadania
 *
 * \notes Funkcja jest wywo�ywana przed wyj�ciem z funkcji obs�ugi przerwania gdy
 * zachodzi konieczno�� wyb�ru nowego zadania
 */

void sched_reschedule(void)
{
    UINT32 reg_eflags;


    INT_DISABLE(reg_eflags);

    pCurrTask->wClkCounter += lb_get_diff();

    pCurrTask->pdwTaskStk = pCurrStack;


    switch (pCurrTask->bTaskState){
      case TSK_STATE_ACTIVE:
        if (pCurrTask->dwTaskPrior >= ((tsk_dscr_t*)(ReadyQueue.pFirst))->dwTaskPrior){
          pCurrTask->bTaskState = TSK_STATE_READY;
          queue_insert((queue_t*)&ReadyQueue,(node_prior_t*)pCurrTask);
          break;
        }else{
          INT_ENABLE(reg_eflags);
          return;
        }
        break;
      case TSK_STATE_END:
        break; 
    }


    pCurrTask = queue_pop(&ReadyQueue);
    pCurrStack = pCurrTask->pdwTaskStk;
    pCurrTask->bTaskState = TSK_STATE_ACTIVE;


    INT_ENABLE(reg_eflags);

}

/** @} */

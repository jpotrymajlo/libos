/**
 * \defgroup zarzadzanie Zarzadzanie zadaniami
 * @{
 *  Funkcje s�u��ca do zarz�dzania zadaniami
 */


#include <libos.h>
#include <string.h>
#include <stdio.h>
#include <arch.h>
#include <errno.h>
#include <stdlib.h>
#include "queue.h"

tsk_dscr_t*  TskArray[MAX_NUMBER_OF_TASKS];
UINT32*       pCurrStack;
UINT32        Enable;
tsk_dscr_t* pCurrTask;
UDWORD        dwTskInd;
UDWORD        dwSysTime;
queue_t      ReadyQueue;
queue_t      WaitQueue;
queue_t      SleepQueue;
queue_t      SuspQueue;
queue_t      EndQueue;

UDWORD* stack_init(void (*task)(void),void *ptos);

static void task_procedure(void (*task)(void))
{
    pCurrTask->task_procedure();
    pCurrTask->bTaskState = TSK_STATE_END;
    queue_insert(&EndQueue,(node_prior_t*)pCurrTask);
    SWITCH_TASK;
}


/**
 * Tworzy zadanie
 *
 * \param task wska�nik do funkcji wykonywanej r�wnolegle z innymi zadaniami
 * \param ptos wska�nik do stosu uzywanego przez zadanie
 * \param name nazwa zadania
 * \param dwPrior priorytet zadania
 * \param state stan nowo utworzonego zadania
 */


UINT32 tsk_create_task(funcptr_t task,
              int size,
              char* name,
              UINT32 dwPrior,
              UBYTE state)
{
    UBYTE* ptos;
    UINT32 reg_eflags;

    ptos = malloc(size);

    INT_DISABLE(reg_eflags);
    
    if (ptos) {
      TskArray[dwTskInd] = malloc(sizeof(tsk_dscr_t));
      if (TskArray[dwTskInd] != NULL) {
        TskArray[dwTskInd]->pdwTaskStk = stack_init(task_procedure,ptos+size);
        TskArray[dwTskInd]->bTaskState = state;
        TskArray[dwTskInd]->dwTaskPrior = dwPrior;
        TskArray[dwTskInd]->taskId = dwTskInd;
        TskArray[dwTskInd]->task_procedure = task;
        TskArray[dwTskInd]->node.pNext = 0;
        TskArray[dwTskInd]->node.pPrev = 0;
        TskArray[dwTskInd]->dwWaitTime = 0;
        TskArray[dwTskInd]->pSynch=NULL;
        TskArray[dwTskInd]->pMsg = NULL;
        TskArray[dwTskInd]->errno_val = 0;
      /*     TskArray[dwTskInd].errno = ENOERROR;*/



        strcpy(TskArray[dwTskInd]->bTaskName,name);

        TskArray[dwTskInd]->bTaskState = state;
        
        switch (state){
        case TSK_STATE_SUSPENDED:
          queue_insert(&SuspQueue,(node_prior_t*)TskArray[dwTskInd++]);
          break;
        case TSK_STATE_READY:
          queue_insert(&ReadyQueue,(node_prior_t*)TskArray[dwTskInd++]);
          break;
        }
      }
    }

    INT_ENABLE(reg_eflags);

    return OK;
}

/**
 * Usypianie zadania przez to samo zadanie
 *
 * \param dwTime czas na jaki zadanie zostaje u�pione
 */


UINT32 tsk_sleep(UINT32 dwTime)
{

    UINT32 reg_eflags;

    INT_DISABLE(reg_eflags);

    pCurrTask->dwWaitTime = dwTime;
    pCurrTask->bTaskState = TSK_STATE_WAIT;
    queue_add(&SleepQueue,(node_t*)pCurrTask);

    SWITCH_TASK;
    /* jezeli budzimy zadanie z WAIT_FOREVER to trzeba ustawic mu czas */
    pCurrTask->dwWaitTime = 0;

    INT_ENABLE(reg_eflags);
    return OK;
}

/**
 * Zawieszenie zadania o podanym id
 *
 * \param TaskID id zawieszanego zadania u�pione
 */

UINT32 tsk_suspend_task(UINT16 TaskID)
{
    tsk_dscr_t* pTask;
    UINT32 reg_eflags;

    INT_DISABLE(reg_eflags);

    if (TaskID>=dwTskInd){
        INT_ENABLE(reg_eflags);
        return ERROR;
    }

    pTask = TskArray[TaskID];
    switch (pTask->bTaskState){
    case TSK_STATE_ACTIVE:
        /* usypiamy sami siebie */
        queue_insert(&SuspQueue,(node_prior_t*)pTask);
        break;
    case TSK_STATE_READY:
        queue_remove(&ReadyQueue,(node_t*)pTask);
        queue_insert(&SuspQueue,(node_prior_t*)pTask);
        break;
    case TSK_STATE_PENDING:
        queue_remove((queue_t*)&(pTask->pSynch->WaitQueue),(node_t*)pTask);
        if (!pTask->pSynch->WaitQueue.pFirst){
            queue_remove((queue_t*)&WaitQueue,(node_t*)pTask->pSynch);
        }
        break;
    case TSK_STATE_WAIT:
        queue_remove(&SleepQueue,(node_t*)pTask);
        queue_insert(&SuspQueue,(node_prior_t*)pTask);
        break;
    default:

        INT_ENABLE(reg_eflags);
        return OK;
    }
    pTask->bTaskState = TSK_STATE_SUSPENDED;
 
    SWITCH_TASK;

    INT_ENABLE(reg_eflags);
    return OK;
}
/**
 * Odwieszenia zadania o podanym id
 *
 * \param TaskID id odwieszanego zadania
 * \note Zadanie przenoszone jest do kolejki zada� oczekuj�cych
 */

UINT32 tsk_resume_task(UINT16 TaskID)
{
    tsk_dscr_t* pTask;
    UINT32 reg_eflags;


    if (TaskID>=dwTskInd)
        return ERROR;

    INT_DISABLE(reg_eflags);

    pTask = TskArray[TaskID];


    if (pTask->bTaskState==TSK_STATE_SUSPENDED){
        queue_remove(&SuspQueue,(node_t*)pTask);
        /* bylismy w jakiejs kolejce przed zawieszeniem lub otrzymalismy 
         * komunikat lub czekamy na semafor lub go dostalismy
         */
        if (pTask->dwWaitTime){
            if (pTask->pSynch){
                switch (pTask->pSynch->synchType){
                case SYNCH_EXCH:
                    pTask->pMsg = queue_pop(&(((exchange_t*)(pTask->pSynch))->MsgsQueue));
                    if (pTask->pMsg){
                        pTask->bTaskState = TSK_STATE_READY;
                        pTask->dwWaitTime = 0;
                        queue_insert(&ReadyQueue,(node_prior_t*)pTask);
                    }else{
                        pTask->bTaskState = TSK_STATE_PENDING;
                        queue_push(&WaitQueue,(node_t*)pTask->pSynch);
                        queue_insert((queue_t*)&(pTask->pSynch->WaitQueue),
                                (node_prior_t*)pTask);
                    }
                    break;
                case SYNCH_SEMAPHOR:
                    if (((sem_t*)pTask->pSynch)->dwCount > 0){
                        ((sem_t*)(pTask->pSynch))->dwCount--;
                        pTask->bTaskState = TSK_STATE_READY;
                        pTask->dwWaitTime = 0;
                        queue_insert(&ReadyQueue,(node_prior_t*)pTask);
                    }else{
                        pTask->bTaskState = TSK_STATE_PENDING;
                        queue_push(&WaitQueue,(node_t*)pTask->pSynch);
                        queue_insert((queue_t*)&(pTask->pSynch->WaitQueue),
                                (node_prior_t*)pTask);
                    }
                    break;
                default:
                    break;
                }
            }else{
                pTask->bTaskState = TSK_STATE_WAIT;
                queue_insert(&SleepQueue,(node_prior_t*)pTask);
            }
        }else{
            pTask->bTaskState = TSK_STATE_READY;
            queue_insert(&ReadyQueue,(node_prior_t*)pTask);

        }

        SWITCH_TASK;

        INT_ENABLE(reg_eflags);
        return OK;
    }else{

        INT_ENABLE(reg_eflags);
        return ERROR;
    }
}
/** @} */


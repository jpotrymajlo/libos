

#include <libos.h>
#include <arch.h>
#include "queue.h"

status_t sem_give(sem_t* pSem)
{
    tsk_dscr_t* tsk;
    status_t status=OK;
    UINT32 proc_state;

    if (pSem){

        INT_DISABLE(proc_state);

        /* jezeli zadanie czeka na ten semafor to dajmy mu znac */
        if ((tsk = queue_pop(&pSem->WaitQueue)) != NULL){
            /* jak zadanie sie obudzi bedzie wiedziec ze ma cos odebrac 
               Mamy zadanie do obudzenia*/
            tsk->pSynch = (synch_t*)pSem;
            tsk->bTaskState = TSK_STATE_READY;
            tsk->dwWaitTime = 0;
            queue_insert(&ReadyQueue,(node_prior_t*)tsk);
            /* je¿eli kolejak zadan oczekujacych na semafor jest pusta
               usun semafor z kolejki Wait */
            if (!(pSem->WaitQueue.pFirst)){
                queue_remove((queue_t*)&WaitQueue,(node_t*)pSem);
            }
            tsk->pSynch = NULL;
            SWITCH_TASK;
        }else{
        /* nie ma zadan oczekujacych na semafor */            
            pSem->dwCount++;
        }
        INT_ENABLE(proc_state);
    }else{
        status = ERROR;
    }
    return status; 
}

status_t sem_take(sem_t* pSem,INT32 time)
{

    UINT32 reg_eflags;
    status_t status = OK;

    if (pSem){
        INT_DISABLE(reg_eflags);
        /* sprawdz czy semafor jest wolny je¿eli tak to wchodzimy. */
        if (pSem->dwCount > 0){
            pSem->dwCount--;
        }else{


            /* tu niestety trzeba wrzucic zgodnie z prirytetem */
            pCurrTask->dwWaitTime = time;
            pCurrTask->bTaskState = TSK_STATE_PENDING;
            pCurrTask->pSynch     = (synch_t*)pSem;

            
            /* Jezeli kolejka zadan oczekujacych na semafor 
             * nie jest pusta wtedy nie musimy juz wrzucac go do 
             * kolejki Wait 
             */
            if (!pSem->WaitQueue.pFirst){
                queue_add(&WaitQueue,(node_t*)pSem);
            }
            /* dopisz do listy zadan oczekujacych na semafor 
             * aktualne zadanie zgodnie z jego priorytetem i przelacz zadania
             */
            queue_insert(&pSem->WaitQueue,(node_prior_t*)pCurrTask);

            SWITCH_TASK;

            /* system wrocil do wykonywania funkcji. Otrzymalismy semafor
             * lub czas oczekiwania zostal przekroczony. Manipulacje na
             * kolejkach zostaly wykonane przez sem_give
             */
            if (pCurrTask->pSynch == NULL){
                /* wszystko jest OK */
            }else{
                if (time == WAIT_FOREVER){
                    /* system nie powinien wrocic bez komunikatu dla takiego czasu 
                       zwrocic jakis blad */
                    status=ERROR;
                }else{
                    status=TIMEOUT;
                }                
            }
        }
        pCurrTask->pSynch = NULL;
        INT_ENABLE(reg_eflags);
    }else{
        status=ERROR;
    }
    return status;

}

status_t sem_create(sem_t* pSem,UINT16 count)
{
    pSem->dwCount = count;
    pSem->synchType = SYNCH_SEMAPHOR;
    queue_init(&pSem->WaitQueue);
    queue_init_node(&pSem->node);

    return OK;
}

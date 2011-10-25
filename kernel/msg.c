/**
 * \defgroup msg Kolejka komunikatów
 * @{
 *  Koleki komunikatow sa wygodnym sposobem komunikacji miedzy zadaniami
 */


#include <libos.h>
#include <arch.h>
#include <stdlib.h>
#include <stdio.h>


#include "queue.h"

/**
 * Inicjalizuje kolejke komunikatow.
 *
 * \param pExch WskaŸnik do kolejki
 */

UINT32 msg_create_mailbox(exchange_t* pExch)
{
    /* zainicjuj skrzynke samymi zerami */
    pExch->synchType = SYNCH_EXCH;

    queue_init(&pExch->MsgsQueue);
    queue_init(&pExch->WaitQueue);
    queue_init_node(&pExch->node);

    return OK;
}


static INT32 msg_send(exchange_t* pExch,msg_t* pMsg)
{
    tsk_dscr_t* tsk;

    if ((tsk = queue_pop(&pExch->WaitQueue)) != NULL){
        /* jak zadanie sie obudzi bedzie wiedziec ze ma cos odebrac 
           Mamy zadanie do obudzenia*/
        tsk->pMsg = pMsg;
        tsk->pSynch = (synch_t*)pExch;
        tsk->bTaskState = TSK_STATE_READY;
        tsk->dwWaitTime = 0;
        queue_insert(&ReadyQueue,(node_prior_t*)tsk);
        if (!(pExch->WaitQueue.pFirst)){
            queue_remove((queue_t*)&WaitQueue,(node_t*)pExch);
        }
        return 1;
    }else{
        queue_add(&pExch->MsgsQueue,(node_t*)pMsg);
        return 0;
    }
}

/**
 * Wysyla komunikat do skrzynki
 *
 * \param pExch WskaŸnik do kolejki
 * \param pMsg  WskaŸnik do komunikatu
 */

UINT32 msg_send_msg(exchange_t* pExch,msg_t* pMsg)
{
	UINT32 reg_eflags;
    UINT32 retVal;

    if (pMsg && pExch){
        INT_DISABLE(reg_eflags);
        switch (msg_send(pExch,pMsg)){
        case 1: /* zadanie wzielo komunikat, przelaczamy zadania */
            SWITCH_TASK;
        case 0: /* ls send dodalo na koniec kolejki, nie ma potrzeby przelaczenia zadan */
            retVal = OK;
            break;
        default: /* blad w przesylaniu komunikatu */
            retVal = ERROR;
            break;
        }
        INT_ENABLE(reg_eflags);
    }else{
        retVal = ERROR;
    }


    return retVal;
}

/**
 * Wysyla komunikat do skrzynki w czasie przerwania
 *
 * \param pExch WskaŸnik do kolejki
 * \param pMsg  WskaŸnik do komunikatu
 */

UINT32 msg_send_msg_intr(exchange_t* pExch,msg_t* pMsg)
{
    
    if (pMsg && pExch){
        switch (msg_send(pExch,pMsg)){
        case 1: /* zadanie wzielo komunikat nie przelaczamy zadania bo jestesmy w przerwaniu */
        case 0: /* ls send dodalo na koniec kolejki, nie ma potrzeby przelaczenia zadan */
            return OK;
        default: /* blad w przesylaniu komunikatu */
            return ERROR;
        }
    }else{
        return ERROR;
    }
}

/**
 * Odbiera komunikat ze skrzynki
 *
 * \param pExch WskaŸnik do kolejki
 * \param dwTime Czas oczekiwania na komunikat
 */


void* msg_receive_msg(exchange_t* pExch,INT32 dwTime)
{
    UINT32 reg_eflags;
    msg_t* pMsg;



    if (pExch){
        /* sprawdz czy cos jest w skrzynce. Jezeli tak, na pewno
           zadne zadanie nie czeka w kolejce bo po?*/
        INT_DISABLE(reg_eflags);
        if ((pMsg = queue_pop(&pExch->MsgsQueue)) != NULL){
            /* dostalismy komunikat mozna spadac */
        }else{
            /* nie ma zadnych zadan, trzeba bedzie poczekac */
            pCurrTask->dwWaitTime = dwTime;
            pCurrTask->bTaskState = TSK_STATE_PENDING;
            pCurrTask->pSynch     = (synch_t*)pExch;
            pCurrTask->pMsg       = NULL;
            /* jezeli zadne zadanie nie czeka przy tej skrzynce 
             * mozna ja zapisac do kolejki oczekujacych 
             */
            if (!pExch->WaitQueue.pFirst){
                queue_add(&WaitQueue,(node_t*)pExch);
            }
            /* tu niestety trzeba wrzucic zgodnie z prirytetem */
            queue_insert(&pExch->WaitQueue,(node_prior_t*)pCurrTask);

            SWITCH_TASK;
            if ((pMsg = pCurrTask->pMsg)!=NULL){
                /* czeka na nas komunikat */
            }else{
                if (dwTime == WAIT_FOREVER){
                    /* system nie powinien wrocic bez komunikatu dla takiego czasu 
                       zwrocic jakis blad */
                }else{
                    /* timeout */
                }
            }
        }

        pCurrTask->pSynch= NULL;
        pCurrTask->pMsg = NULL;

        INT_ENABLE(reg_eflags);
        return pMsg;
    }else{

        return NULL;
    }
}

/**
 * Pobiera komunikat ze skrzynki bez oczekiwania
 *
 * \param pExch WskaŸnik do kolejki
 */


void* msg_accept_msg(exchange_t* pExch)
{  
   void* pMsg;
   UINT32 reg_eflags;

   INT_DISABLE(reg_eflags);
   pMsg = queue_pop((queue_t*)&(pExch->MsgsQueue));
   INT_ENABLE(reg_eflags);

   return  pMsg;

}

/**
 * Zwraca liczbe komunikatow w kolejce
 *
 * \param pExch WskaŸnik do kolejki
 */


UINT32 msg_mailbox_length(exchange_t* pExch)
{
	UDWORD tmp;
    UINT32 reg_eflags;

    INT_DISABLE(reg_eflags);
    tmp = pExch->MsgsQueue.dwLength;
	INT_ENABLE(reg_eflags);
	return tmp;

}


/** @} */




#ifndef _LIBOS_H
#define _LIBOS_H


#include <types.h>

#define MAX_NUMBER_OF_TASKS 20

typedef enum synch_type_e{
    SYNCH_EVENT=0,
    SYNCH_MUTEX,
    SYNCH_SEMAPHOR,
    SYNCH_EXCH
}synch_type_t;


/** 
 *  Wêze³ listy dwukierunkowej
 */
typedef struct node_s{
    struct node_s * pNext; /**<wskaŸnik na nastepny element listy */
    struct node_s * pPrev; /**<wskaŸnik na poprzedni element listy */
}node_t;

/** 
 *  Wêze³ listy dwukierunkowej priorytetowej
 */

typedef struct node_prior_s{
    struct node_prior_s* pNext; /**<wskaŸnik na nastepny element listy */
    struct node_prior_s* pPrev; /**<wskaŸnik na poprzedni element listy */
    UINT32               dwPrior;        /**<priorytet wêz³a */
}node_prior_t;

/**
 * Kolejka 
 */
typedef struct queue_s{
    node_t*   pFirst; /**<wskaŸnik na pierwszy element kolejki */
    node_t*   pLast;  /**<wskaŸnik na ostatni element kolejki */
    UINT32    dwLength;        /**<d³ugoœæ kolejki */
}queue_t;

/**
 * Struktura synchronizuj¹ca 
 *
 * \note Struktura synchronizuj¹ca mo¿e byæ semaforem lub kolejka komunikatów. Gdy zadanie
 * oczekuje na semafor lub komunikat, wstawiane jest na listê zadañ oczekuj¹cy w danej strukturze,
 * a struktura jest wstawiana do listy struktur oczekuj¹cych systemu. 
 */
typedef struct synch_s{
    node_t       node;        /**<Wêze³ */
    queue_t      WaitQueue;  /**<kolejka zadañ oczekuj¹cych */    
    synch_type_t synchType; /**<type struktury */
}synch_t;

/** 
 * Kolejka komunikatów
 */
typedef struct exchange_s{
    node_t       node;        /**<Wêze³ */
    queue_t      WaitQueue;  /**<kolejka zadañ oczekuj¹cych */
    synch_type_t synchType; /**<type struktury */
    queue_t      MsgsQueue;  /**<kolejka komunikatów oczekuj¹cych na zadanie */
}exchange_t;

/**
 * Semafor
 */
typedef struct sem_s{
    node_t       node;         /**<Wêze³ */
    queue_t      WaitQueue;   /**<kolejka zadañ oczekuj¹cych */
    synch_type_t synchType;  /**<type struktury */
    UINT32       dwCount;              /**<licznik semafora */
}sem_t;
/**
 * Podstawowy komunikat
 */

typedef struct msg_s{
    node_t      node;         /**<Wêze³ */
    UINT32      dwMsgType;            /**<typ komunikatu */
    exchange_t* pHome;   /**<adres kolejki z zasobami, z któej pochodzi komunikat */
    exchange_t* pResp;   /**<adres kolejki, do której komunikat powinien byæ odes³any po przetworzeniu */
    UINT32      dwMsgLength;          /**<d³ugoœæ komunikatu */
}msg_t;


typedef struct msg_out_s{
    node_t      node;
    UINT32      dwMsgType;
    exchange_t* pHome;
    exchange_t* pResp;
    UINT32      dwMsgLength;
    UINT8       bMsgData[100];
}msg_out_t;


typedef struct msg_in_s{
    node_t      node;
    UINT32      dwMsgType;
    exchange_t* pHome;
    exchange_t* pResp;
    UINT32      dwMsgLength;
    UINT8       bMsgData[100];
}msg_in_t;

typedef void (*funcptr_t)(void);

/**
 * Opis zadania
 */

typedef struct tsk_dscr_s{
    node_t   node;  /**<Wêze³ pokazuj¹cy na poprzedni i nastepny element na liscie */
    UINT32   dwTaskPrior;  /**<priorytet zadania */
    UINT32*  pdwTaskStk;   /**<wskaŸnik na stos zadania */
    UINT8    bTaskName[10];/**<nazwa zadania */
    UINT8    bTaskState;   /**<stan zadania */
    funcptr_t task_procedure;
    UINT16   bCpuUsage;    
    UINT32   taskId;     /**<id zadania */
    msg_t*   pMsg; /**<wskaŸnik do komunikatu na który czeka zadanie */
    synch_t* pSynch; /**<wskaŸnik do struktury synchronizuj¹cej */
    INT32    dwWaitTime;    /**<czas oczekiwania na komunikat, semafor lub czas uœpienia */
    UINT64   wClkCounter;   /**<liczba cykli, w czasie których zadanie mia³o przydzielony procesor */
    int      errno_val;
}tsk_dscr_t;


/*struct lb_os_dscr{
    struct queue_t*   pReadyQueue;
    struct queue_t*   pSuspQueue;
    struct queue_t*   pWaitQueue;
    UINT64 dwTickCount;
};*/

/**
 * \addtogroup Typy
 * @{
 */
 

/** @} */

extern tsk_dscr_t*  TskArray[MAX_NUMBER_OF_TASKS];
extern UINT32*    pCurrStack;

extern tsk_dscr_t* pCurrTask;
extern UINT32     dwTskInd;
extern UINT32     dwSysTime;
extern queue_t      ReadyQueue;
extern queue_t      WaitQueue;
extern queue_t      SuspQueue;
extern queue_t      SleepQueue;
extern queue_t      EndQueue;
extern char OsVersion[];



#define LOWEST_TASK_PRIOR  0xFFFFFFFF
#define HIGHEST_TASK_PRIOR 0x00000000

#define MAX_WAIT_TIME     (WAIT_FOREVER>>1)
#define WAIT_FOREVER      (-1)

/*typy komunikatów*/
#define MSG_TIME    0x00000003
#define MSG_IO_IN   0x00000004
#define MSG_IO_OUT  0x00000005
#define MSG_CONSOLE 0x00000006

/*stany zadañ*/
typedef enum tsk_state_e{
    TSK_STATE_ACTIVE=0,
    TSK_STATE_READY,  
    TSK_STATE_WAIT,  
    TSK_STATE_PENDING,
    TSK_STATE_SUSPENDED,
    TSK_STATE_END, 
}tsk_state_t;

/**/
#define SLEEP_OK       0x0000
#define SLEEP_ERR_VAL  0x0001

/**/
/* kody bledow */
typedef enum status_e{
  OK=0,
  ERROR,
  TIMEOUT,
}status_t;




void   os_init(void);
void   os_start(void);
void   irq_timer(void);
UINT32 tsk_sleep(UINT32 dwTime);
UINT32 tsk_create_task(void (*task)(void), int size, char* name, UINT32 dwPrior, UBYTE state);
UINT32 tsk_suspend_task(UINT16 wTaskInd);
UINT32 tsk_resume_task(UINT16 wTaskInd);
UINT32 msg_create_mailbox(exchange_t* pExch);
UINT32 msg_send_msg(exchange_t* pExch,msg_t* pMsg);
UINT32 msg_send_msg_intr(exchange_t* pExch,msg_t* pMsg);
void*  msg_receive_msg(exchange_t* pExch,INT32 dwTime);
void*  msg_accept_msg(exchange_t* pExch);
UINT32 msg_mailbox_length(exchange_t* pExch);

status_t sem_give(sem_t* pSem);
status_t sem_take(sem_t* pSem,INT32 time);
status_t sem_create(sem_t* pSem,UINT16 count);


void   sched_reschedule(void);






#undef EXTERN


#endif

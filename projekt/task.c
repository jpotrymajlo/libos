

#include <libos.h>
#include <stdio.h>
#include <debug.h>






static exchange_t ExTask1;
static exchange_t ExTask2;
static exchange_t ExTask3;
static msg_t MsgTask1;
static msg_t MsgTask2;

static int counter;

static void Task1Init(void)
{
	MsgTask1.dwMsgType = 1;
	MsgTask1.pHome = &ExTask1;
	MsgTask1.pResp = &ExTask1;
    msg_send_msg(&ExTask1,&MsgTask1);
}
void Task1(void)
{   
	msg_t* msg;
	Task1Init();

    do{
		msg = msg_receive_msg(&ExTask1,WAIT_FOREVER);
        DBG("task1\n");
		
 /*       sem_take(&Sem1,WAIT_FOREVER);*/
        counter++;
  /*      sem_give(&Sem1);*/
		if (msg){
			msg_send_msg(&ExTask3,msg);        
		}
        tsk_sleep(100);
    }while(1);
}
static void Task2Init(void)
{
	MsgTask2.dwMsgType = 2;
	MsgTask2.pHome = &ExTask2;
	MsgTask2.pResp = &ExTask2;
    msg_send_msg(&ExTask2,&MsgTask2);
}

void Task2(void)
{
	msg_t* msg;
	Task2Init();
    do{
		msg = msg_receive_msg(&ExTask2,WAIT_FOREVER);
        DBG("task2\n");
		
    /*    sem_take(&Sem1,WAIT_FOREVER); */
        counter--;
    /*    sem_give(&Sem1); */

		if (msg){
			msg_send_msg(&ExTask3,msg);
		}        
        tsk_sleep(200);
    }while(1);
}
static void Task3Init(void)
{
    
}
void Task3(void)
{
	msg_t* msg;
	Task3Init();
    do{
        msg=msg_receive_msg(&ExTask3,WAIT_FOREVER);
        DBG("task3\n");
        if (msg){
			switch(msg->dwMsgType){
			case 1:
				break;
			case 2:
				break;
			default:
				break;
			}
			msg_send_msg(msg->pResp,msg);
		}
        
    }while(1);

}
void Task4(void)
{
/*    do {
        printf("%b %b %b %b\n",TskArray[1].bTaskState,
                               TskArray[2].bTaskState,
                               TskArray[3].bTaskState,
                               TskArray[4].bTaskState);
        tsk_sleep(10);
    }while(1);
*/
//  printf("%s","ala ma kota");
}

void Idle(void)
{
	do{
	}while(1);
}

void init(void)
{
    msg_create_mailbox(&ExTask1);
    msg_create_mailbox(&ExTask2);
    msg_create_mailbox(&ExTask3);

    tsk_create_task(Task1, 1000, "Task1", 0x10, TSK_STATE_READY);
    tsk_create_task(Task2, 1000, "Task2", 0x10, TSK_STATE_READY);
    tsk_create_task(Task3, 1000, "Task3", 0x10, TSK_STATE_READY);
    tsk_create_task(Task4, 1000, "Task4", 0x10, TSK_STATE_READY);
}


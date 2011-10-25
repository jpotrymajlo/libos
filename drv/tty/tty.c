#include <libos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <std/std.h>
#include "tty.h"

exchange_t ExInputIO;
exchange_t ExOutputIO;
exchange_t ExTty;
extern exchange_t ExConsole;
extern exchange_t ExConsoleRes;

static void tty_init(void)
{
    static msg_in_t  TtyMsgIn[10];
    static msg_out_t TtyMsgOut[50];
    int i;
    
    msg_create_mailbox(&ExInputIO);
    msg_create_mailbox(&ExOutputIO);
    msg_create_mailbox(&ExTty);
    
    for (i=0;i<10;i++){
        TtyMsgIn[i].dwMsgType = MSG_IO_IN;
        msg_send_msg(&ExInputIO,(msg_t*)&TtyMsgIn[i]);
    }
    for (i=0;i<50;i++){
        TtyMsgOut[i].dwMsgType = MSG_IO_OUT;
        msg_send_msg(&ExOutputIO,(msg_t*)&TtyMsgOut[i]);
    }
}

void tty(void)
{

	msg_in_t* pMsg;
    unsigned char bChar;
    unsigned char buf_index=0; /* bufor nie moze byc dluzszy niz 255 znakow */
    static unsigned char inbuff[LINE_LENGTH];
    msg_out_t* pMsgCon;


	tty_init();
    while (1){        
        if ((pMsg = msg_receive_msg(&ExTty,WAIT_FOREVER))!=NULL){
            switch (pMsg->dwMsgType){
            case MSG_IO_IN:
                bChar = pMsg->bMsgData[0];
                /* oddaj komunikat do skrzynki */
                msg_send_msg(&ExInputIO,(msg_t*)pMsg);
                switch (bChar){
                case '\r':
                case '\n':
                    inbuff[buf_index++]=0x00; /* znak konca linii */
                    std_write("\r\n",2);
                    if ((pMsgCon = msg_accept_msg(&ExConsoleRes)) != NULL){
                        strcpy(pMsgCon->bMsgData,inbuff);
                        pMsgCon->dwMsgLength = buf_index;
                        msg_send_msg(pMsgCon->pResp,(msg_t*)pMsgCon);
                    }
                    buf_index = 0;
                    break;
                case '\b':
                    if (buf_index){
                        std_write(&bChar,1);
                        buf_index--;
                    }
                    break;
                default:
                    std_write(&bChar,1);
 			        if (buf_index < LINE_LENGTH-1){
                        inbuff[buf_index++]=bChar;
                    }
			        else{
                        inbuff[buf_index++]=0x00;
                        if ((pMsgCon = msg_accept_msg(&ExConsoleRes)) != NULL){
                            strcpy(pMsgCon->bMsgData,inbuff);
                            pMsgCon->dwMsgLength = buf_index;
                            msg_send_msg(pMsgCon->pResp,(msg_t*)pMsgCon);
                        }else{
                            printk("pusta skrzynka\n");
                        }
                        buf_index = 0;
                    }
                    break;
                }
                break;
            case MSG_IO_OUT:
                if (pMsg->dwMsgLength){
                    std_write(pMsg->bMsgData,pMsg->dwMsgLength);
                }
                msg_send_msg(&ExOutputIO,(msg_t*)pMsg);                
                break;
            }
        }
    }

}



UINT32 read_tty(void)
{
    return OK;
}

UINT32 write_tty(UINT8* uChar,UINT32 size)
{
    msg_out_t* pMsg;
    pMsg = msg_receive_msg(&ExOutputIO,WAIT_FOREVER);
    if (!pMsg)
        printk("kicha printk\n");
    memcpy(pMsg->bMsgData,uChar,size);
    pMsg->dwMsgLength = size;
    msg_send_msg(&ExTty,(msg_t*)pMsg);
    return OK;
}

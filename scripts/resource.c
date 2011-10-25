
#include "resource.h"

DEF_TASK(Task1      ,1000,Task1      ,0x10,1);
DEF_TASK(Task2      ,1000,Task2      ,0x10,1);
DEF_TASK(Task3      ,1000,Task3      ,0x10,1);
DEF_TASK(Task4      ,1000,Task4      ,0x10,1);
DEF_TASK(screen     ,1000,screen     ,0x10,1);
DEF_TASK(uart       ,1000,uart       ,0x20,1);
DEF_TASK(rtl_handler,1000,rtl_handler,0x20,1);
DEF_TASK(console    ,1000,console    ,0x20,1);
DEF_TASK(Idle    ,   1000,Idle,       LOWEST_TASK_PRIOR,1);


DEF_EXCH(ExTask1);
DEF_EXCH(ExTask2);
DEF_EXCH(ExTask3);
DEF_EXCH(ExHomeScreen);
DEF_EXCH(ExWaitScreen);
DEF_EXCH(ExInpBuff);
DEF_EXCH(ExOutBuff);
DEF_EXCH(ExInputIO);
DEF_EXCH(ExCon);
DEF_EXCH(ExConRes);

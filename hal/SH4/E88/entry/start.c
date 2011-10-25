#include <libos.h>
#define DEBUG
#include <debug.h>


char OsVersion[]="0.4.9";



extern void Idle(void);
extern void tty(void);
extern void console(void);
extern void init(void);



int start(void)
{
    os_init();
    
    tsk_create_task(Idle, 1000, "Idle", LOWEST_TASK_PRIOR, TSK_STATE_READY);
    tsk_create_task(init, 1000, "init", 0x10, TSK_STATE_READY);
    tsk_create_task(tty, 1000, "tty", 0x10, TSK_STATE_READY);
    tsk_create_task(console, 1000, "console", 0x10, TSK_STATE_READY);

    os_start();

    return 0;
}

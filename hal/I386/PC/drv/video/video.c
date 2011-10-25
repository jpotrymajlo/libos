#include <libos.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <../drv/tty/tty.h>

static unsigned char _x_pos;
static unsigned char _y_pos;






static void rollscreen(UBYTE cnt)
{
    UBYTE* pScreen=(UBYTE*)0xB8000;
    cnt = 0;
    memcpy(pScreen,pScreen+160,4000);
    memsetw(pScreen+3840,0x0720,80);
}
void clrscr(void)
{
    _y_pos = 0;
    _x_pos = 0;
    memsetw((void*)0xb8000,0x0720,2000);
}


void create_video(void)
{
    clrscr();
}


UINT32 write_video(UINT8* uChar,UINT32 size)
{

    unsigned int i;
    UBYTE*	pScreen;

    for (i=0;i<size;i++){
        pScreen = (UBYTE*) (0xb8000 + _y_pos*160 + _x_pos*2);
	    switch(uChar[i]){
	    case '\n': /*EOL*/
            _x_pos=0;
            _y_pos++;
		    break;
	    case  '\b':/*ESC*/
		    if (_x_pos>0){
                *(pScreen-1) = 14;
                *(pScreen-2) = 0x20;
			    _x_pos--;
            } 
		    break;
	    default:
            *pScreen++ = uChar[i];
            *pScreen   = 14;
            _x_pos++;
        }
        if (_x_pos>=80){
            _x_pos=0;
            _y_pos++;
        }
        if (_y_pos>=25){
            _y_pos=24;
            rollscreen(1);
        }
    }
    return OK;
}

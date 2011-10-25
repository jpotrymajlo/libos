#ifndef _TTY_H
#define _TTY_H

extern exchange_t ExInputIO;
extern exchange_t ExTty;

#define LINE_LENGTH 81 /*dlugosc linii plus znak konca lancucha */


extern UINT32 write_tty(UINT8* uChar,UINT32 size);


#endif

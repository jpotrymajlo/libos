/******************************************************************************
 * errno.c
 * opis:
 * zmiany:
 * 19.10.2005  wersja poczatkowa
 *
 *****************************************************************************/

#include <libos.h>


int errno_var;

int* get_errno_ptr(void)
{
  if (pCurrTask)
    return &(pCurrTask->errno_val);
  else
    return &errno_var;
}

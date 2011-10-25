#ifndef _LIBOS_STDLIB_H_
#define _LIBOS_STDLIB_H_

#include <types.h>
#include "../kernel/alloc.h"

/**
 * \addtogroup libc
 * @{
 */

/**
 * \defgroup memop Operacje na blokach pami�ci
 * @{
 */

/** 
 * Kopiowanie obszarow pami�ci
 * 
 * Funkcja kopiuje count bajt�w z obszaru pami�ci wskazywanego przez src_ptr
 * do pami�ci wskazywanej przez dst_ptr
 * \param dst_ptr adres obaszaru docelowego
 * \param src_ptr adres �r�d�a
 * \param count liczba bajt�w do skopiowania
 * \return adres docelowego obszaru
 */
void *memcpy(void *dst_ptr, const void *src_ptr, UINT32 count);

/** 
 * Wypelnia obszar bajtami
 * 
 * Funkcja wypelnia obszar pami�ci bajtami
 * \param dst adres obaszaru docelowego
 * \param val bajt zapisywany do pami�ci
 * \param count liczba bajt�w zapisanych do pami�ci
 * \return adres docelowego obszaru
 */
void *memset(void *dst, UBYTE val, UINT32 count);

/** 
 * Wypelnia obszar liczbami 16 bitowymi
 * 
 * Funkcja wypelnia obszar pami�ci slowami 16 bitowymi
 * \param dst adres obaszaru docelowego
 * \param val slowo zapisywane do pami�ci
 * \param count liczba slow zapisanych do pami�ci
 * \return adres docelowego obszaru
 */

void *memsetw(void *dst, UWORD val, UINT32 count);

extern int memcmp(UINT8* s1,UINT8* s2,int n);
#endif

/** @}*/
/** @}*/


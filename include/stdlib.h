#ifndef _LIBOS_STDLIB_H_
#define _LIBOS_STDLIB_H_

#include <types.h>
#include "../kernel/alloc.h"

/**
 * \addtogroup libc
 * @{
 */

/**
 * \defgroup memop Operacje na blokach pamiêci
 * @{
 */

/** 
 * Kopiowanie obszarow pamiêci
 * 
 * Funkcja kopiuje count bajtów z obszaru pamiêci wskazywanego przez src_ptr
 * do pamiêci wskazywanej przez dst_ptr
 * \param dst_ptr adres obaszaru docelowego
 * \param src_ptr adres Ÿród³a
 * \param count liczba bajtów do skopiowania
 * \return adres docelowego obszaru
 */
void *memcpy(void *dst_ptr, const void *src_ptr, UINT32 count);

/** 
 * Wypelnia obszar bajtami
 * 
 * Funkcja wypelnia obszar pamiêci bajtami
 * \param dst adres obaszaru docelowego
 * \param val bajt zapisywany do pamiêci
 * \param count liczba bajtów zapisanych do pamiêci
 * \return adres docelowego obszaru
 */
void *memset(void *dst, UBYTE val, UINT32 count);

/** 
 * Wypelnia obszar liczbami 16 bitowymi
 * 
 * Funkcja wypelnia obszar pamiêci slowami 16 bitowymi
 * \param dst adres obaszaru docelowego
 * \param val slowo zapisywane do pamiêci
 * \param count liczba slow zapisanych do pamiêci
 * \return adres docelowego obszaru
 */

void *memsetw(void *dst, UWORD val, UINT32 count);

extern int memcmp(UINT8* s1,UINT8* s2,int n);
#endif

/** @}*/
/** @}*/


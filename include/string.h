#ifndef __STRING_H_
#define __STRING_H_

#include <types.h>

/**
 * \addtogroup libc
 * @{
 */
/**
 * \defgroup string Operacje na �a�cuchach
 * @{
 */
long strtol(const char* s,char **endptr,int radix);
/**
 * Dlugosc �a�cucha
 * \param str wska�nik do �a�cucha
 */
unsigned long strlen(const char *str);

char* strncpy(char* to,const char* from,int count);
/**
 * Kopiowanie �a�cucha
 * 
 * Funkcja kopiuje �a�cuch src do �a�cucha dest przerywajac ko�czac
 * dzialanie po osiagniecie ko�ca �a�cucha src
 *
 * \param dest wska�nik do �a�cucha przeznaczenia
 * \param src wska�nik do �a�cucha �r�d�owego
 * \return wska�nik docelowego �a�cucha
 * 
 */

char * strcpy(char *dest, const char *src);

/**
 * ��czenie �a�cuchow
 * 
 * Funkcja do��cza �a�cuch src do �a�cucha dest
 *
 * \param dest wska�nik do �a�cucha przeznaczenia
 * \param src wska�nik do �a�cucha �r�d�a
 * \return wska�nik do �a�cucha otrzymanego z po��czenia src i dest
 * 
 */

char* strcat(char *dest, const char *src);

void ByteToString(UBYTE b,UBYTE* s);
void WordToString(UINT16 b,UBYTE* s);
void DwordToString(UINT32 b,UBYTE* s);

/**
 * Por�wnywanie �a�cuchow
 * 
 * Funkcja por�wnuje �a�cuchy str1 i str2 por�wnuj�c znaki na tych samych pozycjach
 *
 * \param str1 wska�nik do pierwszego �a�cucha
 * \param str2 wska�nik do drugiego �a�cucha
 * \return wska�nik do �a�cucha otrzymanego z po��czenia src i dest
 * 
 */

int strcmp(const char* str1, const char* str2);


void strrev(char s[]);

/**
 * Dzieli �a�cuch na tokeny
 * 
 * Funkcja dzieli �a�cuch na fragmenty odzielone jednym lub wi�cej znakami z �a�cucha wzorcowego
 *
 * \param str wska�nik do pierwszego �a�cucha
 * \param pat wska�nik do �a�cucha z wzorcem
 * \return wska�nik do �a�cucha otrzymanego z podzialu
 * \note Funkcja nie jest reentrant. 
 *
 *  Chc�c podzieli� �a�cuch "Ala ma kota" na tokeny wzgl�dem znaku spacji lub znaku ko�ca linii:
 * 
 *  \code
 *  char line[]="ala ma kota";
 *
 *  char* token = strtok(line," \n");
 *
 *   while (token){    
 *       token = strtok(NULL," \n");
 *   }
 *  \endcode
 * 
 */

char * strtok(char * str, const char * pat);
UBYTE str2dword(UDWORD* b,UBYTE* s);

/**
 * Konwersja liczby do �a�cucha
 * 
 * Funkcja zamienia liczbe na odpowiadajacy jej �a�cuch znakow
 *
 * \param number konwertowana liczba
 * \param buf wska�nik do �a�cucha w kt�rych znajdzie sie reprezentacja liczby
 * \param radix podstawa
 * \return wska�nik do �a�cucha zawieraj�cygo reprezentacj� znakow� liczby
 * \note Wska�nik musi wskazywac na obszar gdzie umieszczane beda znaki
 * \note Aktualnie zaimplementowana jest konwersja prz podstawie 10 i 2
 * 
 */

void itoa(int number,char* buf,int radix);
int atoi(const char *nptr);
int   isdigit(int ch);


/** @}*/
/** @}*/

#endif


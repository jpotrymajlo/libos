#ifndef __STRING_H_
#define __STRING_H_

#include <types.h>

/**
 * \addtogroup libc
 * @{
 */
/**
 * \defgroup string Operacje na ³añcuchach
 * @{
 */
long strtol(const char* s,char **endptr,int radix);
/**
 * Dlugosc ³añcucha
 * \param str wskaŸnik do ³añcucha
 */
unsigned long strlen(const char *str);

char* strncpy(char* to,const char* from,int count);
/**
 * Kopiowanie ³añcucha
 * 
 * Funkcja kopiuje ³añcuch src do ³añcucha dest przerywajac koñczac
 * dzialanie po osiagniecie koñca ³añcucha src
 *
 * \param dest wskaŸnik do ³añcucha przeznaczenia
 * \param src wskaŸnik do ³añcucha Ÿród³owego
 * \return wskaŸnik docelowego ³añcucha
 * 
 */

char * strcpy(char *dest, const char *src);

/**
 * £¹czenie ³añcuchow
 * 
 * Funkcja do³¹cza ³añcuch src do ³añcucha dest
 *
 * \param dest wskaŸnik do ³añcucha przeznaczenia
 * \param src wskaŸnik do ³añcucha Ÿród³a
 * \return wskaŸnik do ³añcucha otrzymanego z po³¹czenia src i dest
 * 
 */

char* strcat(char *dest, const char *src);

void ByteToString(UBYTE b,UBYTE* s);
void WordToString(UINT16 b,UBYTE* s);
void DwordToString(UINT32 b,UBYTE* s);

/**
 * Porównywanie ³añcuchow
 * 
 * Funkcja porównuje ³añcuchy str1 i str2 porównuj¹c znaki na tych samych pozycjach
 *
 * \param str1 wskaŸnik do pierwszego ³añcucha
 * \param str2 wskaŸnik do drugiego ³añcucha
 * \return wskaŸnik do ³añcucha otrzymanego z po³¹czenia src i dest
 * 
 */

int strcmp(const char* str1, const char* str2);


void strrev(char s[]);

/**
 * Dzieli ³añcuch na tokeny
 * 
 * Funkcja dzieli ³añcuch na fragmenty odzielone jednym lub wiêcej znakami z ³añcucha wzorcowego
 *
 * \param str wskaŸnik do pierwszego ³añcucha
 * \param pat wskaŸnik do ³añcucha z wzorcem
 * \return wskaŸnik do ³añcucha otrzymanego z podzialu
 * \note Funkcja nie jest reentrant. 
 *
 *  Chc¹c podzieliæ ³añcuch "Ala ma kota" na tokeny wzglêdem znaku spacji lub znaku koñca linii:
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
 * Konwersja liczby do ³añcucha
 * 
 * Funkcja zamienia liczbe na odpowiadajacy jej ³añcuch znakow
 *
 * \param number konwertowana liczba
 * \param buf wskaŸnik do ³añcucha w których znajdzie sie reprezentacja liczby
 * \param radix podstawa
 * \return wskaŸnik do ³añcucha zawieraj¹cygo reprezentacjê znakow¹ liczby
 * \note WskaŸnik musi wskazywac na obszar gdzie umieszczane beda znaki
 * \note Aktualnie zaimplementowana jest konwersja prz podstawie 10 i 2
 * 
 */

void itoa(int number,char* buf,int radix);
int atoi(const char *nptr);
int   isdigit(int ch);


/** @}*/
/** @}*/

#endif


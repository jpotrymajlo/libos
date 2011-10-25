#ifndef __CPU_ARM_DIMMPC__
#define __CPU_ARM_DIMMPC__

/**
 * \addtogroup Typy
 * @{
 */


typedef unsigned char  UBYTE;  /** Liczba 8 bitowa bez znaku */
typedef unsigned char  ubyte;  
typedef unsigned char  uint8;  
typedef unsigned char  u_char; 
typedef unsigned char  UINT8;  /** Liczba 8 bitowa bez znaku */
typedef signed   char  BYTE;   /** Liczba 8 bitowa ze znakiem */
typedef signed   char  byte;   
typedef signed   char  int8;   
typedef signed   char  INT8;   /** Liczba 8 bitowa ze znakiem */

typedef unsigned short UWORD;  /** Liczba 16 bitowa bez znaku */
typedef unsigned short UINT16; /** Liczba 16 bitowa bez znaku */
typedef unsigned short uword;
typedef unsigned short u_short;
typedef unsigned short uint16; 
typedef signed   short WORD;   /** Liczba 16 bitowa ze znakiem */
typedef signed   short word;
typedef signed   short INT16; /** Liczba 16 bitowa ze znakiem */
typedef signed   short int16;
typedef signed   short s16;

typedef unsigned long UDWORD;/** Liczba 32 bitowa bez znaku */
typedef unsigned long UINT32;;/** Liczba 32 bitowa bez znaku */
typedef unsigned long udword;
typedef unsigned long uint32;
typedef unsigned long u32;
typedef unsigned long u_int;
typedef unsigned long u_long;
typedef signed   long DWORD;/** Liczba 32 bitowa ze znakiem */
typedef signed   long INT32;/** Liczba 32 bitowa ze znakiem */
typedef signed   long dword;
typedef signed   long int32;
typedef signed   long s32;
typedef signed   long size_t;
typedef signed   long ssize_t;
typedef signed   long off_t;

typedef unsigned long long UQWORD;
typedef unsigned long long UINT64;/** Liczba 64 bitowa bez znaku */
typedef unsigned long long uqword;
typedef unsigned long long uint64;
typedef signed   long long QWORD;
typedef signed   long long INT64;/** Liczba 64 bitowa ze znakiem */
typedef signed   long long qword;
typedef signed   long long int64;
typedef signed   long long s64;


typedef void           VOID;
typedef void*          PVOID;

#define	NULL           (void*)0


#endif

/** @}*/

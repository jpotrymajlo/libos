/******************************************************************************
 * printf.c
 *
 * zmiany:
 * 27.08.2005  funkcja sprintf, wspolna funkcja formatujaca dla printf i printk
 * 17.10.2005  zmiana znakow przeksztalcenia dla funcji printf 
 *
 *****************************************************************************/



/**
 * \addtogroup libc
 * @{
 */
/**
 * \defgroup stdio Standardowe wyj�cie
 * @{
 */


#include <libos.h>
#include <stdarg.h>
#include <string.h>
#include <debug.h>
#include <stdlib.h>
#include <std/std.h>
#include <tty/tty.h>

#define MIN(A,B)  ((A)<=(B)?(A):(B))

enum PRINT_FORMAT_STATE{
    NORMAL,
    FORMAT,
    FIELD_LENGTH,
    PRECISION,
};

static int fill_buffer(char* dst, char* src, int act_len, int max_len, int to_left, int fill_char)
{
  int ret_val=0;
  int to_fill = max_len - act_len;
  

  if (to_fill > 0) {
    if (to_left == 1) {
      strcpy(dst,src);
      ret_val += act_len;
      dst += act_len;
      memset(dst,' ',to_fill);
      ret_val += to_fill;
    } else {
      memset(dst,fill_char,to_fill);
      ret_val += to_fill;
      dst += to_fill;
      strcpy(dst,src);
      ret_val += act_len;
    }
  } else {
      strcpy(dst,src);
      ret_val += act_len;
  }
  return ret_val;
  
}

static int format_output(char* buffer,const char* format,va_list args)
{

    int field_length=0;
    int max_field_length;
    int to_fill;
    int act_field_length=0;
    int precision=0;
    int shift_to_left=0;
    char field[5];
    int length;
    char ch;
    char field_buff[50];
    int  fb_index;
    int  fill_char;

    UINT32 indeks=0;
    INT32 b;
    UBYTE*  s;
    enum PRINT_FORMAT_STATE state = NORMAL;

    while(*format){
        DBG("reszta lancucha %s\n",format);
        switch (state){
        case NORMAL:
            switch (*format){
            case '%':
                state = FORMAT;
                shift_to_left=0;
                field_length=0;
                fill_char=' ';
                precision=0;
                format++;
                break;
            default:
                buffer[indeks++]=*(format++);
                break;
            }
            break;
        case FORMAT:
            if (*format == '0') {
              fill_char = '0';
              format++;
            } else if (*format > '0' && *format <= '9'){
              state = FIELD_LENGTH;
            } else {
              switch (*format){
              case '.':
                format++;
                state=PRECISION;
                break;
              case '-':
                format++;
                shift_to_left=1;
                break;
              case 'i':
              case 'd':
                format++;
                b=va_arg(args,int);                
                itoa(b,field_buff,10);
                act_field_length=strlen(field_buff);                

                max_field_length = MIN(field_length,(int)sizeof(field_buff));                
                indeks += fill_buffer(&buffer[indeks], field_buff, act_field_length,
                                           max_field_length, shift_to_left,fill_char);

                state = NORMAL;
                break;
              case 'x':
              case 'X': 
                format++;
                b=va_arg(args,int);                
                itoa(b,field_buff,16);
                act_field_length=strlen(field_buff);                

                max_field_length = MIN(field_length,(int)sizeof(field_buff));                
                indeks += fill_buffer(&buffer[indeks], field_buff, act_field_length,
                                           max_field_length, shift_to_left,fill_char);

                state = NORMAL;
                break;
              case 'c':
                buffer[indeks++]=va_arg(args,unsigned int);
                format++;
                state = NORMAL;
                break;
              case 's': {
                int act_offset = indeks; 
                s=va_arg(args,char*);
                strncpy(field_buff,s,(sizeof(field_buff))-1);

                act_field_length = strlen(field_buff);

                max_field_length = MIN(field_length,(int)sizeof(field_buff));                
                indeks += fill_buffer(&buffer[indeks], field_buff, act_field_length,
                                           max_field_length, shift_to_left,' ');
                format++;
                state = NORMAL;
                }
                break;
              }
            }
            break;
        case FIELD_LENGTH:
          length=0;
          ch = *format++;
          while(isdigit(ch)){
            field[length++]=ch;
            ch = *(format++);                
          }
          format--;
          field[length]='\0';
          field_length = atoi(field);
          state = FORMAT;
          break;
        case PRECISION:
          length=0;
          ch = *format++;
          while(isdigit(ch)){
            field[length++]=ch;
            ch = *(format++);                
          }
          format--;
          field[length]='\0';
          precision = atoi(field);
          state = FORMAT;
          break;
        }
    }
    buffer[indeks]='\0';
    
    return indeks;
}

/** 
 * Funkcja wysy�a sformatowany �a�cuch na wyj�cie - bez buforowania
 *
 * \param format  �a�cuch formatuj�cy
 * \param ...     lista argumentow
 *
 * \note Funkcja formatuje tekst tak jak funkcja printf z t� jednak roznica ze �a�cuch wyj�ciowy
 * jest od razy wysy�any na wyj�cie: ekran lub port szeregowy
 *
 */

void printk(const char* format,...)
{
    va_list args;
    int length;
    char buffer[100]="";

    va_start(args,format);
    length = format_output(buffer,format,args);
    va_end(args);

    std_write(buffer,length);
}

/** 
 * Funkcja wysy�a sformatowany �a�cuch na wyj�cie - buforowanie
 *
 * \param format  �a�cuch formatuj�cy
 * \param ...     lista argumentow
 *
 * \note Lanuch wyj�ciowy umieszczany jest w kolejce zadania tty wysy�aj�cy znaki na wyj�cie
 *  i przyjmuj�cygo znaki z wej�cia
 *
 * Dostepne sa nastepuj�cy rodzaje formatowania
 * - %b - liczba zostanie wy�wietlona jako heksadecymalna reprezentacja bajtu
 * - %w - liczba zostanie wy�wietlona jako heksadecymalna reprezentacja slowa 16 bitowego
 * - %d - liczba zostanie wy�wietlona jako heksadecymalna reprezentacja slowa 32 bitowego
 * - %s - �a�cuch
 * - %i - liczba zostanie wy�wietlona jako dziesietna reprezentacja liczby 32 bitowej ze znakiem
 * 
 * przed kazdym specyfikatorem pola mo�e wyst�pi� liczba oznaczaj�ca minimalna szeroko�� pola
 * np:
 * \code
 * printf("liczba %4i",5);
 * zostanie wy�wietlona jako: "5   "
 * \endcode
 * Znaki " zosta�y dodane po to by pokaza� jak ograniczony
 * jest �a�cuch wyj�ciowy
 */
 
void printf(const char* format,...)
{
    va_list args;
    int length;
    char buffer[100]="";

    va_start(args,format);
    length = format_output(buffer,format,args);
    va_end(args);


    write_tty(buffer,length);
}

/** 
 * Funkcja formatuje ci�g znak�w i zapisuje go do podanego �a�cucha
 *
 * \param out     �a�cuch docelowy
 * \param format  �a�cuch formatuj�cy
 * \param ...     lista argumentow
 *
 * \note Funkcja formatuje tekst tak jak funkcja printf z t� jednak roznica ze �a�cuch wyj�ciowy
 * jest od razy wysy�any na wyj�cie: ekran lub port szeregowy
 *
 */

void sprintf(char* out,const char* format,...)
{
    va_list args;
    int length;

    va_start(args,format);
    length = format_output(out,format,args);
    va_end(args);
}


/** @} */
/** @} */

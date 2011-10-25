/******************************************************************************
 * string.c
 *
 * zmiany:
 *  09.10.2005 - w strtok byla sprawdzana wartosc wskaznika, a nie wskazywanej
 *               liczby oraz poprawiony warunek zakonczenia dzielenia
 *****************************************************************************/
#include <libos.h>
#include <types.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <debug.h>




static int ByteToChar(int byte,int base)
{
  if ( byte <= 9){
    return byte+'0';
  }else{
    if (byte <= 15 && base == 16){
      return byte+'A'-0x0A;
    }
  }
  return 'G';
  
}

void strrev(char s[])
{
  int c,i,j;
  for (i=0,j=strlen(s)-1;i<j;i++,j--){
    c=s[i];
    s[i]=s[j];
    s[j]=c;
  }
}


void itoa(int n,char s[],int base)
{
  int i=0;
  int sign=1;
  
  switch (base){
  case 2:
    break;
  case 8:
    break;
  case 10:
    if ((sign = n)<0)
      n = -n;
    break;
  case 16:
    break;
  }
  
  do{
    s[i++]=ByteToChar(n%base,base);
  }while((n/=base)>0);
  if (sign<0)
    s[i++]='-';
  s[i]='\0';
  strrev(s);
}


long atol(const char *nptr)
{
  int c;
  long total;
  int sign;

  while (isspace((int)(unsigned char) *nptr)) ++nptr;

  c = (int)(unsigned char) *nptr++;
  sign = c;
  if (c == '-' || c == '+') c = (int)(unsigned char) *nptr++;

  total = 0;
  while (isdigit(c)) 
  {
    total = 10 * total + (c - '0');
    c = (int)(unsigned char) *nptr++;
  }

  if (sign == '-')
    return -total;
  else
    return total;
}

int atoi(const char *nptr)
{
  return (int) atol(nptr);
}





unsigned long strlen(const char *str)
{
	UINT32 ret_val=0;
	while (*(str++))
	    ret_val++;

	return ret_val;
}
int toupper(int ch)
{
  if (ch >= 'a' && ch <= 'z'){
    return ch - 'a' + 'A';
  }else{
    return ch;
  }
}



/* funkcja do optymalizacji */
long strtol(const char* s,char **endptr,int radix)
{
    int sign=1; /* dodatni znak liczby */
    int number=0;
    int digit;
    char* c = (char*)s;
    errno = ENOERROR;
    /* opusc spacje */
    while (isspace(*s)) s++;
    
    switch (radix){
    case 2:
      /* sprawdz czy liczba jest prawidlowa */
      while (*c){
        if (*c != '0' && *c != '1' ){
          errno = EINVAL;
          return 0;
        }
        c++;
      }
      break;
    case 8:
      break;
    case 10:
      /*sprawdz znak liczby*/
      if (*s == '-'){
        sign = -1;
        s++;
      }else{
        if (*s == '+'){
          s++;
        }
      }
      /* sprawdz czy liczba jest prawidlowa */
      while (*c){
        if (!isdigit(*c)){
          errno = EINVAL;
          return 0;
        }
        c++;
      }
      break;
    case 16:
      /* pomin przedrostek 0x */
      if (*s == '0'){
        if (*(s+1) == 'x' || *(s+1) == 'X'){
          s+=2;
        }
      }
      if (*s == 0){
        /* liczba zawiera tylko przedrostek */
        errno = EINVAL;
        return 0;
      }
      /* sprawdz czy liczba jest prawidlowa */
      while (*c){
        if (!isdigit(*c) && !ishex(*c)){
          errno = EINVAL;
          return 0;
        }
        c++;
      }
      break;
    default:
      errno = EINVAL;
      return 0;
    }
    /* rozpoczynamy konwersje */
    while (1){
      if (isdigit(*s)){
        digit = *s - '0';
      }else{
        if (ishex(*s)){
          digit = toupper(*s) - 'A' + 10;
        }else{
          break;
        }
      }
      number = number*radix + digit;
      s++;
    }
    if (radix == 10){
      number *= sign;
    }
    if (endptr){
      *endptr = (char*)s;
    }
    return number; 
}

char * strcpy(char *to, const char *from)
{
    char *ret = to;
	while ((*to++ = *from++) != 0);
    return ret;
}

char* strncpy(char* to,const char* from,int count)
{
  char *ret = to;
  while ((*to++ = *from++) != 0 && (count--)>0);
  return ret;
}

char* strcat(char *s, const char *add)
{
	char *ret = s;
	while (*s) s++;
	while ((*s++ = *add++) != 0);
	return ret;
}
int strcmp(const char* s1,const char* s2)
{
    while (*s1 == *s2){
        if (*s1 == '\0')
            return 0;
        s1++;
	    s2++;
	}
    return (*s1 - *s2);

}
static unsigned char _issep(char c,const char* sep)
{
  unsigned char sep_len = strlen(sep);
	unsigned char i;
	for (i=0;i<sep_len;i++){
		if (c==sep[i])
			return 1;
	}
	return 0;
}
static char* akt_buff_pos;
char* strtok(char* s1,const char* sep)
{
	char* retVal = s1;
  if (s1)
	  akt_buff_pos=s1;
	else
	  s1 = akt_buff_pos;
  DBG(" reszta linii - %s\n",akt_buff_pos);
  while (*s1 && _issep(*s1,sep))
	  s1++;
	if (*s1)
    retVal = s1;
	else
		return 0;
	while (*s1 && !_issep(*s1,sep))
		s1++;
	if (*s1){
	  /* jezeli nie dotarlismy na koniec lancucha zaznacz koniec tokena i przesun
	   * wskaznik */
	  *s1=0x00;
	  akt_buff_pos = ++s1;	  
  }else{
    /* w przeciwnym wypadku po nstepnym wywolaniu zobaczymy koniec parsowanego
     * lancucha */
    *s1=0x00;
    akt_buff_pos = s1;     
  }	
	
	return retVal;
}

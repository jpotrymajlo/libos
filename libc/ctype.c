#include <ctype.h>

int isspace(int ch)
{
    if (ch == ' '){
        return 1;
    }else{
        return 0;
    }
}

int isdigit(int ch)
{
    if (ch>='0' && ch<='9'){
        return 1;
    }else{
        return 0;
    }
}
int ishex(int ch)
{
  if (ch >= 'A' && ch <= 'Z'){
    return 1;
  }
  if (ch >= 'a' && ch <= 'z'){
    return 1;
  }
  return 0;
}


/**
 * \defgroup konsola Konsola
 * @{
 *  Konsola u�ytkownika jest modu�em sluzacym do komunikacji z systemem.
 *  Dla platformy PC znaki pobierane sa z klawiatury. Aktualnie zaimplementowane
 *  sa nast�puj�ce polece�ia:
 *
 *  \li HE - wy�wietlenie informacji o pomocy
 *  \li SU - zawieszenie zadania o podanym id
 *  \li RE - odwieszenie zadania o podanym id
 *  \li PS - wy�wietlenie informacji o zadaniach w systemie
 *  \li MW - zapis bajtu pod wskazany adres -  MW adres bajt
 *  \li MR - odczyt bajtu spod adresu 
 *
 *  Argumenty polece� sa liczbami hexadecymalnymi poprzedzonymi ciagiem 0X
 */



#include <libos.h>
#include <arch.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <version.h>

#include <debug.h>


exchange_t ExConsole;
exchange_t ExConsoleRes;

#define MAX_TOKEN_NUMBER  10

static void console_init(void);
static int parse_cmd_line(char** token_arr,char* line);
static int run_command(int argc,char* argv[]);


static int OnCmdHe(int argc,char* argv[]);
static int OnCmdVe(int argc,char* argv[]);
static int OnCmdPs(int argc,char* argv[]);
static int OnCmdRe(int argc,char* argv[]);
static int OnCmdSu(int argc,char* argv[]);
static int OnCmdMemoryRead(int argc,char* argv[]);
static int OnCmdMemoryWrite(int argc,char* argv[]);
static int OnCmdMboxLength(int argc,char* argv[]);

typedef int (*cmd_proc_t)(int argc,char* argv[]);

typedef struct cmd_struct_s {
  const char* cmd_string;
  cmd_proc_t  cmd_proc;
  const char* cmd_help_string;
}cmd_struct_t;


cmd_struct_t cmd_array[] = {
  {"HELP"   , OnCmdHe,         "help"          },
  {"VERSION", OnCmdVe,         "version"       },
  {"PS"     , OnCmdPs,         "tasks list"    },
  {"RE"     , OnCmdRe,         "task resume"   },
  {"SU"     , OnCmdSu,         "task suspend"  },
  {"MR"     , OnCmdMemoryRead, "memory read"   },
  {"MW"     , OnCmdMemoryWrite,"memory write"  },
  {"LE"     , OnCmdMboxLength, "mailbox length"},
};


void console(void)
{
  msg_out_t* pMsg;
  int size;

  char* token_arr[MAX_TOKEN_NUMBER];/* tablica na 10 tokenow + ostatni zerowy 
                                       dla latwiejszego parsowania */
  console_init();

  printf("\n\n");
  printf("                     %s on %s ver. %s\n", VER_SOFT_NAME, VER_PLATFORM_NAME, VER_SOFT_VER);
  printf("                       Copyright 2004 %s\n\n", VER_AUTHOR);
    
  while (1){
    printf("libos> ");
    if ((pMsg = msg_receive_msg(&ExConsole,WAIT_FOREVER)) != NULL){
      DBG("received msg - %s\n",pMsg->bMsgData); 
      size = parse_cmd_line(token_arr,pMsg->bMsgData);
      if (size) {
        DBG("- t %s, s %d\n",token_arr[0],size);
        if (run_command(size,token_arr) != OK){
          printf("bledne polecenie lub niewlasciwe argumenty\n");
        }
      }
      msg_send_msg(&ExConsoleRes,(msg_t*)pMsg);
    }
  }
}


static void console_init(void)
{

  static msg_out_t MsgConsole[5];
  int i;
  msg_create_mailbox(&ExConsole);
  msg_create_mailbox(&ExConsoleRes);
  for (i=0;i<5;i++){
    MsgConsole[i].pResp = &ExConsole;
    MsgConsole[i].dwMsgType = MSG_CONSOLE;
    msg_send_msg(&ExConsoleRes,(msg_t*)&MsgConsole[i]);
  }
}

static int parse_cmd_line(char** token_arr,char* line)
{
  int token_num=0; 
  char* token;
  DBG("parsowana linia - %s\n",line);
  token = strtok(line," \n\t");
  while (token && token_num<MAX_TOKEN_NUMBER){
    DBG("token %d,%s\n",token_num,token);
    token_arr[token_num++] = token;
    token = strtok(NULL," \n\t");
  }
  return token_num;
}


static int run_command(int argc,char* argv[])
{
  unsigned int i;

  for (i=0; i<sizeof(cmd_array)/sizeof(cmd_array[0]); i++) {
    if (!strcmp(cmd_array[i].cmd_string, argv[0])) {
      return cmd_array[i].cmd_proc(argc,argv);
    }
  }
  return ERROR;
}

static int OnCmdHe(int argc,char* argv[])
{
  unsigned int i;
  for (i=0; i<sizeof(cmd_array)/sizeof(cmd_array[0]); i++) {
    printf("%-10s - %s\n",cmd_array[i].cmd_string,cmd_array[i].cmd_help_string);
  }
  return OK;
}



static int OnCmdVe(int argc,char* argv[])
{
  printf("%s\n",VER_AUTHOR);
  printf("%s on %s ver. %s\n", VER_SOFT_NAME, VER_PLATFORM_NAME, VER_SOFT_VER);
  printf("kompilacja: %s %s\n",__DATE__,__TIME__);
  
  return OK;

}
/* zoptymalizowac uzywajac tablicy */
static char* state_to_char(tsk_state_t state)
{
  switch (state) {
  case TSK_STATE_WAIT:
    return "W";
  case TSK_STATE_PENDING:
    return "P";
  case TSK_STATE_READY:
    return "R";
  case TSK_STATE_ACTIVE:
    return "A";
  case TSK_STATE_SUSPENDED:
    return "S";
  case TSK_STATE_END:
    return "E";
  default:
    return "U";
  }
}

static int OnCmdPs(int argc,char* argv[])
{
  UINT16 i;
  UINT64 total_time=0;
  INT8   proces_descr[80];
/*	    INT_DISABLE(REG);*/
  for (i=1;i<dwTskInd;i++){
    total_time += TskArray[i]->wClkCounter;
  }
  printf("\nUzycie procesora - %d",(UINT32)div64(1000*total_time,total_time+
                                  TskArray[0]->wClkCounter));
  total_time += TskArray[0]->wClkCounter;
  printf("\nID | Stan | CPU prom | nazwa");

  for (i=0;i<dwTskInd;i++){
    sprintf(proces_descr,"\n%-04d %-06s %-10d %-010s ",TskArray[i]->taskId,
                        state_to_char(TskArray[i]->bTaskState), 
                       (UINT32)div64(1000*TskArray[i]->wClkCounter,total_time),
                       TskArray[i]->bTaskName);
    printf("%s",proces_descr);
  }
  printf("\n");
/*	    INT_ENABLE(REG);*/
        
  return OK;
}

static int OnCmdRe(int argc,char* argv[])
{
  UINT32 taskId;

  if (argc == 2){
    taskId = strtol(argv[1],NULL,16);
    if (errno == ENOERROR){
      tsk_resume_task(taskId);
      return OK;
    }
  }
  return ERROR;
}

static int OnCmdSu(int argc,char* argv[])
{
  UINT32 taskId;

  if (argc == 2) {
    taskId = strtol(argv[1],NULL,16);
    if (errno == ENOERROR){
      DBG("zadanie %d do zawieszenia\n",taskId);
      tsk_suspend_task(taskId);
      return OK;
    }
  }
  return ERROR;
}

static int OnCmdMemoryRead(int argc,char* argv[])
{
  UINT32 addr;
  msg_in_t* pMsg;

  if (argc == 2){
    addr = strtol(argv[1],NULL,16);
    if (errno == ENOERROR){
      while (1){            
        printf("%x - %x",addr,*((UINT8*)addr));
        addr++;
        if ((pMsg = msg_receive_msg(&ExConsole,WAIT_FOREVER)) != NULL){
          if (pMsg->bMsgData[0]==0x1b){ /* kod klawisza ESC */
            return OK;
          }
          msg_send_msg(&ExConsoleRes,(msg_t*)pMsg);
        }
      }
    }
  }
  return ERROR;
}

static int OnCmdMemoryWrite(int argc,char* argv[])
{
  UINT32 addr;
  UINT32 byte;

  if (argc == 3){
    addr = strtol(argv[1],NULL,16);
    if (errno == ENOERROR){    
      return ERROR;
    }
    byte = strtol(argv[2],NULL,16);
    if (errno == ENOERROR){    
      return ERROR;
    }
    *((UINT8*)addr) = (UINT8)byte;
    return OK;

  }
  return ERROR;
}

static int OnCmdMboxLength(int argc,char* argv[])
{
  UINT32 addr;

  if (argc == 2){
    addr = strtol(argv[1],NULL,16);
    if (errno == ENOERROR){        
      printf("%d\n",msg_mailbox_length((exchange_t*)addr));
      return OK;
    }
  }
  return ERROR;
}


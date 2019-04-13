#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);
//cmd_si
static int cmd_si(char *args){
//	printf("%s\n",args);
	    char *arg = strtok(NULL," ");
	    int n = 1,t = 1;
		    
		if(arg != NULL){
			    t = atoi(arg);
				 n = t;
			}
			  cpu_exec(n);
  return 0;
}
//cmd_info,查看信息
static int cmd_info(char *args) {
      char *arg = strtok(NULL," ");
       if(strcmp(arg,"r") == 0){//打印所有寄存器
//		cpu.gpr[0]._32 = 20;

			for(int i = 0; i < 8; i++) {
  				  printf("%s\t0x%08x\t%u\n",regsl[i],cpu.gpr[i]._32,cpu.gpr[i]._32);	       
		  	 }

			for(int i = 0; i < 8; i++) {
				 printf("%s\t0x%04x\t%u\n",regsw[i],cpu.gpr[i]._16,cpu.gpr[i]._16);
			 }

			for(int i = 0; i < 8; i++) {
				 printf("%s\t0x%02x\t%u\n",regsb[i],cpu.gpr[i%4]._8[i/4],cpu.gpr[i%4]._8[i/4]);

			}
			printf("\neip\t0x%08x\t%u\n",cpu.eip,cpu.eip);
	 	}
		else if(strcmp(arg,"w") == 0) {

		}
		else {
		    printf("Please check info arguments !\n");
		}
		return 0;
}

//cmd_x扫描内存
static int cmd_x(char *args) {
	char *arg1 = strtok(NULL," ");//内存长度len
	char *arg2 = strtok(NULL," ");//起始地址vaddr
	int len = atoi(arg1);

	bool success = true;
	uint32_t vaddr;
	vaddr=expr(arg2,&success);//利用表达式处理地址
//	sscanf(arg2,"%x",&vaddr);//16进制字符串转换为32位无符号数,表示地址
	printf("Address       Dword block   Byte sequence\n");
	
	uint32_t instr;//存放地址vaddr中的内容
	for(int i = 0; i < len; i++) {
		instr =  vaddr_read(vaddr+i*len,len);//从内存中读取内容,即指令
		printf("0x%08x    0x%08x    ",vaddr+i*len,instr);
		//小端顺序输出
		uint8_t *p_instr = (uint8_t*)&instr;
		for(int j = 0; j < len; j++) {
			printf("%02x",*(p_instr + j));
		}
		printf("\n");
	}	
	return 0;
}
static int cmd_p(char *args) {
	bool success = true;
  expr(args,&success);
	if(success)printf("expr is ok\n");
	else printf("expr is error\n");
	return 0;
}
//指令结构体：名称，描述，函数名
static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Execute in step", cmd_si},
  { "info","r --show all of registers infomation \n     - w --others",cmd_info}, 
  { "x","scan memory",cmd_x},
  { "p","evaluate expression",cmd_p},
 	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

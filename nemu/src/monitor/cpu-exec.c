#include "nemu.h"
#include "monitor/monitor.h"
#include "monitor/watchpoint.h"
/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INSTR_TO_PRINT 99999999//打印步数<16
#define MAX_EXECUTE_STEPS 0xffffffff//死循环判断
int nemu_state = NEMU_STOP;

void exec_wrapper(bool);
WP* scan_watchpoint(WP** wp_trigger);

/* Simulate how the CPU works. */
void cpu_exec(uint64_t n) {
  if (nemu_state == NEMU_END) {

    printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
    return;
  }
  nemu_state = NEMU_RUNNING;
  bool print_flag = n < MAX_INSTR_TO_PRINT;
  uint32_t now_steps = 0;
  for (; n > 0; n --) {
    /*
  	 * Execute one instruction, including instruction fetch,
     * instruction decode, and the actual execution. */
    uint32_t old_eip = cpu.eip;
	 	exec_wrapper(print_flag);
    old_eip = old_eip;//仅仅是为了消除注释DEBUG后的报错
    //执行步数超出限制，报错
		now_steps++;
		if(now_steps == MAX_EXECUTE_STEPS){
			nemu_state = NEMU_STOP;
			Log("程序陷入死循环，请检查程序！！！\n");
		}
#ifdef DEBUG
    /* TODO: check watchpoints here. */
		WP* wp_trigger[32];
		WP* p = scan_watchpoint(wp_trigger),*wp = NULL;
		if(p != NULL){
	   	for(int i = 0; *(wp_trigger + i) != NULL; i++){
				wp = *(wp_trigger+i);
				printf("Hit watchpoint %d at address 0x%08x\n",wp->NO,old_eip);
				printf("%-10s= %s\n","expr",wp->expr);
				printf("%-10s= %08x\n","old_val",wp->old_val);
				printf("%-10s= %08x\n","new_val",wp->new_val);
				printf("program paused\n");
				wp->old_val = wp->new_val;
			}
			if(sizeof(wp_trigger) != 0)nemu_state = NEMU_STOP;
		}
#endif

#ifdef HAS_IOE
    extern void device_update();
    device_update();
#endif

    if (nemu_state != NEMU_RUNNING) { return; }
  }

  if (nemu_state == NEMU_RUNNING) { nemu_state = NEMU_STOP; }
}

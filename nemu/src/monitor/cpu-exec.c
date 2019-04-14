#include "nemu.h"
#include "monitor/monitor.h"
//#include "monitor/watchpoint.h"
/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INSTR_TO_PRINT 16//打印步数<16

int nemu_state = NEMU_STOP;

void exec_wrapper(bool);
//WP* scan_watchpoint();

/* Simulate how the CPU works. */
void cpu_exec(uint64_t n) {
  if (nemu_state == NEMU_END) {

    printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
    return;
  }
  nemu_state = NEMU_RUNNING;
  bool print_flag = n < MAX_INSTR_TO_PRINT;

  for (; n > 0; n --) {
    /*
  	 * Execute one instruction, including instruction fetch,
     * instruction decode, and the actual execution. */
    exec_wrapper(print_flag);

#ifdef DEBUG
    /* TODO: check watchpoints here. */
//		WP* wp_trigger[32];
/*		WP* p = scan_watchpoint(wp_trigger),*wp = NULL;
		if(p != NULL)
	   	for(int i = 0; i < sizeof(wp_trigger); i++){
				wp = wp_trigger[i];
				printf("Hit watchpoint %d at address 0x%08x\n",wp->NO,cpu.eip);
				printf("%-10s= %s\n","expr",wp->expr);
				printf("%-10s= %08x\n","old_val",wp->old_val);
				printf("%-10s= %08x\n","new_val",wp->new_val);
				printf("program paused\n");
				wp->old_val = wp->new_val;
			}
			if(sizeof(wp_trigger) != 0)nemu_state = NEMU_STOP;
		}*/
#endif

#ifdef HAS_IOE
    extern void device_update();
    device_update();
#endif

    if (nemu_state != NEMU_RUNNING) { return; }
  }

  if (nemu_state == NEMU_RUNNING) { nemu_state = NEMU_STOP; }
}

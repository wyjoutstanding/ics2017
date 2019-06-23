#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC];
static int nr_proc = 0;
//指针不能NULL
PCB pcb_tmp;
PCB *current = &pcb_tmp;

int num_proc = 0;

uintptr_t loader(_Protect *as, const char *filename);

void load_prog(const char *filename) {
  int i = nr_proc ++;
  _protect(&pcb[i].as);

  uintptr_t entry = loader(&pcb[i].as, filename);

  // TODO: remove the following three lines after you have implemented _umake()
//  _switch(&pcb[i].as);
//  current = &pcb[i];
//  ((void (*)(void))entry)();

  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);

  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void *)entry, NULL, NULL);
}

_RegSet* schedule(_RegSet *prev) {
  current->tf = prev;//保存上一个
  // current = &pcb[0];//切换到当前
  if(nr_proc / 200 == 0){
    current = &pcb[0];
    nr_proc ++;
  }
  else {
    current = &pcb[1];
    nr_proc = 0;
  }
  _switch(&current->as);//虚拟地址切换，CR3页目录首地址
  return current->tf;//返回即将要切换的上下文
}

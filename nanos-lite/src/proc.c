#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC];
static int nr_proc = 0;
PCB *current = NULL;

uintptr_t loader(_Protect *as, const char *filename);

void load_prog(const char *filename) {
  int i = nr_proc ++;
  _protect(&pcb[i].as);
  Log("filename = %s", filename);

  uintptr_t entry = loader(&pcb[i].as, filename);

  // TODO: remove the following three lines after you have implemented _umake()
  // _switch(&pcb[i].as);
  // current = &pcb[i];
  // ((void (*)(void))entry)();

  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);

  // Log("load_prog has reached _umake stack.start = %d", stack.start);
  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void *)entry, NULL, NULL);
}

static int num = 0;

_RegSet* schedule(_RegSet *prev) {
  return NULL;
  // save the context pointer
  current->tf = prev;
  num++;

  // current 指针指向当前运行进程的 PCB
  // 轮流返回仙剑奇侠传和 hello
  if (num % 200 == 0) {
    current = &pcb[1];
  }
  else {
    current = &pcb[0];
  }

  // TODO: switch to the new address space,
  _switch(&current->as);

  // then return the new context
  return current->tf;
}

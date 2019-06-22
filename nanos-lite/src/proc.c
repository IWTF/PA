#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC];
static int nr_proc = 0;
PCB *current = NULL;

uintptr_t loader(_Protect *as, const char *filename);

void load_prog(const char *filename) {
  int i = nr_proc ++;
  _protect(&pcb[i].as);

  uintptr_t entry = loader(&pcb[i].as, filename);

  // TODO: remove the following three lines after you have implemented _umake()
  // _switch(&pcb[i].as);
  // current = &pcb[i];
  // ((void (*)(void))entry)();

  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);

  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void *)entry, NULL, NULL);
}

static PCB *current_game = &pcb[0];
void switch_game() {
  current_game = (current_game == &pcb[0] ? &pcb[2] : &pcb[0]);
}

_RegSet* schedule(_RegSet *prev) {
  // return NULL;
  // current->tf = prev;

  // current = (current == current_game ? &pcb[1] : current_game);

  // _switch(&current->as);
  // save the context pointer
  current->tf = prev;

  // always select pcb[0] as the new process
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);

  // TODO: switch to the new address space,
  // then return the new context
  return current->tf;
}

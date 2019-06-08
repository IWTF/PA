#include "nemu.h"
#include "monitor/monitor.h"
#include "monitor/watchpoint.h"

/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INSTR_TO_PRINT 100000

int nemu_state = NEMU_STOP;

void exec_wrapper(bool);

/* Simulate how the CPU works. */
void cpu_exec(uint64_t n) {
  if (nemu_state == NEMU_END) {
    printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
    return;
  }
  nemu_state = NEMU_RUNNING;

  bool print_flag = n < MAX_INSTR_TO_PRINT;
  if((int)n == -1)
    print_flag = 1;

  for (; n > 0; n --) {
    /* Execute one instruction, including instruction fetch,
     * instruction decode, and the actual execution. */
    exec_wrapper(print_flag);

#ifdef DEBUG
    /* TODO: check watchpoints here. */
    // for (int i=0; i<32; i++) {
    //   WP *p = scan_watchpoint();
    //   if (p != NULL) {
    //     nemu_state = NEMU_STOP;

    //     if (p->type == 0) {
    //       printf("  Hit watchpoint %d at address %d\n", p->NO, cpu.eip);
    //       printf("  expr \t  = %s\n", p->expr);
    //       printf("  old value = %d\n", p->old_val);
    //       printf("  new value = %d\n", p->new_val);
    //       p->old_val = p->new_val;
    //     } else {
          // char *addr = strtok(p->expr, " ");
          // addr = strtok(NULL, " ");
          // addr = strtok(NULL, " ");
    //       printf("Breakpoint %d at %s\n", p->NO, addr);
    //       p->old_val = 0;
    //       p->new_val = 0;
    //       break;
    //     }
    //   }
    // }
    WP *p = scan_watchpoint();
    while (p) {
      nemu_state = NEMU_STOP;
      if (p->type == 0) {
        printf("  Hit watchpoint %d at address %8x\n", p->NO, cpu.eip);
        printf("  expr \t  = %s\n", p->expr);
        printf("  old value = %d\n", p->old_val);
        printf("  new value = %d\n", p->new_val);
        p->old_val = p->new_val;
      } else {
        // vaddr_write(cpu.eip, 1, p->str);
        printf("  Breakpoint %d at %8x\n", p->NO, cpu.eip);
        // printf("  pause at %8x\n", cpu.eip);
        // printf("  %x:\tcc\n", cpu.eip);
        break;
      }
      p = p->next;
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

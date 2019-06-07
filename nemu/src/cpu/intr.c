#include "cpu/exec.h"
#include "memory/mmu.h"

// NO = id_dest->val, ret_addr = decoding.seq_eip
void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  // TODO();

  // 依次将EFLAGS, CS(代码段寄存器), EIP寄存器的值压栈
  rtl_push((rtlreg_t *)&cpu.eflags);
  rtl_push((rtlreg_t *)&cpu.CS);
  rtl_push((rtlreg_t *)&ret_addr);

  uint32_t IDT_BASE = cpu.IDTR.IDT_BASE;
  uint32_t eip_low, eip_high, offset;
  eip_low = vaddr_read(IDT_BASE + NO * 8, 4) & 0x0000ffff;
  eip_high = vaddr_read(IDT_BASE + NO * 8 + 4, 4) & 0xffff0000;
  
  offset = eip_low | eip_high;
  decoding.jmp_eip = offset;
  decoding.is_jmp = true;
}

void dev_raise_intr() {
}

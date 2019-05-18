#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  // printf("mov CF is: %d\n", cpu.eflags.CF);
  print_asm_template2(mov);
}

make_EHelper(push) {
  // TODO();
  rtl_li(&t3, id_dest->val);
  rtl_push(&t3);
  print_asm_template1(push);
}

make_EHelper(pop) {
  // TODO();
  rtl_pop(&t3);
  operand_write(id_dest, &t3);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
  // TODO();
  rtl_mv(&cpu.esp, &cpu.ebp);
  rtl_pop(&cpu.ebp);

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    // TODO();
    if (cpu.gpr[0]._16 < 0)
      cpu.gpr[2]._16 = 0xffff;
    else
      cpu.gpr[2]._16 = 0;
  }
  else {
    // TODO();
    if (cpu.gpr[0]._32 < 0)
      cpu.gpr[2]._32 = 0xffffffff;
    else
      cpu.gpr[2]._32 = 0;
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;

  printf("operand_size is 16? :  %d\n", decoding.is_operand_size_16);
  printf("id_dest size: %d\n", id_dest->width);

  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}

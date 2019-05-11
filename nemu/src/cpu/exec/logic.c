#include "cpu/exec.h"

make_EHelper(test) {
  // TODO();
  // printf("scr1 val:%d\n", id_src->val);
  // printf("src2 val:%d\n", id_src2->val);
  rtl_and(&t0, &id_dest->val, &id_src->val);

  printf("id_dest->width: %d\n", id_dest->val);
  
  rtl_update_ZFSF(&t0, id_dest->width);
  rtl_set_OF(&tzero);
  rtl_set_CF(&tzero);

  print_asm_template2(test);
}

make_EHelper(and) {

  rtl_and(&t0,&id_dest->val,&id_src->val);
  printf("%#x\n", id_dest->val);
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&id_dest->val,id_dest->width);
  rtl_set_OF(&tzero);
  rtl_set_CF(&tzero);

  print_asm_template2(and);
}

make_EHelper(xor) {
  // TODO();
  rtl_xor(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);

  rtl_update_ZFSF(&t0, id_dest->width);
  rtl_set_OF(&tzero);
  rtl_set_CF(&tzero);

  print_asm_template2(xor);
}

make_EHelper(or) {
  // TODO();
  rtl_or(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);

  printf("id_dest->width: %d\n", id_dest->width);

  rtl_update_ZFSF(&t0, id_dest->width);
  rtl_set_OF(&tzero);
  rtl_set_CF(&tzero);

  print_asm_template2(or);
}

make_EHelper(sar) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  TODO();

  print_asm_template1(not);
}

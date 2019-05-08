#include "cpu/exec.h"

make_EHelper(test) {
  TODO();

  print_asm_template2(test);
}

make_EHelper(and) {
 // TODO();
  rtl_sext(&id_src->val,&id_src->val,id_src->width);//signed extent imm
	rtl_and(&t1,&id_dest->val,&id_src->val);
	operand_write(id_dest,&t1);//write into register/memory
	//flag affected
	rtl_set_CF(&t0);
	rtl_set_OF(&t0);
	rtl_update_ZFSF(&t1,id_dest->width);
  print_asm_template2(and);
}

make_EHelper(xor) {
//  TODO();
//  if((&cpu.eax) == (&id_dest->val))
//  printf("dest type:%d dest_val:%u src_val:%u\n",id_dest->type,id_dest->val,id_src->val);
  rtl_xor(&t0,&id_dest->val,&id_src->val);
	//rtl_sr(id_dest->reg,id_dest->width,&t0);//写入寄存器
	operand_write(id_dest,&t0);

	rtl_update_ZFSF(&t0,id_dest->width);
	rtl_set_OF(&tzero);
	rtl_set_CF(&tzero);
  print_asm_template2(xor);
}

make_EHelper(or) {
//  TODO();
  rtl_or(&t0,&id_dest->val,&id_src->val);
	operand_write(id_dest,&t0);

	rtl_set_OF(&tzero);
	rtl_set_CF(&tzero);
	rtl_update_ZFSF(&t0,id_dest->width);

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

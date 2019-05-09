#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
 	print_asm_template2(mov);
}
//push's execute function
make_EHelper(push) {
 // rtl_push(&reg_l(id_dest->reg));//ebp入栈,esp<-ebp,only consider dword (uint32_t)
  rtl_push(&id_dest->val);//reg/imm/memory
  print_asm_template1(push);
}

make_EHelper(pop) {
//  TODO();
//  rtl_pop(&reg_l(id_dest->reg));
//  rtl_pop(&id_dest->val);
	rtl_pop(&t3);
	operand_write(id_dest,&t3);
	print_asm_template1(pop);
}

make_EHelper(pusha) {
//  TODO();
  rtl_push(&id_dest->val);
  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
  TODO();

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
   // TODO();
    rtl_lr(&t0,R_EAX,2);
		rtl_li(&t1,0x0ffff);
		if(t0 < 0) rtl_sr(R_EDX,2,&t1);
		else rtl_sr(R_EDX,2,&tzero);
	}
  else {
    rtl_lr(&t0,R_EAX,4);
		rtl_li(&t1,0x0ffffffff);
		if(t0 < 0) rtl_sr(R_EDX,4,&t1);
		else rtl_sr(R_EDX,4,&tzero);
//	   TODO();
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
//change two r/m
make_EHelper(xchg) {
 // rtl_li(&t0,id_dest->val);//read dest
//	operand_write(id_dest,&id_src->val);//dest<-src
//	operand_write(id_src,&t0);//src<-dest
  print_asm_template2(xchg);
}

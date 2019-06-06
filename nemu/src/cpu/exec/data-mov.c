#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
 	print_asm_template2(mov);
}
//push's execute function
make_EHelper(push) {
 // rtl_push(&reg_l(id_dest->reg));//ebp入栈,esp<-ebp,only consider dword (uint32_t)
  rtl_sext(&id_dest->val,&id_dest->val,id_dest->width);//push sext
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
  t3 = cpu.esp;
  rtl_push(&cpu.eax);
	rtl_push(&cpu.ecx);
	rtl_push(&cpu.edx);
	rtl_push(&cpu.ebx);
	rtl_push(&t3);
	rtl_push(&cpu.ebp);
	rtl_push(&cpu.esi);
	rtl_push(&cpu.edi);
  print_asm("pusha");
}

make_EHelper(popa) {
//  TODO();
  rtl_pop(&cpu.edi);
	rtl_pop(&cpu.esi);
	rtl_pop(&cpu.ebp);
	rtl_pop(&t3);
	rtl_pop(&cpu.ebx);
	rtl_pop(&cpu.edx);
	rtl_pop(&cpu.ecx);
	rtl_pop(&cpu.eax);
  print_asm("popa");
}

make_EHelper(leave) {
//  TODO();
  rtl_lr(&t0,R_EBP,4);
	rtl_sr(R_ESP,4,&t0);
	rtl_pop(&t0);
	rtl_sr(R_EBP,4,&t0);
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
	uint32_t t;
  if (decoding.is_operand_size_16) {
//    TODO();
    rtl_lr(&t,R_EAX,1);//AL
  	rtl_sext(&t,&t,1);//AL sext to  AX
	  rtl_sr(R_EAX,2,&t);//AX <- sext(AL)
	}
  else {
//    TODO(); 
    rtl_lr(&t,R_EAX,2);//AX
//		Log("eax:%x t0:%x\n",cpu.eax,t);
  	rtl_sext(&t,&t,2);//AX sext to EAX
//		Log("t0:%x\n",t);
	  rtl_sr(R_EAX,4,&t);//EAX <- sext(AX)
	 }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t3, &id_src->val, id_src->width);
//	Log("src:%x t3:%x w:%d\n",id_src->val,t3,id_src->width);
  operand_write(id_dest, &t3);
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

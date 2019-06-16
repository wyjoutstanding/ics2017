#include "cpu/exec.h"

void diff_test_skip_qemu();
void diff_test_skip_nemu();

make_EHelper(lidt) {
//  TODO();
//  rtl_lm((rtlreg_t*)&cpu.IDTR.limit,id_src->addr,2);
	cpu.IDTR.limit = vaddr_read(id_dest->addr,2);
//	Log("limit:%x\n",cpu.IDTR.limit);
	if(decoding.is_operand_size_16){
		t3 = vaddr_read(id_dest->addr+2,4);
		t3 = t3 & 0x00ffffff;
		rtl_mv((rtlreg_t*)&cpu.IDTR.base,&t3);
	}
	else {
		cpu.IDTR.base = vaddr_read(id_dest->addr+2,4);
	}
//	Log("base:%x\n",cpu.IDTR.base);
	 //	rtl_mv((rtlreg_t*)&cpu.IDTR.base,(rtlreg_t*)&(vaddr_read(id_src->addr+2,4)));
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
//  TODO();
//   if(id_dest->reg == 0) cpu.cr0.val = reg_l(id_src->reg);
  switch (id_dest->reg) {
	  case 0: cpu.cr0.val = id_src->val; break;
	  case 3: cpu.cr3.val = id_src->val; break;
	  default: Assert(0, "unsupported control register");
  }
  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
//  TODO();
//   if(id_src->reg == 0) reg_l(id_dest->reg) = cpu.cr0.val;
// 	else reg_l(id_dest->reg) = cpu.cr3.val;
  switch (id_src->reg) {
	  case 0: t0 = cpu.cr0.val; break;
	  case 3: t0 = cpu.cr3.val; break;
	  default: Assert(0, "unsupported control register");
  }
  operand_write(id_dest, &t0);
  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}
void raise_intr(uint8_t, vaddr_t);
make_EHelper(int) {
//  TODO();
	raise_intr((uint8_t)id_dest->val,decoding.seq_eip); 	
  print_asm("int %s", id_dest->str);

#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
//  TODO();
  rtl_pop(&decoding.jmp_eip);
	decoding.is_jmp = true;
	rtl_pop(&cpu.CS);
	rtl_pop((rtlreg_t*)&cpu.EFLAGS);
  print_asm("iret");
}

uint32_t pio_read(ioaddr_t, int);
void pio_write(ioaddr_t, int, uint32_t);

make_EHelper(in) {
 // TODO();
  uint32_t data = pio_read(id_src->val,id_src->width);
	operand_write(id_dest,&data);
  print_asm_template2(in);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(out) {
//  TODO();
  pio_write(id_dest->val,id_src->width,id_src->val);
	diff_test_skip_qemu();
  print_asm_template2(out);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

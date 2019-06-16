#include "cpu/exec.h"

void diff_test_skip_qemu();
void diff_test_skip_nemu();

make_EHelper(lidt) {
  // TODO();
  cpu.IDTR.limit = vaddr_read(id_dest->addr, 2);
  if (decoding.is_operand_size_16) {
	cpu.IDTR.base = vaddr_read(id_dest->addr + 2, 4) & 0x00ffffff;
  }
  else {
	cpu.IDTR.base = vaddr_read(id_dest->addr + 2, 4);	
  }
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  // TODO();

  switch (id_dest->reg) {
	  case 0: cpu.cr0.val = id_src->val; break;
	  case 3: cpu.cr3.val = id_src->val; break;
	  default: Assert(0, "unsupported control register");
  }

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  // TODO();

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

extern void raise_intr(uint8_t NO, vaddr_t ret_addr);

make_EHelper(int) {
  // TODO();
  raise_intr(id_dest->val, decoding.seq_eip);

  print_asm("int %s", id_dest->str);

#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
  // TODO();
  rtl_pop(&decoding.jmp_eip);
  decoding.is_jmp = 1;
  rtl_pop(&cpu.CS);
  rtl_pop((rtlreg_t*)&cpu.EFLAGS);
  print_asm("iret");
}

uint32_t pio_read(ioaddr_t, int);
void pio_write(ioaddr_t, int, uint32_t);

make_EHelper(in) {
  // TODO();
  // printf("src = 0x%08x\tdest = 0x%08X\n", id_src->val, id_dest->val);
  t0 = pio_read(id_src->val, id_dest->width);
  // printf("in t0 = 0x%08x\n", t0);
  operand_write(id_dest, &t0);
  // printf("dest = 0x%08x\n", id_dest->val);
  print_asm_template2(in);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(out) {
  // TODO();
  // printf("src = 0x%08x\tdest = 0x%08X\n", id_src->val, id_dest->val);
  // printf("src-width = %d\tdest-width = %d\n", id_src->width, id_dest->width);
  // t0 = pio_read(id_dest->val, id_src->width);
  // printf("b t0 = 0x%08x\n", t0);
  pio_write(id_dest->val, id_src->width, id_src->val);
  // t0 = pio_read(id_dest->val, id_src->width);
  // printf("a t0 = 0x%08x\n", t0);
  print_asm_template2(out);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

//  TODO();
		uint32_t base,jmp,jmp_low,jmp_high;
	  rtl_push((rtlreg_t*)&cpu.EFLAGS);
		rtl_push((rtlreg_t*)&cpu.CS);
		rtl_push((rtlreg_t*)&cpu.eip);
		//取值
		base = cpu.IDTR.base;
		//GDT -> 8B
		jmp_low = vaddr_read(base+8*NO,4) & 0x0000ffff;
		jmp_high = vaddr_read(base+8*NO+4,4) & 0xffff0000;
		jmp = jmp_low | jmp_high;
		decoding.jmp_eip = jmp;
		decoding.is_jmp = true;
}

void dev_raise_intr() {
}

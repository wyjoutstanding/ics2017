#include "nemu.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];


/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
// printf("pmem u:%p  addr:%p u+len:%p",pmem,(void*)addr,(void*)(pmem+addr));
  uint32_t *vaddr =(uint32_t*)(pmem + addr);
//	*vaddr = *vaddr & 0;
	printf("vaddr:%08x\n",*vaddr);
 	return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  memcpy(guest_to_host(addr), &data, len);
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  paddr_write(addr, len, data);
}

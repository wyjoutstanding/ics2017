#include "nemu.h"

#define PMEM_SIZE (128 * 1024 * 1024)

int is_mmio(paddr_t);
uint32_t mmio_read(paddr_t, int, int);
void mmio_write(paddr_t, int, uint32_t, int);

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];


/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
// printf("pmem u:%p  addr:%p u+len:%p",pmem,(void*)addr,(void*)(pmem+addr));
//  uint32_t *vaddr =(uint32_t*)(pmem + addr);//获取指定指令的地址
//	*vaddr = *vaddr & 0;
//	printf("vaddr:%08x\n",*vaddr);
  int mmio_id = is_mmio(addr);
	if(mmio_id != -1)return mmio_read(addr,len,mmio_id) & (~0u >> ((4 - len) << 3));
	return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  int mmio_id = is_mmio(addr);
	if(mmio_id != -1)mmio_write(addr,len,data,mmio_id);
	else	memcpy(guest_to_host(addr), &data, len);
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  paddr_write(addr, len, data);
}

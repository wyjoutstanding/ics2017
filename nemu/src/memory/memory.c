#include "nemu.h"

#include "memory/mmu.h"//self add

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

uint32_t paddr_read(paddr_t addr ,int len);
void paddr_write(paddr_t addr, int len, uint32_t data);
paddr_t page_translate(vaddr_t vaddr) {	
	PDE pd;
	PTE pb;

	paddr_t pd_addr = (cpu.cr3.page_directory_base << 12) |  ((vaddr >> 22) << 2);
	pd.val = paddr_read(pd_addr, 4);//fetch a PageDirectory
	assert(pd.present == 1);
	
	paddr_t pb_addr = (pd.page_frame << 12) | (((vaddr >> 12) & 0x3ff) << 2);
	pb.val = paddr_read(pb_addr, 4);//fecth a PageTableEntry
  Log("cr3:0x%08u pd_addr:0x%08u pb_val:0x%08u pb_addr:0x%08u pb_val:0x%08u", cpu.cr3.page_directory_base, pd_addr, pd.val, pb_addr, pb.val);
	assert(pb.present == 1);	
  //tell os both levels are used;replacement algorithm
	if(pd.accessed == 0){
		pd.accessed = 1;
		paddr_write(pd_addr, 4, pd.val);
	}
	if(pb.accessed == 0) {
	  pb.accessed = 1;
	  paddr_write(pb_addr, 4, pb.val);
	}
	//writeback
  if(pb.dirty == 0 && pb.read_write == 1) {
		pb.dirty = 1;
		paddr_write(pb_addr, 4, pb.val);
	}
  
	paddr_t paddr = (pb.page_frame << 12) | (vaddr & 0xfff);
  Log("va:0x%u pa:0x%u", vaddr, paddr);
	return paddr;
}


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
	if(cpu.cr0.paging){
		if( len > PAGE_SIZE - (addr & 0xfff) + 1) {//cross page read 
			assert(0);
		}
		else {
			paddr_t paddr = page_translate(addr);
      return paddr_read(paddr,len);
		}
	}
	
	return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
 if(cpu.cr0.paging) {
	 if( len > PAGE_SIZE - (addr & 0xfff) + 1) {//cross page write
		 assert(0);
	 }
	 else {
		 paddr_t paddr = page_translate(addr);
		 paddr_write(paddr, len, data);
	 }
 }
 return paddr_write(addr, len, data);
}

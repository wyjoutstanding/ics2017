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
  // if ( !cpu.cr0.protect_enable || !cpu.cr0.paging)return vaddr;
	paddr_t pd_addr = (cpu.cr3.page_directory_base << 12) |  (((vaddr >> 22) & 0x3ff) << 2);
	pd.val = paddr_read(pd_addr, 4);//fetch a PageDirectory
	// Log("cr3:0x%08x cr3<<12:0x%08x vaddr:0x%08x vaddr>>22:0x%08x", cpu.cr3.page_directory_base, cpu.cr3.page_directory_base << 12, vaddr, vaddr >> 22); 
	assert(pd.present == 1);
	
	paddr_t pb_addr = (pd.page_frame << 12) | (((vaddr >> 12) & 0x3ff) << 2);
	pb.val = paddr_read(pb_addr, 4);//fecth a PageTableEntry
  
	// Log("pd_addr:0x%08x pd_val:0x%08x pb_addr:0x%08x pb_val:0x%08x",pd_addr, pd.val, pb_addr, pb.val);
	// Log("pb_addr:0x%08x pb_val:0x%08x pb_addr:0x%08x pb_val:0x%08x",pd_addr, pd.val, pb_addr, pb.val);
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
  // Log("va:0x%08x pa:0x%08x", vaddr, paddr);
	return paddr;
}

uint32_t paddr_read(paddr_t addr, int len) {
  int mmio_id = is_mmio(addr);
	if(mmio_id != -1)return mmio_read(addr,len,mmio_id) & (~0u >> ((4 - len) << 3));
	return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  int mmio_id = is_mmio(addr);
	if(mmio_id != -1)mmio_write(addr,len,data,mmio_id);
	else	memcpy(guest_to_host(addr), &data, len);
}

//判断是否跨页：
//一个页大小为4096B（12位表示），所以仅需判断虚拟地址的高20位即可
 #define CROSS_PAGE(addr, len) \
   ((((addr) + (len) - 1) & ~PAGE_MASK) != ((addr) & ~PAGE_MASK))
// uint32_t vaddr_read(vaddr_t addr, int len) {
//   paddr_t paddr;

//   if (CROSS_PAGE(addr, len)) {
//     /* data cross the page boundary */
//     union {
//       uint8_t bytes[4];
//       uint32_t dword;
//     } data = {0};
//     for (int i = 0; i < len; i++) {
//       paddr = page_translate(addr + i, false);
//       data.bytes[i] = (uint8_t)paddr_read(paddr, 1);
//     }
//     return data.dword;
//   } else {
//     paddr = page_translate(addr, false);
//     return paddr_read(paddr, len);
//   }
// }

// void vaddr_write(vaddr_t addr, int len, uint32_t data) {
//   paddr_t paddr;

//   if (CROSS_PAGE(addr, len)) {
//     /* data cross the page boundary */
//     assert(0);
//     for (int i = 0; i < len; i++) {
//       paddr = page_translate(addr, true);
//       paddr_write(paddr, 1, data);
//       data >>= 8;
//       addr++;
//     }
//   } else {
//     paddr = page_translate(addr, true);
//     paddr_write(paddr, len, data);
//   }
// }
uint32_t vaddr_read(vaddr_t addr, int len) {
	if(cpu.cr0.protect_enable && cpu.cr0.paging){
		if( CROSS_PAGE(addr, len)) {//cross page read 
		// Log("len:%d",len);
		union {
			uint8_t bytes[4];
			uint32_t dword;
		}readData = {0};

		paddr_t paddr;
		/*
			依次将len个字节读入bytes数组中，返回其共用体，等于小端解释
		 */
		for(int i = 0; i < len; i++){
			paddr = page_translate(addr + i);
		    readData.bytes[i] = (uint8_t)paddr_read(paddr, 1);
		}
		// Log("dword:%x",data.dword);
		// assert(0);
		return readData.dword;
    
		}
		else {
			paddr_t paddr = page_translate(addr);
      return paddr_read(paddr,len);
		}
	}
	return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
 if(cpu.cr0.protect_enable && cpu.cr0.paging) {
	 if( CROSS_PAGE(addr, len)) {//cross page write
		 assert(0);
	 }
	 else {
		 paddr_t paddr = page_translate(addr);
		 paddr_write(paddr, len, data);
	 }
 }
 else paddr_write(addr, len, data);//else别漏
}

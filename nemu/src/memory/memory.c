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
paddr_t page_translate(vaddr_t vaddr, bool is_write) {	
	PDE pd;
	PTE pb;
  if ( !cpu.cr0.protect_enable || !cpu.cr0.paging)return vaddr;
	paddr_t pd_addr = (cpu.cr3.page_directory_base << 12) |  (((vaddr >> 22) & 0x3ff) << 2);
	pd.val = paddr_read(pd_addr, 4);//fetch a PageDirectory
//	Log("cr3:0x%08x cr3<<12:0x%08x vaddr:0x%08x vaddr>>22:0x%08x", cpu.cr3.page_directory_base, cpu.cr3.page_directory_base << 12, vaddr, vaddr >> 22); 
	assert(pd.present == 1);
	
	paddr_t pb_addr = (pd.page_frame << 12) | (((vaddr >> 12) & 0x3ff) << 2);
	pb.val = paddr_read(pb_addr, 4);//fecth a PageTableEntry
  
	// Log("cr3:0x%08x pd_addr:0x%08x pb_val:0x%08x pb_addr:0x%08x pb_val:0x%08x", cpu.cr3.page_directory_base, pd_addr, pd.val, pb_addr, pb.val);
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

  // PDE pde, *pgdir;
  // PTE pte, *pgtab;
  // paddr_t paddr = addr;

  // if (cpu.cr0.protect_enable && cpu.cr0.paging) {
  //   pgdir = (PDE *)(intptr_t)(cpu.cr3.page_directory_base << 12);
  //   pde.val = paddr_read((intptr_t)&pgdir[(addr >> 22) & 0x3ff], 4);
  //   Assert(pde.present, "Page dir not valid: va 0x%x eip: 0x%x", addr, cpu.eip);
  //   pde.accessed = 1;

  //   pgtab = (PTE *)(intptr_t)(pde.page_frame << 12);
  //   pte.val = paddr_read((intptr_t)&pgtab[(addr >> 12) & 0x3ff], 4);
  //   Assert(pte.present, "Page dir not valid: va 0x%x", addr);
  //   pte.accessed = 1;
  //   pte.dirty = pte.read_write ? 1 : pte.dirty;
  //   // Log("va:0x%08x pa:0x%08x",addr, paddr);
   
  //   paddr = (pte.page_frame << 12) | (addr & PAGE_MASK);
	// // Log("va:0x%08x pa:0x%08x",addr, paddr);
  // }

  // return paddr;
}

// paddr_t page_translate(vaddr_t addr, bool is_write) {
//   PDE pde, *pgdir;
//   PTE pte, *pgtab;
//   paddr_t paddr = addr;

//   if (cpu.cr0.protect_enable && cpu.cr0.paging) {
//     pgdir = (PDE *)(intptr_t)(cpu.cr3.page_directory_base << 12);
//     pde.val = paddr_read((intptr_t)&pgdir[(addr >> 22) & 0x3ff], 4);
//     Assert(pde.present, "Page dir not valid: va 0x%x eip: 0x%x", addr, cpu.eip);
//     pde.accessed = 1;

//     pgtab = (PTE *)(intptr_t)(pde.page_frame << 12);
//     pte.val = paddr_read((intptr_t)&pgtab[(addr >> 12) & 0x3ff], 4);
//     Assert(pte.present, "Page dir not valid: va 0x%x", addr);
//     pte.accessed = 1;
//     pte.dirty = is_write ? 1 : pte.dirty;

//     paddr = (pte.page_frame << 12) | (addr & PAGE_MASK);
//   }

//   return paddr;
// }

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
	// if(cpu.cr0.protect_enable && cpu.cr0.paging){
		if( CROSS_PAGE(addr, len)) {//cross page read 
			assert(0);
		}
		else {
			paddr_t paddr = page_translate(addr,false);
      return paddr_read(paddr,len);
		}
	// }
	
	// return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
//  if(cpu.cr0.protect_enable && cpu.cr0.paging) {
	 if( CROSS_PAGE(addr, len)) {//cross page write
		 assert(0);
	 }
	 else {
		 paddr_t paddr = page_translate(addr,true);
		 paddr_write(paddr, len, data);
	 }
//  }
//  paddr_write(addr, len, data);
}

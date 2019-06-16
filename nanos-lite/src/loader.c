#include "common.h"

#define PAGE_SIZE 4096
#define DEFAULT_ENTRY ((void *)0x8048000)
void ramdisk_read(void *buf, off_t offset, size_t len);
size_t get_ramdisk_size();

ssize_t fs_read(int fd, void* buf, size_t len);
int fs_open(const char* pathname, int flags, int mode);
int fs_close(int fd);
size_t fs_filesz(int fd);
void* new_page(void);

uintptr_t loader(_Protect *as, const char *filename) {
//  TODO();
//  ramdisk_read(DEFAULT_ENTRY,0,get_ramdisk_size());
/*	int fd = fs_open(filename, 0, 0);
  fs_read(fd, DEFAULT_ENTRY, fs_filesz(fd));
	fs_close(fd);
*/  
	int fd = fs_open(filename, 0, 0);
	uint32_t file_size = fs_filesz(fd);
	int nr_page = 0;
	while(nr_page * PAGE_SIZE < file_size) {
		void* pa = new_page();
		assert(pa != NULL);
		nr_page++;
		void* va = as->ptr;
    Log("Map va to pa: 0x%08x to 0x%08x", va, pa);
		_map(as, va, pa);
		fs_read(fd, pa, PAGE_SIZE);
	}
	fs_close(fd);
	return (uintptr_t)DEFAULT_ENTRY;
}

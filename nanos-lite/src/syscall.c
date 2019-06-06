#include "common.h"
#include "syscall.h"
#include "fs.h"

int mm_brk(uint32_t new_brk);

static inline uintptr_t sys_open(uintptr_t pathname, uintptr_t flags, uintptr_t mode) {
  TODO();
  return 1;
}
static inline uintptr_t sys_write(uintptr_t fd, uintptr_t buf, uintptr_t len) {
//  TODO();
  Log("fd:%d len:%d str:%s",fd,len,(char*)buf);
	if(fd == 1 || fd == 2){
//		char s[len]=vaddr_read(buf,len);
		for(int i = 0; i < len; i++){
			_putc(((char*)buf)[i]);
			Log("%c",((char*)buf)[i]);
		}
	}
  return len;
}

static inline uintptr_t sys_read(uintptr_t fd, uintptr_t buf, uintptr_t len) {
  TODO();
  return 1;
}

static inline uintptr_t sys_lseek(uintptr_t fd, uintptr_t offset, uintptr_t whence) {
  return fs_lseek(fd, offset, whence);
}

static inline uintptr_t sys_close(uintptr_t fd) {
  TODO();
  return 1;
}

static inline uintptr_t sys_brk(uintptr_t new_brk) {
  TODO();
  return 1;
}
static inline uintptr_t sys_none(_RegSet *r){
		SYSCALL_ARG1(r) = 1;
		return 1;
}

static inline uintptr_t sys_exit(){
	_halt(0);
	return 1;
}

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
	a[1] = SYSCALL_ARG2(r);
	a[2] = SYSCALL_ARG3(r);
	a[3] = SYSCALL_ARG4(r);

  switch (a[0]) {
    case SYS_none: sys_none(r); break;
		case SYS_exit: sys_exit(); break;
		case SYS_write: Log("sys_write");SYSCALL_ARG1(r) = sys_write(a[1], a[2], a[3]); break;
		default: panic("Unhandled syscall ID = %d", a[0]);
  }
  return NULL;
}

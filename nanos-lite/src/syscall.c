#include "common.h"
#include "syscall.h"
#include "fs.h"

int mm_brk(uint32_t new_brk);

static inline uintptr_t sys_open(uintptr_t pathname, uintptr_t flags, uintptr_t mode) {
  TODO();
  // 我们约定，这个系统调用什么都不用做，直接返回 1
  return 1;
}

static inline uintptr_t sys_write(uintptr_t fd, uintptr_t buf, uintptr_t len) {
  TODO();
  return 1;
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

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);

  // printf("a[1] = %d\n", a[1]);
  
  uintptr_t ret = -1;
  switch (a[0]) {
    case SYS_none: ret = 1; break;
    case SYS_exit: _halt(0); break;
    case SYS_write: ret = fs_write(a[1], (void *)a[2], a[3]); break;
    case SYS_brk: ret = 0; break;
    case SYS_open: ret = fs_open((char *)a[1], a[2], a[3]); break;
    case SYS_read: ret = fs_read(a[1], (void *)a[2], a[3]); break;
    case SYS_lseek: ret = fs_lseek(a[1], a[2], a[3]); break;
    case SYS_close: ret = fs_close(a[1]); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  SYSCALL_ARG1(r) = ret;

  return NULL;
}

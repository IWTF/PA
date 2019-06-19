#include "common.h"
#include "syscall.h"
#include "fs.h"

extern char _end;
extern ssize_t fs_read(int fd, void *buf, size_t len);
extern ssize_t fs_write(int fd, const void *buf, size_t len);
extern int fs_open(const char *pathname, int flags, int mode);
extern off_t fs_lseek(int fd, off_t offset, int whence);
extern int fs_close(int fd);

int mm_brk(uint32_t new_brk);

static inline uintptr_t sys_open(uintptr_t pathname, uintptr_t flags, uintptr_t mode) {
  // TODO();
  return fs_open((char *)pathname, flags, mode);
  return 1;
}

static inline uintptr_t sys_write(uintptr_t fd, uintptr_t buf, uintptr_t len) {
  // TODO();
  // Log("调用了sys_write()");
  return fs_write(fd, (void *)buf, len);
  return 1;
}

static inline uintptr_t sys_read(uintptr_t fd, uintptr_t buf, uintptr_t len) {
  // TODO();
  return fs_read(fd, (void *)buf, len);
  return 1;
}

static inline uintptr_t sys_lseek(uintptr_t fd, uintptr_t offset, uintptr_t whence) {
  return fs_lseek(fd, offset, whence);
}

static inline uintptr_t sys_close(uintptr_t fd) {
  // TODO();
  return fs_close(fd);
  return 1;
}

static inline uintptr_t sys_brk(uintptr_t new_brk) {
  // TODO();
  return mm_brk(new_brk);
  // return 0;
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
    case SYS_none:  ret = 1; break;
    // case SYS_exit:  _halt(0); break;
    case SYS_exit:  _halt(a[1]); break;
    case SYS_write: ret = sys_write(a[1], a[2], a[3]); break;
    case SYS_brk:   ret = sys_brk(a[1]); break;
    case SYS_open:  ret = sys_open(a[1], a[2], a[3]); break;
    case SYS_read:  ret = sys_read(a[1], a[2], a[3]); break;
    case SYS_lseek: ret = sys_lseek(a[1], a[2], a[3]); break;
    case SYS_close: ret = sys_close(a[1]); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  SYSCALL_ARG1(r) = ret;

  return NULL;
}

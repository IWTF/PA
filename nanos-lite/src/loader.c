#include "common.h"

extern uint8_t ramdisk_start;
extern uint8_t ramdisk_end;
extern void ramdisk_read(void *buf, off_t offset, size_t len) ;
extern int fs_close(int fd);
extern int fs_open(const char *pathname, int flags, int mode);
extern ssize_t fs_read(int fd, void *buf, size_t count);
#define DEFAULT_ENTRY ((void *)0x4000000)

uintptr_t loader(_Protect *as, const char *filename) {
  // TODO();
  // ramdisk_read(DEFAULT_ENTRY, 0, ((&ramdisk_end) - (&ramdisk_start)));
  int fd = fs_open(filename, 0, 0);
  fs_read(fd, DEFAULT_ENTRY, ((&ramdisk_end) - (&ramdisk_start)));
  fs_close(fd);
  return (uintptr_t)DEFAULT_ENTRY;
}

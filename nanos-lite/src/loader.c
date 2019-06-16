#include "common.h"

extern uint8_t ramdisk_start;
extern uint8_t ramdisk_end;
extern void ramdisk_read(void *buf, off_t offset, size_t len) ;
extern int fs_close(int fd);
extern int fs_open(const char *pathname, int flags, int mode);
extern ssize_t fs_read(int fd, void *buf, size_t count);
size_t fs_filesz(int fd);
extern void* new_page(void);
#define DEFAULT_ENTRY ((void *)0x4000000)

uintptr_t loader(_Protect *as, const char *filename) {
  // TODO();
  // ramdisk_read(DEFAULT_ENTRY, 0, (ramdisk_end-ramdisk_start));

  // int fd = fs_open(filename, 0, 0);
  // fs_read(fd, DEFAULT_ENTRY, fs_filesz(fd)); 
  // fs_close(fd); 

  // 打开待装入的文件后，还需要获取文件大小
  int fd = fs_open(filename, 0, 0);
  int file_size = fs_filesz(fd);

  void *pa;
  void *va = DEFAULT_ENTRY;

  while (file_size > 0) {
  	// 获取一个空闲物理页
	pa = new_page();

  	Log("Map va to pa: 0x%08x to 0x%08x", va, pa);
	_map(as, va, pa);
	fs_read(fd, pa, PGSIZE);

	// 更新虚拟地址
	va += PGSIZE;

	// 当file_size不足一页大小时，跳出循环处理
	if ((file_size - PGSIZE) < 0)
		break;
	file_size = file_size - PGSIZE;
  }

  if (file_size > 0) {
  	pa = new_page();

  	Log("Map va to pa: 0x%08x to 0x%08x", va, pa);
	_map(as, va, pa);
	fs_read(fd, pa, file_size);
  }
  fs_close(fd);

  return (uintptr_t)DEFAULT_ENTRY;
}

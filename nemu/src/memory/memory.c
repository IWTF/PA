#include "nemu.h"
#include "device/mmio.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  int map_NO = is_mmio(addr);
	if (map_NO != -1) {
		return mmio_read(addr, len, map_NO);
	}
  	return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  // is_mmio() 函数判断一个物理地址是否被映射到 I/O 空间,如果是, is_mmio() 会返回映射号, 否则返回 -1
	int map_NO = is_mmio(addr);
	if (map_NO != -1) {
		mmio_write(addr, len, data, map_NO);
	}
  	memcpy(guest_to_host(addr), &data, len);
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  if(cpu.cr0.paging) {   // 判断是否开启了分页机制
      // if (data cross the page boundary) {   // 特殊情况： 数据位于两个页
      //     /* this is a special case, you can handle it later. */
      //     assert(0);
      // }
      // else {      // 虚拟地址到物理地址的转换，返回
      //     paddr_t paddr = page_translate(addr);
      //     return paddr_read(paddr, len);
      // }
    paddr_t paddr = page_translate(addr);
    return paddr_read(paddr, len);
  }
  else
      return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  // paddr_write(addr, len, data);
  if(cpu.cr0.paging) {   // 判断是否开启了分页机制
      // if (data cross the page boundary) {   // 特殊情况： 数据位于两个页
      //     /* this is a special case, you can handle it later. */
      //     assert(0);
      // }
      // else {      // 虚拟地址到物理地址的转换，返回
      //     paddr_t paddr = page_translate(addr);
      //     return paddr_write(paddr, len, data);
      // }
    paddr_t paddr = page_translate(addr);
    return paddr_write(paddr, len, data);
  }
  else
      return paddr_write(addr, len, data);
}

// 自定义page_translate()函数
paddr_t page_translate(vaddr_t vaddr) {
  // 若没有开启分页机制，直接返回
  if (cpu.cr0.paging == 0)
    return vaddr;

  Log("vaddr is: 0x%x", vaddr);
  Log("CR3 val is: 0x%x", cpu.cr3.val);
  Log("CR3 page_directory_base is: 0x%x", cpu.cr3.page_directory_base);
  assert(0);
  
  // 获取页目录索引和页表索引
  // uint32_t pde_index = vaddr>>22;
  // uint32_t pte_index = vaddr>>12 & 0x3ff;

  // // 获取页表基址
  // uint32_t pde_addr  = (cpu.cr3.page_directory_base<<12) + (pde_index<<2);
  // Log("pde_addr is: 0x%x", pde_addr);
  // // 读数据，判断物理页是否可用
  // uint32_t pde = paddr_read(pde_addr, 4);
  // assert((pde>>31)&0x1);

  // 计算页表的物理地址
  // uint32_t pte_addr = (pde & 0xfffff000) + (pte_index<<2);
  // uint32_t pte = paddr_read(pte_addr, 4);
  // if (pde.)


  // Log("dic_addr is: 0x%x", pde_addr);

  assert(0);

  return 0;

}

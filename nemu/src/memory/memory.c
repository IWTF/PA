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
  if(cpu.cr0.paging) {            // 判断是否开启了分页机制
      if (PT_SIZE < len) {        // 特殊情况： 数据位于两个页
          /* this is a special case, you can handle it later. */
          assert(0);
      }
      else {                      // 虚拟地址到物理地址的转换，返回
          paddr_t paddr = page_translate(addr, false);
          return paddr_read(paddr, len);
      }
  }
  else
      return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  // paddr_write(addr, len, data);
  if(cpu.cr0.paging) {            // 判断是否开启了分页机制
      if (PT_SIZE < len) {        // 特殊情况： 数据位于两个页
          /* this is a special case, you can handle it later. */
          assert(0);
      }
      else {                      // 虚拟地址到物理地址的转换，返回
          paddr_t paddr = page_translate(addr, true);
          return paddr_write(paddr, len, data);
      }
  }
  else
      return paddr_write(addr, len, data);
}

// 自定义page_translate()函数
paddr_t page_translate(vaddr_t vaddr, bool is_write) {
  // 若没有开启分页机制，直接返回
  if (cpu.cr0.paging == 0)
    return vaddr;

  // Log("vaddr is: 0x%x", vaddr);
  // Log("CR3 page_directory_base is: 0x%x", cpu.cr3.page_directory_base);

  // 获取页目录索引，页表索引，页内偏移
  uint32_t pde_index = (vaddr>>22)&0x3ff;
  uint32_t pte_index = (vaddr>>12)&0x3ff;
  uint32_t off = vaddr & 0xfff;
  // Log("页目录索引 is: 0x%x", pde_index);
  // Log("页表索引 is: 0x%x", pte_index);
  // Log("页内偏移 is: 0x%x", off);


  // 查页目录,获取页表基址
  uint32_t pde_base = cpu.cr3.page_directory_base;
  uint32_t pde = (pde_base<<12) + (pde_index<<2);
  uint32_t pte_base = paddr_read(pde, 4);
  assert(pte_base & 0x1);
  // Log("pde is 0x%x,  pde val is: 0x%x", pde, pte_base);

  // 查页表，获取页框号
  uint32_t pte = (pte_base & 0xfffff000) + (pte_index<<2);
  uint32_t pte_val = paddr_read(pte, 4);
  // Log("pte is 0x%x,  pte val is: 0x%x", pte, pte_val);
  assert(pte_val & 0x1);

  // 获取accessed位
  uint32_t pde_accessed = (pte_base>>5) & 0x1;
  // uint32_t pte_accessed = (pte_val>>5) & 0x1;
  // uint32_t pte_dirty = (pte_val>>6) & 0x1;

  // 检验 PDE 的 accessed 位
  if (pde_accessed == 0) {
    pde_accessed = 1;
    paddr_write(pde, 4, pde_base + (pde_accessed<<5));
  }

  // // 检验 PTE 的 accessed 位
  // if (pte_accessed == 0 || (pte_dirty == 0 && is_write)) {
  //   pte_accessed = 1;
  //   pte_dirty = 1;
  //   pte_val = pte_val + pte_accessed + pte_dirty;
  //   paddr_write(pte, 4, pte_val);
  // }

  uint32_t paddr = (pte_val & 0xfffff000) + off;
  // Log("paddr is: 0x%x", paddr);

  return paddr;

}

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
    bool overflow_page = (addr >> 12) != ((addr + len - 1) >> 12);
    if (overflow_page) {        // 特殊情况： 数据位于两个页
      // this is a special case, you can handle it later. 
       
      // 前一页的大小
      uint32_t pre_page_size = 0x1000 - (addr & 0xfff);
      paddr_t pre_paddr = page_translate(addr, false);

      // 后一页的虚拟地址
      uint32_t nxt_page = addr + pre_page_size;
      paddr_t nxt_paddr = page_translate(nxt_page, false);
     
      
      // 前后两页中读出的数据
      uint32_t pre_data = paddr_read(pre_paddr, pre_page_size);
      uint32_t nxt_data = paddr_read(nxt_paddr, (len-pre_page_size));

      // 小端拼接并返回
      return pre_data | (nxt_data<<(pre_page_size*8)); 
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

    // 判断首尾是否在一页内
    bool overflow_page = (addr >> 12) != ((addr + len - 1) >> 12);
    
    if (overflow_page) {        // 特殊情况： 数据位于两个页
      /* this is a special case, you can handle it later. */
       
      // 前一页的大小
      uint32_t pre_page_size = 0x1000 - (addr & 0xfff);
      paddr_t pre_paddr = page_translate(addr, false);

      // 后一页的虚拟地址
      uint32_t nxt_page = addr + pre_page_size;
      paddr_t nxt_paddr = page_translate(nxt_page, false);
     
      
      // 将data分成两部分,f分别写入内存
      uint32_t nxt_data = data >> (32 - pre_page_size * 8);
      paddr_write(pre_paddr, pre_page_size, data);
      paddr_write(nxt_paddr, (len - pre_page_size), nxt_data);
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
  PDE pde_obj;
  pde_obj.val = paddr_read(pde, 4);
  assert(pde_obj.present);
  // Log("pde is 0x%x,  pde val is: 0x%x", pde, pde_obj.val);

  // 查页表，获取页框号
  uint32_t pte = (pde_obj.val & 0xfffff000) + (pte_index<<2);
  PTE pte_obj;
  pte_obj.val = paddr_read(pte, 4);
  // Log("pte is 0x%x,  pte val is: 0x%x", pte, pte_obj.val);
  assert(pte_obj.present);


  // 检验 PDE 的 accessed 位
  if (!pde_obj.accessed) {
    pde_obj.accessed = 1;
    paddr_write(pde, 4, pde_obj.val);
  }

  // // 检验 PTE 的 accessed 位
  if (!pte_obj.accessed || (!pte_obj.dirty == 0 && is_write)) {
    pte_obj.accessed = 1;
    pte_obj.dirty = 1;
    paddr_write(pte, 4, pte_obj.val);
  }

  uint32_t paddr = (pte_obj.val & 0xfffff000) | off;
  // Log("paddr is: 0x%x", paddr);

  return paddr;

}

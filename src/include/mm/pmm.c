
// This file is a part of MRNIU/SimpleKernel (https://github.com/MRNIU/SimpleKernel).

// pmm.c for MRNIU/SimpleKernel.

#include "mm/pmm.h"
#include "assert.h"

// 物理页帧数组长度
static uint32_t phy_pages_count;

// 可用物理内存页起始地址
static uint32_t pmm_addr_start;

// 可用物理内存页结束地址
static uint32_t pmm_addr_end;

// 物理内存页面管理的栈
static uint32_t pmm_stack[PAGE_MAX_SIZE+1];

// 物理内存管理的栈指针
static uint32_t pmm_stack_top;

void pmm_init(struct multiboot_tag *tag) {
		multiboot_memory_map_entry_t * mmap;
		mmap = ((struct multiboot_tag_mmap *) tag)->entries;
		for (; (uint8_t *) mmap< (uint8_t *) tag + tag->size;
		     mmap = (multiboot_memory_map_entry_t *)((unsigned long) mmap +
		                                             ((struct multiboot_tag_mmap *) tag)->entry_size)) {
				// 如果是可用内存
				if ((unsigned) mmap->type == MULTIBOOT_MEMORY_AVAILABLE
				    && (unsigned) (mmap->addr & 0xffffffff) == 0x100000) {
						// 把内核结束位置到结束位置的内存段，按页存储到页管理栈里
						uint32_t page_addr = (mmap->addr);
						uint32_t length = (mmap->len);
						while (page_addr < length && page_addr <= PMM_MAX_SIZE) {
								pmm_free_page(page_addr);
								page_addr += PMM_PAGE_SIZE;
								phy_pages_count++;
						}
				}
		}
		printk_color(COL_INFO, "[INFO] ");
		printk ("pmm_init\n");
		return;
}

uint32_t pmm_alloc_page(){
		assert(pmm_stack_top != 0);
		uint32_t page = pmm_stack[pmm_stack_top--];
		return page;
}

void pmm_free_page(uint32_t p){
		assert(pmm_stack_top != PAGE_MAX_SIZE);
		pmm_stack[++pmm_stack_top] = p;
		return;
}

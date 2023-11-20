#ifndef PAGING_H
#define PAGING_H

#include "types.h"

#define PAGESIZE 4096
#define PAGE(i) ((i) / 4096)
#define set_page_frame_used(page) mem_bitmap[((u32)page) / 8] |= (1 << (((u32)page) % 8))
#define release_page_frame(p_addr) mem_bitmap[((u32)p_addr / PAGESIZE) / 8] &= ~(1 << (((u32)p_addr / PAGESIZE) % 8))

#define RAM_MAXPAGE 0x10000

#define USER_OFFSET 0x40000000
#define STACK_OFFSET 0x40001fff

char *get_page_frame(void);
void init_mm(void);
void init_bitmap(void);
u32 *pd_create(u32 phys_addr, u32 pages);

#endif

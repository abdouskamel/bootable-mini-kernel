#include "paging.h"

u8 mem_bitmap[RAM_MAXPAGE / 8] = {0};
static u32 *pd0;
static u32 *pt0;

char *get_page_frame(void)
{
    int byte, bit;
    int page = -1;

    for (byte = 0; byte < RAM_MAXPAGE / 8; byte++)
        if (mem_bitmap[byte] != 0xFF)
            for (bit = 0; bit < 8; bit++)
                if (!(mem_bitmap[byte] & (1 << bit)))
                {
                    page = 8 * byte + bit;
                    set_page_frame_used(page);
                    return (char *)(page * PAGESIZE);
                }
    return (char *)-1;
}

void init_mm(void)
{
    init_bitmap();

    int i;
    pd0 = (u32 *)get_page_frame();
    pt0 = (u32 *)get_page_frame();

    // Init page directory 
    *pd0 = ((u32)pt0) | 0x03;
    for (i = 1; i < 1024; ++i)
        pd0[i] = 0;

    // Init page table 
    u32 page_addr = 0;
    for (i = 0; i < 1024; ++i)
    {
        pt0[i] = page_addr | 0x03;
        page_addr += 4096;
    }

    // Activate pagination 
    asm("mov %0, %%eax\n"
        "mov %%eax, %%cr3\n"
        "mov %%cr0, %%eax\n"
        "or $0x80000000, %%eax\n"
        "mov %%eax, %%cr0\n" ::"m"((u32)pd0));
}

void init_bitmap(void)
{
    int i;

    for (i = 0; i < RAM_MAXPAGE / 8; i++)
        mem_bitmap[i] = 0;

    // Pages for the kernel 
    for (i = PAGE(0); i < PAGE(0x20000); ++i)
        set_page_frame_used(i);

    // Pages for hardware 
    for (i = PAGE(0xa0000); i < PAGE(0x100000); ++i)
        set_page_frame_used(i);
}

u32 *pd_create(u32 phys_addr, u32 pages)
{
    u32 *pd, *pt;
    int i;

    pd = (u32 *)get_page_frame();
    for (i = 0; i < 1024; ++i)
        pd[i] = 0;

    pt = (u32 *)get_page_frame();
    for (i = 0; i < 1024; ++i)
        pt[i] = 0;

    // Kernel space
    pd[0] = pd0[0];
    pd[0] |= 0x03;

    // User space
    pd[USER_OFFSET >> 22] = ((u32)pt) | 0X07;
    pt[0] = 0x100000 | 0x07;

    for (i = 0; i < pages; ++i)
    {
        pt[i] = phys_addr | 0x07;
        phys_addr += PAGESIZE;
    }

    return pd;
}

#ifndef GDT_H
#define GDT_H

#include "types.h"

#define GDT_BASE 0x800
#define GDT_SIZE 8

struct gdt_entry
{
    u16 limit_0_15;
    u16 base_0_15;
    u8 base_16_23;
    u8 access;
    u8 limit_16_19 : 4;
    u8 other : 4;
    u8 base_24_31;

} __attribute__((packed));

struct gdtr
{
    u16 limit;
    u32 base;

} __attribute__((packed));

struct tss
{
    u16 previous_task, __previous_task_unused;
    u32 esp0;
    u16 ss0, __ss0_unused;
    u32 esp1;
    u16 ss1, __ss1_unused;
    u32 esp2;
    u16 ss2, __ss2_unused;
    u32 cr3;
    u32 epid, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    u16 es, __es_unused;
    u16 cs, __cs_unused;
    u16 ss, __ss_unused;
    u16 ds, __ds_unused;
    u16 fs, __fs_unused;
    u16 gs, __gs_unused;
    u16 ldt_selector, __ldt_selector_unused;
    u16 debug_flags, io_map;

} __attribute__((packed));

void init_gdt(void);

#endif

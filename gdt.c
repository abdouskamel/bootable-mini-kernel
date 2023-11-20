#include "gdt.h"
#include "lib.h"
#include "screen.h"
#include "paging.h"

struct gdtr gdtr;
struct tss def_tss;

static void init_gdt_desc(u32 base, u32 limit, u8 access, u8 other, struct gdt_entry *gdte)
{
    gdte->limit_0_15 = (limit & 0xffff);
    gdte->base_0_15 = (base & 0xffff);
    gdte->base_16_23 = ((base >> 16) & 0xff);
    gdte->access = access;
    gdte->limit_16_19 = ((limit >> 16) & 0xf);
    gdte->other = other & 0xf;
    gdte->base_24_31 = ((base >> 24) & 0xff);
}

void init_gdt(void)
{
    struct gdt_entry gdt[GDT_SIZE];

    // null
    init_gdt_desc(0x0, 0x0, 0x0, 0x0, &gdt[0]);
    // code
    init_gdt_desc(0x0, 0xfffff, 0b10011010, 0b1100, &gdt[1]);
    // data
    init_gdt_desc(0x0, 0xfffff, 0b10010010, 0b1100, &gdt[2]);
    // stack
    init_gdt_desc(0x0, 0x0, 0b10010110, 0b1100, &gdt[3]);

    // ucode
    init_gdt_desc(0x0, 0xfffff, 0b11111110, 0x0d, &gdt[4]);
    // udata
    init_gdt_desc(0x0, 0xfffff, 0b11110010, 0x0d, &gdt[5]);
    // ustack
    init_gdt_desc(0x0, 0x0, 0b11110110, 0x0d, &gdt[6]);

    // TSS descriptor
    init_gdt_desc((u32)&def_tss, sizeof(struct tss) - 1, 0b10001001, 0x0, &gdt[7]);

    gdtr.limit = GDT_SIZE * 8;
    gdtr.base = GDT_BASE;

    memcpy((char *)GDT_BASE, (char *)gdt, GDT_SIZE * 8);
    asm("lgdt (gdtr)");

    // Init tss
    def_tss.debug_flags = 0x0;
    def_tss.io_map = 0x0;
    def_tss.esp0 = 0x20000;
    def_tss.ss0 = 0x18;
    asm("mov $0x38, %ax\n"
        "ltr %ax");

    asm("mov $0x10, %ax\n"
        "mov %ax, %ds\n"
        "mov %ax, %es\n"
        "mov %ax, %fs\n"
        "mov %ax, %gs\n"
        "ljmp $0x08, $next\n"
        "next: \n");
}

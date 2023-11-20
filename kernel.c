#include "screen.h"
#include "gdt.h"
#include "idt.h"
#include "asm.h"
#include "paging.h"
#include "process.h"

extern struct tss def_tss;
extern struct process *current;
extern int n_proc;

void main(void);

void kstart(void)
{
    screen_scroll_up(25);

    init_idt();
    screen_puts("IDT configured !\n");

    init_pic();
    screen_puts("PIC configured !\n");

    init_gdt();
    screen_puts("GDT loaded !\n");

    asm("mov $0x18, %ax\n"
        "mov %ax, %ss\n"
        "mov $0x20000, %esp\n");

    main();
}

void task1(void)
{
    char *str = (char *)USER_OFFSET + 100;
    str[0] = 't';
    str[1] = 'a';
    str[2] = 's';
    str[3] = 'k';
    str[4] = '1';
    str[5] = '\n';
    str[6] = '\0';
    asm("mov $1, %%eax; mov %0, %%ebx; int $0x30" ::"m"(str));

    while (1)
        ;
}

void task2(void)
{
    char *str = (char *)USER_OFFSET + 100;
    str[0] = 't';
    str[1] = 'a';
    str[2] = 's';
    str[3] = 'k';
    str[4] = '2';
    str[5] = '\n';
    str[6] = '\0';
    asm("mov $1, %%eax; mov %0, %%ebx; int $0x30" ::"m"(str));

    while (1)
        ;
}

void task3(void)
{
    char *str = (char *)USER_OFFSET + 100;
    str[0] = 't';
    str[1] = 'a';
    str[2] = 's';
    str[3] = 'k';
    str[4] = '3';
    str[5] = '\n';
    str[6] = '\0';
    asm("mov $1, %%eax; mov %0, %%ebx; int $0x30" ::"m"(str));

    while (1)
        ;
}

void main(void)
{
    init_mm();
    screen_puts("Pagination enabled !\n");

    load_task((u32 *)0x100000, (u32 *)task1, 0x2000);
    load_task((u32 *)0x102000, (u32 *)task2, 0x2000);
    load_task((u32 *)0x104000, (u32 *)task3, 0x2000);

    sti;
    while (1)
        ;
}

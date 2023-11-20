#include "idt.h"
#include "screen.h"
#include "asm.h"
#include "lib.h"
#include "kbd.h"
#include "process.h"

#define IDT_BASE 0x0

extern void _asm_exc_pf(void);
extern void _asm_irq_def(void);
extern void _asm_irq_0(void);
extern void _asm_irq_1(void);
extern void _asm_irq_sys(void);

struct idtr idtr;

void init_idt_desc(u16 seg, u32 offset, u16 type, struct idt_entry *idte)
{
    idte->offset_0_15 = (offset & 0xffff);
    idte->seg = seg;
    idte->type = type;
    idte->offset_16_31 = ((offset >> 16) & 0xffff);
}

void init_idt(void)
{
    struct idt_entry idt[IDT_SIZE];

    int i;
    for (i = 0; i < IDT_SIZE; ++i)
        init_idt_desc(0x08, (u32)_asm_irq_def, INT_GATE, &idt[i]);

    init_idt_desc(0x08, (u32)_asm_exc_pf, INT_GATE, &idt[PAGE_FAULT_IN]);
    init_idt_desc(0x08, (u32)_asm_irq_0, INT_GATE, &idt[CLOCK_INT]);
    init_idt_desc(0x08, (u32)_asm_irq_1, INT_GATE, &idt[KEYBOARD_INT]);
    init_idt_desc(0x08, (u32)_asm_irq_sys, TRAP_GATE, &idt[SYSCALLS_INT]);

    memcpy((char *)IDT_BASE, (char *)idt, IDT_SIZE * 8);

    idtr.limit = IDT_SIZE * 8;
    idtr.base = IDT_BASE;

    asm("lidt (idtr)");
}

void init_pic(void)
{
    // ICW 1
    outb(0x20, 0x11);
    outb(0xa0, 0x11);

    // ICW 2
    outb(0x21, 0x20);
    outb(0xa1, 0x70);

    // ICW 3
    outb(0x21, 0x04);
    outb(0xa1, 0x02);

    // ICW 4
    outb(0x21, 0x01);
    outb(0xa1, 0x01);
}

void isr_pf_int(void)
{
    screen_puts("That's a page fault !\n");
    asm("hlt");
}

void isr_def_int(void)
{
    screen_puts("That's a default interrupt handler.\n");
}

void isr_clock_int(void)
{
    schedule();
}

char *kbd_buf;
int kbd_buf_pos = 0;
static int input_mode = 0;

void isr_kbd_int(void)
{
    uchar i;
    static int lshift_enable = 0;
    static int rshift_enable = 0;
    static int alt_enable = 0;
    static int ctrl_enable = 0;

    // Wait for the buffer to be full
    do
    {
        inb(0x64, i);
    } while ((i & 0x01) == 0);

    inb(0x60, i);

    if (i <= 0x80)
    {
        switch (i)
        {
        case 0x2a:
            lshift_enable = 1;
            break;

        case 0x36:
            rshift_enable = 1;
            break;

        case 0x1d:
            ctrl_enable = 1;
            break;

        case 0x38:
            alt_enable = 1;
            break;

        default:
            i = kbdmap[(i - 1) * 4 + (lshift_enable || rshift_enable)];
            screen_putc(i);
            if (input_mode)
                kbd_buf[kbd_buf_pos++] = i;
        }
    }

    else
    {
        i -= 0x80;
        switch (i)
        {
        case 0x2a:
            lshift_enable = 0;
            break;

        case 0x36:
            rshift_enable = 0;
            break;

        case 0x1d:
            ctrl_enable = 0;
            break;

        case 0x38:
            alt_enable = 0;
            break;
        }
    }
}

void set_kbd_in(char *buffer)
{
    kbd_buf = buffer;
    kbd_buf_pos = 0;
    input_mode = 1;
}

void set_kbd_no_in(void)
{
    input_mode = 0;
}

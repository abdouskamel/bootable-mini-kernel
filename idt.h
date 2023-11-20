#ifndef IDT_H
#define IDT_H

#include "types.h"

#define INT_GATE 0b1000111000000000
#define TRAP_GATE 0b1110111100000000
#define IDT_SIZE 256

#define PAGE_FAULT_IN 0x0e
#define CLOCK_INT 0x20
#define KEYBOARD_INT 0x21
#define SYSCALLS_INT 0x30

struct idt_entry
{
    u16 offset_0_15;
    u16 seg;
    u16 type;
    u16 offset_16_31;

} __attribute__((packed));

struct idtr
{
    u16 limit;
    u32 base;

} __attribute__((packed));

void init_idt_desc(u16 seg, u32 offset, u16 type, struct idt_entry *idte);
void init_idt(void);
void init_pic(void);

void isr_def_int(void);
void isr_clock_int(void);
void isr_kbd_int(void);

void set_kbd_in(char *buffer);
void set_kbd_no_in(void);

#endif

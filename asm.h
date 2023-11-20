#ifndef ASM_H
#define ASM_H

#define cli asm("cli")
#define sti asm("sti")
#define outb(port, value) asm volatile("outb %%al, %%dx" :: "d" (port), "a" (value))
#define outbp(port, value) asm volatile("outb %%al, %%dx; jmp tmp; tmp:" :: "d" (port), "a" (value))
#define inb(port, value) asm volatile("inb %%dx, %%al" : "=a" (value) : "d" (port))

#endif

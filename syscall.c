#include "types.h"
#include "screen.h"
#include "gdt.h"
#include "idt.h"
#include "asm.h"

extern char *kbd_buf;
extern int kbd_buf_pos;

void do_syscall(int sys_num)
{
    char *str_ptr;
    int i;

    switch (sys_num)
    {
    case 1:
        asm("mov 0x2c(%%ebp), %%eax\n"
            "mov %%eax, %0\n" : "=m"(str_ptr) :);

        cli;
        screen_puts(str_ptr);
        sti;
        break;

    case 2:
        asm("mov 0x2c(%%ebp), %%eax\n"
            "mov %%eax, %0\n" : "=m"(str_ptr) :);

        set_kbd_in(str_ptr);

        while (kbd_buf[kbd_buf_pos - 1] != '\n')
            asm("hlt");

        kbd_buf[kbd_buf_pos] = '\0';
        set_kbd_no_in();
        break;

    default:
        screen_puts("Unknown syscall !\n");
    }
}

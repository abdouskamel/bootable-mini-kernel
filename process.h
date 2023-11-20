#ifndef PROCESS_H
#define PROCESS_H

#include "types.h"

#define KERNELMODE 0
#define USERMODE 1

struct process
{
    u32 pid;
    struct
    {
        u32 eax, ecx, edx, ebx;
        u32 esp, ebp, esi, edi;
        u32 eip, eflags;
        u16 cs, ss, ds, es, fs, gs;
        u32 cr3;

    } regs __attribute__((packed));

    struct
    {
        u32 ss0, esp0;
    } kstack __attribute__((packed));

} __attribute__((packed));

void switch_to_task(int n, int mode);
void schedule(void);

void load_task(u32 *phys_addr, u32 *task, u32 size);

#endif

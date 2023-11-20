#include "process.h"
#include "gdt.h"
#include "lib.h"
#include "paging.h"

extern u8 mem_bitmap[RAM_MAXPAGE / 8];

struct process *current = 0;
struct process p_list[3];
int n_proc = 0;

extern struct tss def_tss;
extern void do_switch(void);

void switch_to_task(int n, int mode)
{
    u32 kesp, eflags, esp, eip;
    u16 kss, ss, cs;

    current = &p_list[n];
    def_tss.ss0 = current->kstack.ss0;
    def_tss.esp0 = current->kstack.esp0;

    esp = current->regs.esp;
    eip = current->regs.eip;

    ss = current->regs.ss;
    cs = current->regs.cs;
    eflags = (current->regs.eflags | 0x200) & 0xFFFFBFFF;

    if (mode == USERMODE)
    {
        kss = current->kstack.ss0;
        kesp = current->kstack.esp0;
    }

    else
    {
        kss = current->regs.ss;
        kesp = current->regs.esp;
    }

    asm("mov %0, %%ax\n"
        "mov %%ax, %%ss\n"
        "mov %1, %%esp\n"
        "cmp %[KMODE], %[mode]\n"
        "je next\n"
        "push %2\n"
        "push %3\n"
        "next:\n"
        "push %4\n"
        "push %5\n"
        "push %6\n"
        "push %7\n"
        "ljmp $0x08, $do_switch" ::
            "m"(kss),
        "m"(kesp),
        "m"(ss),
        "m"(esp),
        "m"(eflags),
        "m"(cs),
        "m"(eip),
        "m"(current),
        [KMODE] "i"(KERNELMODE),
        [mode] "g"(mode));
}

void schedule(void)
{
    struct process *p;
    u32 *stack_ptr;

    asm("mov (%%ebp), %%eax; mov %%eax, %0" : "=m"(stack_ptr) :);

    if (current == 0 && n_proc > 0)
        switch_to_task(0, USERMODE);

    else if (n_proc <= 1)
        return;

    else
    {
        current->regs.eflags = stack_ptr[16];
        current->regs.cs = stack_ptr[15];
        current->regs.eip = stack_ptr[14];
        current->regs.eax = stack_ptr[13];
        current->regs.ecx = stack_ptr[12];
        current->regs.edx = stack_ptr[11];
        current->regs.ebx = stack_ptr[10];
        current->regs.ebp = stack_ptr[8];
        current->regs.esi = stack_ptr[7];
        current->regs.edi = stack_ptr[6];

        current->regs.ds = stack_ptr[5];
        current->regs.es = stack_ptr[4];
        current->regs.fs = stack_ptr[3];
        current->regs.gs = stack_ptr[2];

        /* La tache interrompue était en mode utilisateur */
        if (current->regs.cs != 0x08)
        {
            current->regs.esp = stack_ptr[17];
            current->regs.ss = stack_ptr[18];
        }
        
        // La tache interrompue était en mode superviseur
        else
        {
            current->regs.esp = stack_ptr[9] + 12;
            current->regs.ss = def_tss.ss0;
        }

        current->kstack.ss0 = def_tss.ss0;
        current->kstack.esp0 = def_tss.esp0;

        if (n_proc > current->pid + 1)
            p = &p_list[current->pid + 1];
        else
            p = &p_list[0];

        // Switch to usermode 
        if (p->regs.cs != 0x08)
            switch_to_task(p->pid, USERMODE);

        // Switch to kernelmode 
        else
            switch_to_task(p->pid, KERNELMODE);
    }
}

void load_task(u32 *phys_addr, u32 *task, u32 size)
{
    u32 page_base, pages;
    u32 *pd;
    int i;

    memcpy((char *)phys_addr, (char *)task, size);

    page_base = PAGE((u32)phys_addr);
    if (size % PAGESIZE == 0)
        pages = size / PAGESIZE;

    else
        pages = size / PAGESIZE + 1;

    for (i = 0; i < pages; ++i)
        set_page_frame_used(page_base + i);

    pd = pd_create((u32)phys_addr, pages);

    /* Create kernel stack for this process. */
    u32 kstack_base = (u32)get_page_frame();

    p_list[n_proc].regs.esp = STACK_OFFSET;
    p_list[n_proc].regs.eip = USER_OFFSET;

    p_list[n_proc].regs.ss = 0x33;
    p_list[n_proc].regs.cs = 0x23;

    p_list[n_proc].regs.ds = 0x2b;
    p_list[n_proc].regs.es = 0x2b;
    p_list[n_proc].regs.fs = 0x2b;
    p_list[n_proc].regs.gs = 0x2b;
    p_list[n_proc].regs.cr3 = (u32)pd;

    p_list[n_proc].kstack.ss0 = 0x18;
    p_list[n_proc].kstack.esp0 = kstack_base + PAGESIZE;

    p_list[n_proc].pid = n_proc;
    n_proc++;
}

.global _asm_exc_pf, _asm_irq_def, _asm_irq_0, _asm_irq_1, _asm_irq_sys

.macro save_context
    pushal
    push %ds
    push %es
    push %fs
    push %gs
    push %ax
    mov $0x10, %ax
    mov %ax, %ds
    pop %ax
.endm

.macro restore_context
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popal
.endm

_asm_exc_pf:
    save_context
    call isr_pf_int
    restore_context
    iret

_asm_irq_def:
    save_context
    call isr_def_int
    mov $0x20, %al
    out %al, $0x20
    restore_context
    iret

_asm_irq_0:
    save_context
    call isr_clock_int
    mov $0x20, %al
    out %al, $0x20
    restore_context
    iret

_asm_irq_1:
    save_context
    call isr_kbd_int
    mov $0x20, %al
    out %al, $0x20
    restore_context
    iret

_asm_irq_sys:
    save_context

    push %eax
    call do_syscall
    pop %eax

    restore_context
    iret

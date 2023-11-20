.global do_switch

do_switch:
    mov (%esp), %esi
    pop %eax

    push 4(%esi)
    push 8(%esi)
    push 12(%esi)
    push 16(%esi)
    push 24(%esi)
    push 28(%esi)
    push 32(%esi)
    pushw 48(%esi)
    pushw 50(%esi)
    pushw 52(%esi)
    pushw 54(%esi)

    mov $0x20, %al
    out %al, $0x20

    mov 56(%esi), %eax
    mov %eax, %cr3

    popw %gs
    popw %fs
    popw %es
    popw %ds
    pop %edi
    pop %esi
    pop %ebp
    pop %ebx
    pop %edx
    pop %ecx
    pop %eax

    iret

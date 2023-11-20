.code16
.global _start

.set K_SEGMENT_BASE, 0x100

.set K_NB_SECTORS, 0x30
.set K_SECTOR_POS, 0x02
.set BOOT_DRIVE, 0x0

.text
    jmp _start
    .include "utils.asm"

boot_msg: .ascii "Bootloader is here !\r\n"
          msg_len = . - boot_msg

_start:
    # Load %ds and %es.
    mov $0x07c0, %ax
    mov %ax, %ds
    mov %ax, %es

    # Load %ss and %sp
    mov $0x8000, %ax
    mov %ax, %ss
    mov $0xffff, %sp

    # Say that the bootloader is here
    mov $boot_msg, %bp
    mov $msg_len, %cx
    call puts

    # Load the kernel
    mov $BOOT_DRIVE, %dl
    xor %ax, %ax
    int $0x13

    # Load ES:BX with kernel position
    push %es

    mov $K_SEGMENT_BASE, %ax
    mov %ax, %es
    mov $0x0, %bx

    mov $K_NB_SECTORS, %al
    mov $0x0, %ch
    mov $0x0, %dh
    mov $K_SECTOR_POS, %cl

    mov $BOOT_DRIVE, %dl
    mov $0x02, %ah
    int $0x13

    pop %es

    # Let's load GDTR
    mov $gdt_size, %ax
    mov %ax, gdtptr

    xor %eax, %eax
    mov %ds, %ax
    shl $0x04, %eax
    add $gdt, %eax
    mov %eax, gdtptr + 0x02

    # Time to switch to protected mode
    cli
    lgdt gdtptr
    mov %cr0, %eax
    or $0x01, %eax
    mov %eax, %cr0

    # This odd statement is here to clear CPU cache.
    jmp next

next:
    # Init segment registers
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %es
    mov %ax, %ss
    mov $0x9F000, %esp

    # Let's far jump to the kernel
    ljmp $0x08, $0x1000

gdt: .byte 0, 0, 0, 0, 0, 0, 0, 0
gdt_cs: .byte 0xff, 0xff, 0x00, 0x00, 0x00, 0b10011011, 0b11011111, 0x00
gdt_ds: .byte 0xff, 0xff, 0x00, 0x00, 0x00, 0b10010011, 0b11011111, 0x00
gdt_size = . - gdt

gdtptr:
    .word 0 # gdt limit
    .int  0 # gdt base

.org 510, 0x90
.byte 0x55
.byte 0xaa

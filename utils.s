.code16
.global puts

.text

/*
 * Print a string in the screen using BIOS interrupts.
 * The string is passed with ES:BP, and its length is in CX.
 */
puts:
    push %cx
    push %bp

    # Get current cursor pos
    mov $0x00, %bh
    mov $0x03, %ah
    int $0x10

    pop %bp
    pop %cx

    # Print the string
    mov $0x07, %bl
    mov $0x00, %bh
    mov $0x01, %al
    mov $0x13, %ah
    int $0x10

    ret

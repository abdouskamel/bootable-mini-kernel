.global _start
.set GRUB_MAGIC, 0x1BADB002
.set GRUB_FLAGS, 0x00000001
.set GRUB_CHECKSUM, -(GRUB_MAGIC + GRUB_FLAGS)

.text
_start:
    jmp start

.align 4
.int GRUB_MAGIC
.int GRUB_FLAGS
.int GRUB_CHECKSUM

start:
    call kstart

    cli
    hlt

BITS 32

section .multiboot
align 4
    dd 0x1BADB002
    dd 0x00000007
    dd -(0x1BADB002 + 0x00000007)
    dd 0
    dd 1024
    dd 768
    dd 32

section .text
global _start
extern pyra_kernel_entry

_start:
    cli
    mov esp, stack_top
    push ebx
    push eax
    call pyra_kernel_entry
    add esp, 8
.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:

.section .init, "ax"
.global _start
_start:

    # init global pointer
    # temporarily disable linker relaxations as we can't
    # use gp relative addressing for setting gp itself
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop

    # Copy static data from ROM to RAM
    la      a0, __DATA_BEGIN__  # start of .data in RAM
    la      a1, _etext          # start of .data in ROM
    la      a2, _edata          # length of .data
    sub     a2, a2, a0
    call    memcpy

    # Clear the bss segment
    la      a0, __bss_start
    la      a2, _end
    sub     a2, a2, a0
    li      a1, 0
    call    memset

    # Init stack and frame pointers
    la sp, __stack_top
    add s0, sp, zero

    jal zero, main

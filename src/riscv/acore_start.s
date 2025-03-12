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

    # set trap handler
    la a0, trap_handler
    csrw mtvec, a0

    jal zero, main


trap_handler:
    # spill registers to memory (x4 is destroyed)
	li x4, 0x20000000
	sw x0, 0(x4)
	sw x1, 4(x4)
	sw x2, 8(x4)
	sw x3, 12(x4)
	sw x0, 16(x4)  # invalid
	sw x5, 20(x4)
	sw x6, 24(x4)
	sw x7, 28(x4)
	sw x8, 32(x4)
	sw x9, 36(x4)
	sw x10, 40(x4)
	sw x11, 44(x4)
	sw x12, 48(x4)
	sw x13, 52(x4)
	sw x14, 56(x4)
	sw x15, 60(x4)
	sw x16, 64(x4)
	sw x17, 68(x4)
	sw x18, 72(x4)
	sw x19, 76(x4)
	sw x20, 80(x4)
	sw x21, 84(x4)
	sw x22, 88(x4)
	sw x23, 92(x4)
	sw x24, 96(x4)
	sw x25, 100(x4)
	sw x26, 104(x4)
	sw x27, 108(x4)
	sw x28, 112(x4)
	sw x29, 116(x4)
	sw x30, 120(x4)
	sw x31, 124(x4)
    jal zero, trap_handler_c
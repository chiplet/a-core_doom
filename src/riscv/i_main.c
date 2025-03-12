/*
 * i_main.c
 *
 * Main entry point
 *
 * Copyright (C) 2021 Sylvain Munaut
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "doomdef.h"
#include "d_main.h"

// hack
#include "console.h"
#include "stdint.h"
#include "a-core.h"
#include "acore-uart.h"
#include "a-core-utils.h"
#include "a-core-csr.h"

// stackless print of 32-bit hexadecimal number
static inline void print_u32_hex(const uint32_t value) {
	for (int i = 0; i < 8; i++) {
		uint32_t shifted_hex = value >> (8-i-1)*4;
		uint8_t masked_hex = shifted_hex & 0xF;
		uint8_t ascii_digit = masked_hex + 48;
		if (ascii_digit > 57)
			ascii_digit += 7;
		*((volatile uint8_t*)(A_CORE_AXI4LUART+UART_TX_BYTE)) = ascii_digit;
	}
	*((volatile uint8_t*)(A_CORE_AXI4LUART+UART_TX_BYTE)) = '\n';
}

// static inline void spill_regs() {
// 	asm("li x4, 0x20000000");
// 	asm("sw x0, 0(x4)");
// 	asm("sw x1, 4(x4)");
// 	asm("sw x2, 8(x4)");
// 	asm("sw x3, 12(x4)");
// 	asm("sw x0, 16(x4)"); // invalid
// 	asm("sw x5, 20(x4)");
// 	asm("sw x6, 24(x4)");
// 	asm("sw x7, 28(x4)");
// 	asm("sw x8, 32(x4)");
// 	asm("sw x9, 36(x4)");
// 	asm("sw x10, 40(x4)");
// 	asm("sw x11, 44(x4)");
// 	asm("sw x12, 48(x4)");
// 	asm("sw x13, 52(x4)");
// 	asm("sw x14, 56(x4)");
// 	asm("sw x15, 60(x4)");
// 	asm("sw x16, 64(x4)");
// 	asm("sw x17, 68(x4)");
// 	asm("sw x18, 72(x4)");
// 	asm("sw x19, 76(x4)");
// 	asm("sw x20, 80(x4)");
// 	asm("sw x21, 84(x4)");
// 	asm("sw x22, 88(x4)");
// 	asm("sw x23, 92(x4)");
// 	asm("sw x24, 96(x4)");
// 	asm("sw x25, 100(x4)");
// 	asm("sw x26, 104(x4)");
// 	asm("sw x27, 108(x4)");
// 	asm("sw x28, 112(x4)");
// 	asm("sw x29, 116(x4)");
// 	asm("sw x30, 120(x4)");
// 	asm("sw x31, 124(x4)");
// }

// This function is invoked by trap_handler assembly routine that spills
// register file contents to memory and hands off control to this function
// for debug printing tasks.
void trap_handler_c()
{
	// Indicate we have entered the trap handler by enabling LED0
	*((volatile uint32_t*)(0x30000010)) = 0b01;
	console_printf("\nEXCEPTION\n");
	
	// Print exception source and cause
	// bootleg store mepc for later analysis (grab with jtag debugger)
	uint32_t mepc = csr_read(CSR_MEPC);
	// *((volatile uint32_t*)(0x20000000)) = mepc;
	// print_u32_hex(mepc);
	console_printf("mepc = 0x%08x\n", mepc);
	
	const char* mcauses[16] = {
		"Instruction address misaligned", "Instruction access fault",
		"Illegal instruction", "Breakpoint",
		"Load address misaligned", "Load access fault",
		"Store/AMO address misaligned", "Store/AMO access fault",
		"Environment call from U-mode", "Environment call from S-mode",
		"Reserved", "Environment call from M-mode",
		"Instruction page fault", "Load page fault",
		"Reserved", "Store/AMO page fault"
	};
	uint32_t mcause = csr_read(CSR_MCAUSE);
	// *((volatile uint32_t*)(0x20000004)) = mcause;
	// print_u32_hex(mcause);
	console_printf("mcause = 0x%08x : %s\n", mcause, mcauses[mcause]);

	// print register dump
	console_printf("\nREGISTER DUMP\n");
	const char* alt_names[32] = {
		"zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
		"s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
		"a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
		"s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
	};
	volatile uint32_t* spill_addr = (volatile uint32_t*)(0x20000000);
	for (int i = 0; i < 32; i++) {
		uint32_t x = *(spill_addr++);
		if (i != 4) {
			console_printf("x%d/%s : 0x%08x\n", i, alt_names[i], x);
		} else {
			console_printf("x%d/%s : UNKNOWN\n", i, alt_names[i]);
		}
	}

    // printf("mepc: 0x%08x\n", mepc);
	// *((volatile uint32_t*)(0x30000010)) = 0b1;
	for(;;);
}


int main(void)
{
	*((volatile uint32_t*)(0x30000010)) = 0;
	// HACK: hardcode uart init to place it in the beginning of text
	*((volatile uint32_t*)(A_CORE_AXI4LUART+UART_TX_CLK_THRESH)) = BAUDRATE;
	// send message for good luck
	// stackless
	// *((volatile uint8_t*)(A_CORE_AXI4LUART+UART_TX_BYTE)) = 'm';
	// *((volatile uint8_t*)(A_CORE_AXI4LUART+UART_TX_BYTE)) = 'a';
	// *((volatile uint8_t*)(A_CORE_AXI4LUART+UART_TX_BYTE)) = 'i';
	// *((volatile uint8_t*)(A_CORE_AXI4LUART+UART_TX_BYTE)) = 'n';
	// *((volatile uint8_t*)(A_CORE_AXI4LUART+UART_TX_BYTE)) = '\n';
	// stackful
	console_printf("main\n");

	// paniik! D::
	// asm("li a0,1");
	// asm("sw a0,0(a0)");

	D_DoomMain();
	return 0;
}

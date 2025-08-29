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
#include "config.h"
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


// This function is invoked by trap_handler assembly routine that spills
// register file contents to memory and hands off control to this function
// for debug printing tasks.
void trap_handler_c()
{
	*((volatile uint8_t*)(A_CORE_AXI4LUART+UART_TX_BYTE)) = 't';
	*((volatile uint8_t*)(A_CORE_AXI4LUART+UART_TX_BYTE)) = 'r';
	*((volatile uint8_t*)(A_CORE_AXI4LUART+UART_TX_BYTE)) = 'a';
	*((volatile uint8_t*)(A_CORE_AXI4LUART+UART_TX_BYTE)) = 'p';
	*((volatile uint8_t*)(A_CORE_AXI4LUART+UART_TX_BYTE)) = '\n';
	// for(;;);

	// Indicate we have entered the trap handler by enabling LED0
	*((volatile uint32_t*)(0x30000010)) = 0b01;
	console_printf("\nEXCEPTION\n");
	
	// Print exception source and cause
	// bootleg store mepc for later analysis (grab with jtag debugger)
	uint32_t mepc = csr_read(CSR_MEPC);
	// *((volatile uint32_t*)(0x20000000)) = mepc;
	// console_printf("mepc = ");
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
	// stackless
	// console_printf("mcause = ");
	// print_u32_hex(mcause);
	// console_printf("\n");
	// console_printf(mcauses[mcause]);
	// console_printf("\n");
	// for(;;);
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

void clean() {
    volatile uint8_t *framebuf = (volatile uint8_t *)VID_FB_BASE;
    for (int i = 0; i < 320 * 200; i++) {
        framebuf[i] = 0;
        delay(2);
    }
}


int main(void)
{
	*((volatile uint32_t*)(0x30000010)) = 2;
	// for (;;) {
	// 	*((volatile uint32_t*)(0x30000010)) = 3;
	// 	delay(10000000);
	// 	*((volatile uint32_t*)(0x30000010)) = 0;
	// 	delay(10000000);
	// }
	// init peripherals
    // init_uart((volatile uint32_t*)A_CORE_AXI4LUART, BAUDRATE);
	// // *((volatile uint32_t*)(A_CORE_AXI4LUART+UART_TX_CLK_THRESH)) = BAUDRATE;
    // *((volatile uint32_t*)(A_CORE_AXI4LUART + 16)) = BAUDRATE; // baud rate counter thereshold
	
	// // *((volatile uint32_t*)(0x30000010)) = 1;
	// // // HACK: hardcode uart init to place it in the beginning of text
	// // // send message for good luck
	// // // stackless

	// *((volatile uint8_t*)(A_CORE_AXI4LUART+UART_TX_BYTE)) = 'm';
	// *((volatile uint8_t*)(A_CORE_AXI4LUART+UART_TX_BYTE)) = 'a';
	// *((volatile uint8_t*)(A_CORE_AXI4LUART+UART_TX_BYTE)) = 'i';
	// *((volatile uint8_t*)(A_CORE_AXI4LUART+UART_TX_BYTE)) = 'n';
	// *((volatile uint8_t*)(A_CORE_AXI4LUART+UART_TX_BYTE)) = '\n';
	// // // stackful
	// console_printf("printf(main)\n");
	// // for(;;);

	// console_printf("hello world! 0x%08x\n", 0x12345678);
	// // for(;;);

	// Enable Video Controller
	*((volatile uint32_t*)(VID_CTRL_BASE)) = 1;

	// // test read from SPI flash ROM (wad is stored here)
	// for (int i = 0; i < 10; i++) {
	// 	uint8_t wad = *((volatile uint8_t*)(0x41000000 + i));
	// 	printf("wad[%d] = 0x%02x\n", i, wad);
	// 	// uint32_t wad = *((volatile uint32_t*)(0x41000000 + i*4));
	// 	// printf("wad[%d] = 0x%08x\n", 4*i, wad);
	// }
	// for(;;);

	// test HRAM
	// doesn't work when executing from HRAM tough :D
	// console_printf("writing to hram\n");
	// volatile uint32_t* hram_base = (volatile uint32_t*)0x40000000;
	// for (int i = 0; i < 128000; i++) {
	// 	hram_base[i] = i;
	// }

	// console_printf("reading from hram\n");
	// for (int i = 0; i < 128000; i++) {
	// 	uint32_t read_word = hram_base[i];
	// 	if (read_word != i) {
	// 		console_printf("error: i = %d, read = 0x%08x\n", i, read_word);
	// 	}
	// }
	// console_printf("done reading!\n");





	// // test video
	// Test Palette Memory
	uint32_t palette_base = VID_PAL_BASE;
	*((volatile uint32_t*)(palette_base + 0*4)) = 0;
	*((volatile uint32_t*)(palette_base + 1*4)) = 0xff;
	*((volatile uint32_t*)(palette_base + 2*4)) = 0xff00;
	*((volatile uint32_t*)(palette_base + 3*4)) = 0xff0000;

	clean(); 
	delay(100000);

	// int count = 0;
	// for(;;) {
	// 	uint32_t framebuf_addr = 0x50000000;
	// 	// *((volatile uint8_t*)(framebuf_addr + count++)) = count % 4;
	// 	delay(2);
	// 	if (count >= 320*200) {
	// 		count = 0;
	// 		// bootleg precision register dump
	// 		asm("li x1,1");
	// 		asm("sw x1,0(x1)");
	// 	}

	volatile uint8_t* framebuf = (volatile uint8_t*)VID_FB_BASE;
	for (int y = 0; y < 200; y++) {
		for (int x = 0; x < 320; x++) {
			framebuf[320*y + x] = (x+y) % 4;
			delay(1000);
		}
	}
	// }

	// paniik! D::
	// asm("li a0,1");
	// asm("sw a0,0(a0)");

	// for(;;);

	D_DoomMain();
	// return 0;
}

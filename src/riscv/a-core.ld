/*
 * A-Core linker script
 *
 * This file is used to configure how program
 * instructions and data will be mapped to memory
 *
 * The default linker script used by ld can be viewed with `ld --verbose`
 */

OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv", "elf32-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY {
    ROM (rx) : ORIGIN = PROGMEM_START, LENGTH = PROGMEM_LENGTH
    RAM (rwx)  : ORIGIN = DATAMEM_START, LENGTH = DATAMEM_LENGTH
}

SECTIONS
{
  . = PROGMEM_START;   /* address of first program instruction */
  .text : {
    __TEXT_BEGIN__ = .;
    *(.init)
    *(.text.init)
    *(.text .text.*)
    . = ALIGN(4);
  } > ROM
  PROVIDE (__etext = .);
  PROVIDE (_etext = .);
  PROVIDE (etext = .);

  /* .data and .sdata are loaded to RAM from ROM */
  .data           :
  {
    __DATA_BEGIN__ = .;
    *(.data .data.* .gnu.linkonce.d.*)
  } > RAM AT > ROM
  .sdata          :
  {
    __SDATA_BEGIN__ = .;
    *(.srodata.cst16) *(.srodata.cst8) *(.srodata.cst4) *(.srodata.cst2) *(.srodata .srodata.*)
    *(.sdata .sdata.* .gnu.linkonce.s.*)
  } > RAM AT > ROM
  _edata = .; PROVIDE (edata = .);
  . = .;

  /* rodata can live at ROM */
  .rodata : {
      *(.rodata .rodata.* .gnu.linkonce.r.*)
  } > ROM

  /* .bss and .sbss are initialized to zero */
  .sbss :
  {
    __bss_start = .;
    *(.dynsbss)
    *(.sbss .sbss.* .gnu.linkonce.sb.*)
    *(.scommon)
  } > RAM
  .bss : {
   *(.dynbss)
   *(.bss .bss.* .gnu.linkonce.b.*)
   *(COMMON)
   /* Align here to ensure that the .bss section occupies space up to
      _end.  Align after .bss to ensure correct alignment even if the
      .bss section disappears because there are no input sections.
      FIXME: Why do we need it? When there is no .bss section, we do not
      pad the .data section.  */
   . = ALIGN(. != 0 ? 32 / 8 : 1);
  } > RAM
  . = ALIGN(32 / 8);
  . = SEGMENT_START("ldata-segment", .);
  . = ALIGN(32 / 8);
  __BSS_END__ = .;
  _end = .; PROVIDE (end = .);
    .heap :
    {
        . = ALIGN(4);
        _heap_start = .;
    } >RAM


  /* 0x800 is a magic number for 12-bit signed offsets */
  __global_pointer$ = MIN(__SDATA_BEGIN__ + 0x800, MAX(__DATA_BEGIN__ + 0x800, __BSS_END__ - 0x800));

  PROVIDE(__stack_top = ORIGIN(RAM) + LENGTH(RAM));
}
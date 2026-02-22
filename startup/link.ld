OUTPUT_ARCH( "riscv" )
ENTRY(_start)

MEMORY {
  DRAM(rwx): ORIGIN = 0x80000000, LENGTH = 4096M
}

SECTIONS
{
  .text.init : {
    *(.text.init)
  }> DRAM
  .text ALIGN (0x1000) : {
	  *(.text)
  }> DRAM
  .rodata : {
    _global_pointer = .;
    *(.rodata)
    *(.srodata)
  }> DRAM
  .data ALIGN (0x1000) : {
    _sstack = .;
    . += 4K;
	  *(.data)
  }> DRAM
  .bss ALIGN (0x1000) : {
    _bss_start = .;
	  *(.bss)
    *(COMMON)
    . = ALIGN (0x8);
    _bss_end = .;
  }> DRAM
}

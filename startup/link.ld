OUTPUT_ARCH( "riscv" )
ENTRY(_start)
SECTIONS
{
  . = 0x80000000;
  .text.init : { *(.text.init) }
  .data ALIGN (0x1000) : {
	  *(.data)
  }
  .bss : {
	  *(.bss)
  }
  _end = .;
}

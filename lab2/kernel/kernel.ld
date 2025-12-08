OUTPUT_ARCH( "riscv" )
ENTRY( _entry )

SECTIONS
{
  /* QEMU 把内核加载到这个物理地址 */
  . = 0x80000000;

  /* 代码段 */
  .text : {
    *(.text .text.*)
    . = ALIGN(0x1000);
  }

  /* 只读数据段 */
  .rodata : {
    . = ALIGN(16);
    *(.srodata .srodata.*) /* do not need to distinguish this from .rodata */
    . = ALIGN(16);
    *(.rodata .rodata.*)
  }

  /* 数据段 */
  .data : {
    . = ALIGN(16);
    *(.sdata .sdata.*)
    . = ALIGN(16);
    *(.data .data.*)
  }

  /* BSS */
  .bss : {
    . = ALIGN(16);
    sbss = .;         /* 定义符号 sbss (Start BSS) */
    *(.sbss .sbss.*)
    . = ALIGN(16);
    *(.bss .bss.*)
    ebss = .;         /* 定义符号 ebss (End BSS) */
  }

  PROVIDE(end = .);
}
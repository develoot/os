OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS
{
	.text : ALIGN(0x08)
	{
		*(.text)
	}
	.data : ALIGN(0x08)
	{
		*(.data)
	}
	.rodata : ALIGN(0x08)
	{
		*(.rodata)
	}
	.bss : ALIGN(0x08)
	{
		*(COMMON)
		*(.bss)
	}
}

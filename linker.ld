ENTRY(_start)

SECTIONS
{
	. = 2M;

	_kbin_beg = .;

	.text ALIGN(4K):
	{
		KEEP(*(.multiboot))
		*(.text)
	}

	/* Read-only data. */
	.rodata ALIGN(4K):
	{
		*(.rodata)
		*(.rodata.*)
	}

	/* Read-write data (initialized) */
	.data ALIGN(4K):
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss ALIGN(4K):
	{
		*(COMMON)
		*(.bss)
	}

	_kbin_end = .;

	.comment :
	{
		*(.comment)
	}

	.rel :
	{
		*(.rel)
		*(.rel.*)
	}

	.got :
	{
		*(.got)
		*(.got.*)
	}

	.igot :
	{
		*(.igot)
		*(.igot.*)
	}

	.iplt :
	{
		*(.iplt)
		*(.iplt.*)
	}
}

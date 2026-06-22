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
		_debug_line_beg = .;
		*(.debug_line)
		_debug_line_end = .;
		_debug_str_beg = .;
		*(.debug_str)
		_debug_str_end = .;
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
}

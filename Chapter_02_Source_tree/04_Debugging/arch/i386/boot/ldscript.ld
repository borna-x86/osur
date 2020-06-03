/*! linker script for memory layout */

/* Its parsed as C before used in linking! */

ENTRY(arch_startup)

SECTIONS {
	.code 0x0 :
	{
		kernel_code_addr = .;

		/* instructions */
		*?/boot/startup.asm.o ( .text* )
		*?/boot/premjesti.o ( .text* )
	}
	.instrukcije 0x100000 : AT(0x10000)
	{
		*( .text* )
	}
	size_i = SIZEOF(.instrukcije);
	xxxx = 0x10000;

	.data 0x500000: AT(0x50000)
	{
		kernel_data_addr = .;

		/* read only data (constants), initialized global variables */
		* ( .rodata* .data* )
	}
	size_data = SIZEOF(.data);

	.bss 0x800000: AT(0x80000)
	{
		*( .bss* COMMON* )

		. = ALIGN (4096);
	}

	.stog_u_slici 0x900000: AT(0x90000)
	{
		*( .stog )
		. = ALIGN (4096);
	}
	/*
	 * what with other sections generated with gcc (various versions)?
	 * if optimizing for size discard them with: /DISCARD/ : { *(*) }
	 * if debugging with qemu/gdb they must be included
	 * - they can be included implicitly, without declaring them here
	 * - but they will be put behind "kernel_end_addr" so its best to use
	 *   multiboot loader (QEMU is that)
	 */

#ifndef DEBUG
	/DISCARD/ : { *(*) }
#endif
	kernel_end_addr = .;
}

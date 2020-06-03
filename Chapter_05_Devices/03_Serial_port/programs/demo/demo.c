/*! Allocator demo program */

#include <stdio.h>
#include <api/prog_info.h>
#include <kernel/memory.h>

int demo ()
{
	printf("Starting DEMO program\n");
	printf("kprints were added to allocator, to display which memory pool segment was used\n");
	printf("the kernel memory pool is used to test malloc/free, since it is already initialized\n");

	printf("Allocating 55 bytes\n");
	void* mem1 = kmalloc(55);


	printf("Allocating 500 bytes\n");
	void* mem2 = kmalloc(500);

	printf("Allocating 600 bytes\n");
	void* mem3 = kmalloc(600);

	printf("Freeing 55 bytes\n");
	kfree(mem1);

	printf("Freeing 500 bytes\n");
	kfree(mem2);


	printf("Freeing 600 bytes\n");
	kfree(mem3);

	return 0;
}

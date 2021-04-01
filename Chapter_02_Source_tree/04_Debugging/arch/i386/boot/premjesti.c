#include <types/basic.h>

void cpy(char *od, char* kamo, size_t size);

void premjesti()
{
	extern size_t size_instrukcije[];
	extern size_t size_data[];
	//extern size_t size_bss[];
	//extern size_t size_stack[];


	cpy((char *) 0x10000, (char *) 0x200000, (size_t) size_instrukcije);
	cpy((char *) 0x40000, (char *) 0x400000, (size_t) size_data);
	//cpy((char *) 0x80000, (char *) 0x800000, (size_t) size_bss);
	//cpy((char *) 0x90000, (char *) 0x900000, (size_t) size_stack);
}

void cpy(char *od, char* kamo, size_t size) {
	size_t i;

	for(i = 0; i < size; ++i) {
		*kamo++ = *od++;
	}

}
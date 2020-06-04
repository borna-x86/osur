#include <types/basic.h>

void cpy(char *od, char* kamo, size_t size);

void premjesti()
{
	//extern char size_data;
	//extern char size_i;
	//size_t size_d = (size_t) &size_data;
	char *od = (char *) 0x100000;
	char *kamo = (char *) 0x200000;
	
	extern char end_address;
	size_t size = (size_t)&end_address - 0x200000;

	cpy(od, kamo, size);
 

	//od = (char *) 0x10000;
	//kamo = (char *) 0x100000;
	//size_d = (size_t) &size_i;

	//cpy(od, kamo, size_d);

}

void cpy(char *od, char* kamo, size_t size) {
	size_t i;

	for(i = 0; i < size; ++i) {
		*kamo++ = *od++;
	}

}
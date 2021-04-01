#include <stdio.h>
#include <api/prog_info.h>

int hello_world ()
{
   printf ( "Example program: [%s:%s]\n%s\n\n", __FILE__, __FUNCTION__,
       hello_world_PROG_HELP );

   printf ( "Hello World!\n" );
   extern char* kernel_data_addr[];
   printf("kda = %x\n", (char*) kernel_data_addr);
   extern char* bss_addr[];
   extern int skrivena_varijabla;
   printf("bsa = %x\n",(char*) bss_addr);
   printf("ska = %x\n", &skrivena_varijabla);
   MINUS();

   return 0;
}

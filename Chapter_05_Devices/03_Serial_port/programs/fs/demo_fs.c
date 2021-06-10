/*! Keyboard api testing */

#include <stdio.h>
#include <time.h>
#include <api/prog_info.h>

int disk_demo ()
{
	int fd = open("file:test", O_CREAT | O_WRONLY, 0);
	printf("fd=%d\n", fd);
	int retval = write(fd, "neki tekst", 11);
	printf("retval=%d\n", retval);
	retval = close(fd);
	printf("retval=%d\n", retval);

	fd = open("file:test", O_RDONLY, 0);
	printf("fd=%d\n", fd);
	char buff[11];
	retval = read(fd, buff, 11);
	printf("retval=%d\n", retval);
	printf("buff=%s\n\n", buff);


    printf("testing with 600 \'a\' chars into file (to check if block boundaries are handled OK)\n");
    fd = open("file:datoteka2", O_CREAT | O_WRONLY, 0);
    printf("fd=%d\n");
    char buff1[600];
    int i;
    for(i = 0; i < 600; ++i) {
        buff1[i] = 'a';
    }

    retval =  write(fd, buff1, 600);
    printf("written %d chars\n", retval);
    printf("close retval: %d\n", close(fd));

    fd = open("file:datoteka2", O_RDONLY, 0);
    printf("open fd=%d\n", fd);
    char buff2[600];
    retval = read(fd, buff2, 600);
    printf("read returned %d\n", retval);

    int is_ok = 1;
    for(i = 0; i < 600; ++i) {
        if(buff2[i] != buff1[i]) {
            is_ok = 0;
            break;
        }
    }

    printf("Do all characters read from file equal \'a\'? %d\n", is_ok);

    return 0;
}

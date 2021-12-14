#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>           /* Definition of AT_* constants */
#include <sys/stat.h>

int main(int argc, char const *argv[])
{

	

	char const * arg1 = argv[1];
	int x = strtol(arg1, NULL, 0);
	printf("%s\n", argv[2]);

	FILE* fp;
	char buf[100];

	for(int i=0; i<x; i++){
		sprintf(buf, "%s%s%d",argv[2], "file", i);
		fp = fopen(buf, "w");
		fwrite("Hello", 1, 5, fp);
	}

	return 0;
}



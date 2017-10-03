#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main (int argc, char *argv[]) {  
int o;
char c[1000];
int shmid;
key_t key;
char *shm, *s;
FILE *fp;

while ((o = getopt (argc, argv, "h")) != -1)
	switch (o)
    {
		case 'h':
			perror("Help");
			break;
		case '?':
			return 1;
		default:
			break;
    }

fp = fopen("./Input.txt", "r");
if(fp == NULL){
	perror("fopen");
}

printf("Data from the file:\n%s", c);
while (fgets(c, sizeof(c), fp))
{
	printf("%s", c);
}
printf("File printing complete!\n");



fclose(fp);
	
return 0;
}

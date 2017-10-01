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
char c;
int shmid;
key_t key;
char *shm, *s;
FILE *fp;

while ((o = getopt (argc, argv, "h")) != -1)
	switch (c)
    {
		case 'h':
			perror("Help");
			break;
		case '?':
			return 1;
		default:
			abort ();
    }

fp = fopen("./Input.txt", "r");
if(fp == NULL){
	perror("fopen");
}
	
return 0;
}

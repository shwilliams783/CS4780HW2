#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define BUFFER 1000

int main (int argc, char *argv[]) {  
int o;
char c[BUFFER];
int shmid;
key_t key = 4780;
char *shm, *s;
FILE *fp;

/* Options */
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

/* Open the Input file */
fp = fopen("./Input.txt", "r");
if(fp == NULL)
{
	perror("fopen");
}

/* Create shared memory segment */
if ((shmid = shmget(key, BUFFER, IPC_CREAT | 0666)) < 0) 
{
	perror("shmget");
    exit(1);
}

/* Point shm to shared memory */
if ((shm = shmat(shmid, NULL, 0)) == (char *) -1)
{
	perror("shmat");
    exit(1);
}

/* Read the file */
while (fgets(shm, BUFFER, fp))
{
	printf("%s", shm);
}
printf("File printing complete!\n");

/* Release shared memory */
shm = NULL;
shmdt(shm);
shmctl(shmid, IPC_RMID, NULL);
fclose(fp);
	
return 0;
}

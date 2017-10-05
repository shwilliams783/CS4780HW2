#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define BUFFER 1000
#define MAXPROC 95

int main (int argc, char *argv[]) {
int o;
int errno;
int index = 0;
pid_t pid = getpid();
char c[BUFFER];
char idArg[33];
char indexArg[33];
char keyArg[33];
int shmidArray;
key_t keyArray = 8675;
char *shmArrayPtr[BUFFER];
char shmArray[MAXPROC][BUFFER];
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
        perror("MASTER: fopen");
}

/* Create shared memory segment for an array of strings */
shmidArray = shmget(keyArray, sizeof(char[MAXPROC][BUFFER]), IPC_CREAT | 0666);
if (shmidArray < 0)
{
	perror("MASTER: shmget: shmArray");
	exit(1);
}

/* Point shmArray to shared memory */
*shmArrayPtr = shmat(shmidArray, NULL, 0);
if ((void *)shmArray == (void *)-1)
{
    perror("MASTER: shmat: shmArray");
    exit(1);
}

snprintf(keyArg, 33, "%d", shmidArray);

/* Read the file string array mode */
for(index = 0; index < 95; index++)
{
	fgets((*shmArrayPtr + (index*BUFFER)), BUFFER, fp);
	/*printf("%d, %s", index, (*shmArrayPtr + (index * BUFFER)));*/
}

/* Fork off child processes */
for(index = 0; index < 3; index++)
{
	if(pid != 0)
	{
		perror("Parent process prints after fork()");
		pid = fork();
	}
	else if(pid == 0)
	{
		snprintf(idArg, 10, "%d", index);
		snprintf(indexArg, 10, "%d", ((index-1)*5));		
		execl("./palin", "palin", idArg, indexArg, keyArg, (char*)0);
	}
}

/* Release shared memory */
/*if(pid != 0)
{
	errno = shmdt(*shmArrayPtr);
	if(errno == -1)
	{
		perror("MASTER: shmdt: shmArray");
	}
	errno = shmctl(shmidArray, IPC_RMID, NULL);
	if(errno == -1)
	{
		perror("MASTER: shmctl: shmidArray");
	}
	fclose(fp);
	*shmArrayPtr = NULL;
}*/
return 0;
}

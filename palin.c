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
int i;
int errno;
int id = atoi(argv[1]);
int index = atoi(argv[2]);
int key = atoi(argv[3]);
pid_t pid = getpid();
int shmidArray;
key_t keyArray = 8675;
char *shmArrayPtr[BUFFER];
char shmArray[MAXPROC][BUFFER];

/* Point shmArray to shared memory */
*shmArrayPtr = shmat(key, NULL, 0);
if ((void *)shmArray == (void *)-1)
{
    perror("PALIN: shmat: key");
    exit(1);
}

printf("\nPalin.c Executed!\n");
for(i = 0; i < 5; i++)
{
	printf("%d\t%d\t%s\n", pid, index, (*shmArrayPtr + (index*BUFFER)));
	index++;
}

errno = shmdt(*shmArrayPtr);
if(errno == -1)
{
	perror("PALIN: shmdt: shmArrayPtr");
}
/*errno = shmctl(key, IPC_RMID, NULL);
if(errno == -1)
{
	perror("PALIN: shmctl: key");
}*/

return 0;
}

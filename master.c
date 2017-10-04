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
int index = 0;
char c[BUFFER];
int shmidStr, shmidArray;
key_t keyStr = 4780;
key_t keyArray = 8675;
char *shm, *shmArrayPtr[BUFFER];
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
        perror("fopen");
}

/* Create shared memory segment for a string */
if ((shmidStr = shmget(keyStr, BUFFER, IPC_CREAT | 0666)) < 0)
{
    perror("shmget: shm");
    exit(1);
}

/* Create shared memory segment for an array of strings */
shmidArray = shmget(keyArray, sizeof(char[MAXPROC][BUFFER]), IPC_CREAT | 0666);
if (shmidArray < 0)
{
	perror("shmget: shmArray");
	exit(1);
}

/* Point shm to shared memory */
if ((shm = shmat(shmidStr, NULL, 0)) == (char *) -1)
{
    perror("shmat: shm");
    exit(1);
}

/* Point shmArray to shared memory */
*shmArrayPtr = shmat(shmidArray, NULL, 0);
if ((void *)shmArray == (void *)-1)
{
    perror("shmat: shmArray");
    exit(1);
}

/* Read the file single string mode */
/*while (fgets(shm, BUFFER, fp))
{
        printf("%d, %s", (index), shm);
        index++;
}
printf("File printing complete!\n");*/


/* Read the file string array mode */
for(index = 0; index < 95; index++)
{
	fgets(shmArray[index], BUFFER, fp);
	printf("%d, %s", index, shmArray[index]);
}
printf("File printing complete!\n");


/* Release shared memory */
shm = NULL;
shmdt(shm);
shmctl(shmidStr, IPC_RMID, NULL);
shmdt(shmArray);
shmctl(shmidArray, IPC_RMID, NULL);
fclose(fp);

return 0;
}

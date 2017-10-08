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
enum state { idle, want_in, in_cs };
int o;
int errno;
int index = 0;
pid_t pid = getpid();
char c[BUFFER];
char idArg[33];
char indexArg[33];
char arrayArg[33];
char turnArg[33];
char flagArg[33];
int shmidArray;
int shmidTurn;
int shmidFlag;
key_t keyArray = 8675;
key_t keyTurn = 1138;
key_t keyFlag = 1123;
char *shmArrayPtr;
int *turn;
enum state *flag;/*Flag corresponding to each process in shared memory */
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
	perror("MASTER: shmget: shmidArray");
	exit(1);
}

/* Point shmArrayPtr to shared memory */
shmArrayPtr = shmat(shmidArray, NULL, 0);
if ((void *)shmArrayPtr == (void *)-1)
{
    perror("MASTER: shmat: shmArrayPtr");
    exit(1);
}

/* Create shared memory segment for an int */
shmidTurn = shmget(keyTurn, sizeof(int), IPC_CREAT | 0666);
if (shmidTurn < 0)
{
	perror("MASTER: shmget: shmidTurn");
	exit(1);
}

/* Point turn to shared memory */
turn = shmat(shmidTurn, NULL, 0);
if ((void *)turn == (void *)-1)
{
    perror("MASTER: shmat: turn");
    exit(1);
}

/* Set turn to 1*/
*turn = 1;

/* Create shared memory segment for an array of states */
shmidFlag = shmget(keyFlag, 76, IPC_CREAT | 0666);
if (shmidFlag < 0)
{
	perror("MASTER: shmget: shmidFlag");
	exit(1);
}

/* Point flag to shared memory */
flag = shmat(shmidFlag, NULL, 0);
if ((void *)flag == (void *)-1)
{
    perror("MASTER: shmat: flag");
    exit(1);
}


/* Convert the Array, Turn, and Flag keys into strings for EXEC parameters */
sprintf(arrayArg, "%d", shmidArray);
sprintf(turnArg, "%d", shmidTurn);
sprintf(flagArg, "%d", shmidFlag); 

/* Read the file string array mode */
for(index = 0; index < 95; index++)
{
	fgets((shmArrayPtr + (index*BUFFER)), BUFFER, fp);
	/* printf("%d, %s", index, (shmArrayPtr + (index * BUFFER))); */
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
		execl("./palin", "palin", idArg, indexArg, arrayArg, turnArg, flagArg, (char*)0);
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
	
	errno = shmdt(*turn);
	if(errno == -1)
	{
		perror("MASTER: shmdt: turn");
	}
	errno = shmctl(shmidTurn, IPC_RMID, NULL);
	if(errno == -1)
	{
		perror("MASTER: shmctl: shmidTurn");
	}
	
	errno = shmdt(*flag);
	if(errno == -1)
	{
		perror("MASTER: shmdt: flag");
	}
	errno = shmctl(shmidFlag, IPC_RMID, NULL);
	if(errno == -1)
	{
		perror("MASTER: shmctl: shmidFlag");
	}
	
	fclose(fp);
	*shmArrayPtr = NULL;
	*turn = NULL;
	*flag = NULL;
}*/
return 0;
}

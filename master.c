#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define BUFFER 1000
#define MAXPROC 96

int errno;
enum state { idle, want_in, in_cs };
int shmidArray;
int shmidTurn;
int shmidFlag;
char *shmArrayPtr;
int *turn;
enum state *flag;

void sigIntHandler(int signum)
{
	perror("Caught SIGINT! Killing all child processes.");
	
	errno = shmdt(shmArrayPtr);
	if(errno == -1)
	{
		perror("MASTER: shmdt: shmArray");
	}
	
	errno = shmctl(shmidArray, IPC_RMID, NULL);
	if(errno == -1)
	{
		perror("MASTER: shmctl: shmidArray");
	}

	errno = shmdt(turn);
	if(errno == -1)
	{
		perror("MASTER: shmdt: turn");
	}
	
	errno = shmctl(shmidTurn, IPC_RMID, NULL);
	if(errno == -1)
	{
		perror("MASTER: shmctl: shmidTurn");
	}

	errno = shmdt(flag);
	if(errno == -1)
	{
		perror("MASTER: shmdt: flag");
	}
	
	errno = shmctl(shmidFlag, IPC_RMID, NULL);
	if(errno == -1)
	{
		perror("MASTER: shmctl: shmidFlag");
	}
	
	exit(signum);
}



int main (int argc, char *argv[]) {

int o;
int i;
int returnStatus;
int index = 0;
pid_t pid[20] = {getpid()};
char c[BUFFER];
char idArg[33];
char indexArg[33];
char arrayArg[33];
char turnArg[33];
char flagArg[33];
key_t keyArray = 8675;
key_t keyTurn = 1138;
key_t keyFlag = 1123;
FILE *fp;
signal(SIGINT, sigIntHandler);


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
fclose(fp);

/* Fork off child processes */
for(index = 0; index < 20; index++)
{
	if(pid[index] != 0)
	{
		/* printf("Child Process Loop - PID = %d, Index = %d\n", pid[index], index); */
		perror("Parent process prints after fork()");
		pid[index+1] = fork();
		/*if(pid[index] != 0)
		{
			printf("CP Inner Loop - CPID = %d, Index = %d\n", pid[index+1], index+1);
			waitpid(pid[index+1], &returnStatus, 0);
			if(returnStatus)
			{
				perror("MASTER: child terminated with an error!");
			}
		}*/
	}
	else if(pid[index] == 0)
	{
		snprintf(idArg, 10, "%d", index);
		snprintf(indexArg, 10, "%d", ((index-1)*5));		
		execl("./palin", "palin", idArg, indexArg, arrayArg, turnArg, flagArg, (char*)0);
	}
}

/* Signal handler for killing child processes */
for(i = 0; i < 60; i++)
{
	sleep(1);
}

perror("Time is up!");

/* Release shared memory */
errno = shmdt(shmArrayPtr);
if(errno == -1)
{
	perror("MASTER: shmdt: shmArray");
}
errno = shmctl(shmidArray, IPC_RMID, NULL);
if(errno == -1)
{
	perror("MASTER: shmctl: shmidArray");
}

errno = shmdt(turn);
if(errno == -1)
{
	perror("MASTER: shmdt: turn");
}
errno = shmctl(shmidTurn, IPC_RMID, NULL);
if(errno == -1)
{
	perror("MASTER: shmctl: shmidTurn");
}

errno = shmdt(flag);
if(errno == -1)
{
	perror("MASTER: shmdt: flag");
}
errno = shmctl(shmidFlag, IPC_RMID, NULL);
if(errno == -1)
{
	perror("MASTER: shmctl: shmidFlag");
}

for(i = 1; i < 20; i++)
{
	/* printf("Killing process #%d\n", pid[i]); */
	kill(pid[i], SIGINT);
}

return 0;
}

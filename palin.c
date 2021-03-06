#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define BUFFER 1000
#define MAXPROC 96

enum state { idle, want_in, in_cs };
int errno;
char errmsg[100];
int id;
char *shmArrayPtr;
int *turn;
enum state *flag;

void sigIntHandler(int signum)
{
	snprintf(errmsg, sizeof(errmsg), "PALIN %d: Caught SIGINT! Terminating process.", id);
	perror(errmsg);	
	errno = shmdt(shmArrayPtr);
	if(errno == -1)
	{
		snprintf(errmsg, sizeof(errmsg), "PALIN %d: shmdt(shmArrayPtr)", id);
		perror(errmsg);
	}
	errno = shmdt(turn);
	if(errno == -1)
	{
		snprintf(errmsg, sizeof(errmsg), "PALIN %d: shmdt(turn).", id);
		perror(errmsg);
	}

	errno = shmdt(flag);
	if(errno == -1)
	{
		snprintf(errmsg, sizeof(errmsg), "PALIN %d: shmdt(flag).", id);
		perror(errmsg);
	}
	exit(signum);
}


int main (int argc, char *argv[]) {
int i;
int j;
int k;
int palindrome = 1;
int index = atoi(argv[2]);
int arrayKey = atoi(argv[3]);
int turnKey = atoi(argv[4]);
int flagKey = atoi(argv[5]);
pid_t pid = getpid();
int shmidArray;
key_t keyArray = 8675;
key_t keyTurn = 1138;
key_t keyFlag = 1123;
char *startPtr;
char *endPtr;
char *currentPtr;
FILE *fp;
signal(SIGINT, sigIntHandler);
time_t currentTime;
struct tm* timeInfo;
char timeString[100];

id = atoi(argv[1]);


/* Identify self with PID */
/* printf("Palin.c ID# %d\tPID:%d\n", id, pid); */

/* Seed random number generator */
srand(pid * time(NULL));

/* Point shmArrayPtr to shared memory */
shmArrayPtr = shmat(arrayKey, NULL, 0);
if ((void *)shmArrayPtr == (void *)-1)
{
	snprintf(errmsg, sizeof(errmsg), "PALIN %d: shmat(arrayKey).", id);
	perror(errmsg);	
    exit(1);
}

/* Point turn to shared memory */
turn = shmat(turnKey, NULL, 0);
if ((void *)turn == (void *)-1)
{
	snprintf(errmsg, sizeof(errmsg), "PALIN %d: shmat(turnKey).", id);
	perror(errmsg);	
    exit(1);
}

/* Point flag to shared memory */
flag = shmat(flagKey, NULL, 0);
if ((void *)flag == (void *)-1)
{
	snprintf(errmsg, sizeof(errmsg), "PALIN %d: shmat(flagKey).", id);
	perror(errmsg);
    exit(1);
}

/* printf("\nPalin.c Executed!\n"); */


startPtr = (shmArrayPtr + (index*BUFFER));
/* printf("startPtr = %s\n", startPtr); */
currentPtr = strtok(startPtr, "\n");
/* printf("currentPtr = %s, strlen = %d\n", currentPtr, strlen(currentPtr)); */
endPtr = currentPtr + (strlen(currentPtr)-1);
/* printf("currentPtr = %s, strlen = %d, endPtr[0] = %c\n", currentPtr, strlen(currentPtr), endPtr[0]); */


for(i = 0; i < 5; i++)
{
	/* printf("currentPtr = %s, strlen = %d, endPtr[0] = %c\n", currentPtr, strlen(currentPtr), endPtr[0]);*/
	palindrome = 1;
	for(k = 0; k < strlen(currentPtr); k++)
	{
		if(currentPtr[k] != endPtr[0-k]) /* Character mismatch; not a palindrome */
		{
			/* printf("Not a palindrome at index = %d: %c != %c\n", (index), currentPtr[k], endPtr[0-k]); */
			palindrome = 0;
			break;
		}
	}

	/*if(palindrome)
	{
		printf("Palindrome at index = %d: %s\n", index, currentPtr);
	}*/
	
	currentTime = time(NULL);
    timeInfo = localtime(&currentTime);
	strftime(timeString, 26, "%H:%M:%S", timeInfo);
	snprintf(errmsg, sizeof(errmsg), "Palin\t%d\tattempting to enter critical section at time:\t%s\t", id, timeString);
	perror(errmsg);
	do{
		*(flag + id*4) = want_in; /* Raise my flag */
		j = *turn; /* Set local variable */
		/* wait until its my turn */
		while( j != id )
			/* j = ( *(flag + j*4) != idle ) ? turn : (( j + 1 ) % 19) + 1; */
			if(*(flag + j*4) != idle)
				j = *turn;
			else
				j = (( j + 1 ) % 19) + 1;
			/* Declare intention to enter critical section */
			*(flag + id*4) = in_cs;
			/* Check that no one else is in critical section */
			for( j = 1; j < 20; j++ )
				if(( j != id ) && ( *(flag + j*4) == in_cs ))
					break;
	}while(( j < 20 ) || ( *turn != id && *(flag + *turn*4) != idle ));
	/*  Assign turn to self and enter critical section */
	*turn = id;
	/* Mark time of entering critical section */
	currentTime = time(NULL);
    timeInfo = localtime(&currentTime);
	strftime(timeString, 26, "%H:%M:%S", timeInfo);
	snprintf(errmsg, sizeof(errmsg), "Palin\t%d\thas entered critical section at time:\t\t%s\t", id, timeString);
	perror(errmsg);
	/* Critical Section */
	sleep(rand()%2);
	/* printf("%d\t%d\t%s\n", pid, index, (shmArrayPtr + (index*BUFFER))); */ 	
	if(palindrome)
	{
		/* printf("Palindrome at index = %d: %s\n", index, currentPtr); */
		/* Open palin.out for appending */
		fp = fopen("./palin.out", "a");
		if(fp == NULL)
		{
			snprintf(errmsg, sizeof(errmsg), "PALIN %d: fopen(palin.out).", id);
			perror(errmsg);
		}
		fprintf(fp, "%d\t%d\t%s\n", pid, index, currentPtr);
		fclose(fp);		
	}
	else
	{
		/* printf("Not a palindrome at index = %d: %c != %c\n", (index), currentPtr[k], endPtr[0-k]); */
		/* Open Palin.out for appending */
		fp = fopen("./nopalin.out", "a");
		if(fp == NULL)
		{
			snprintf(errmsg, sizeof(errmsg), "PALIN %d: fopen(nopalin.out).", id);
			perror(errmsg);
		}
		fprintf(fp, "%d\t%d\t%s\n", pid, index, currentPtr);
		fclose(fp);
	}
	sleep(rand()%2);
	/*  Exit critical section */
	/* Mark time of entering critical section */
	currentTime = time(NULL);
    timeInfo = localtime(&currentTime);
	strftime(timeString, 26, "%H:%M:%S", timeInfo);
	snprintf(errmsg, sizeof(errmsg), "Palin\t%d\thas exited critical section at time:\t\t%s\t", id, timeString);
	perror(errmsg);
	j = (((*turn + 1) % 19) + 1);
	while(*(flag + j*4) == idle)
		j = (((j + 1) % 19) + 1);
	/*  Assign turn to next waiting process; change own flag to idle */
	*turn = j; (*(flag + id*4)) = idle;
	/* printf("currentPtr = %s, strlen = %d, endPtr[0] = %c\n", currentPtr, strlen(currentPtr), endPtr[0]); */
	index++; /* Remainder Section */
	startPtr = (startPtr + BUFFER);
	currentPtr = strtok(startPtr, "\n");
	endPtr = currentPtr + (strlen(currentPtr) - 1);
	/* printf("*turn = %d\t*flag(id) = %d\ti = %d\n", *turn, (*(flag + id*4)), i); */
}

/* printf("Job complete! Palin #%d\n", id); */

errno = shmdt(shmArrayPtr);
if(errno == -1)
{
	snprintf(errmsg, sizeof(errmsg), "PALIN %d: shmdt(shmArrayPtr).", id);
	perror(errmsg);
}

errno = shmdt(turn);
if(errno == -1)
{
	snprintf(errmsg, sizeof(errmsg), "PALIN %d: shmdt(turn).", id);
	perror(errmsg);
}

errno = shmdt(flag);
if(errno == -1)
{
	snprintf(errmsg, sizeof(errmsg), "PALIN %d: shmdt(flag).", id);
	perror(errmsg);
}

return 0;
}

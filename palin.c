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
int i;
int j;
int k;
int palindrome = 1;
int errno;
int id = atoi(argv[1]);
int index = atoi(argv[2]);
int arrayKey = atoi(argv[3]);
int turnKey = atoi(argv[4]);
int flagKey = atoi(argv[5]);
pid_t pid = getpid();
int shmidArray;
key_t keyArray = 8675;
key_t keyTurn = 1138;
key_t keyFlag = 1123;
char *shmArrayPtr;
char *startPtr;
char *endPtr;
char *currentPtr;
int *turn;
enum state *flag;
FILE *fp;


/* Seed random number generator */
srand(pid * time(NULL));

/* Point shmArrayPtr to shared memory */
shmArrayPtr = shmat(arrayKey, NULL, 0);
if ((void *)shmArrayPtr == (void *)-1)
{
    perror("PALIN: shmat: arrayKey");
    exit(1);
}

/* Point turn to shared memory */
turn = shmat(turnKey, NULL, 0);
if ((void *)turn == (void *)-1)
{
    perror("PALIN: shmat: turnKey");
    exit(1);
}

/* Point flag to shared memory */
flag = shmat(flagKey, NULL, 0);
if ((void *)flag == (void *)-1)
{
    perror("PALIN: shmat: flagKey");
    exit(1);
}

printf("\nPalin.c Executed!\n");


startPtr = (shmArrayPtr + (index*BUFFER));
/* printf("startPtr = %s\n", startPtr); */
currentPtr = strtok(startPtr, "\n");
/* printf("currentPtr = %s, strlen = %d\n", currentPtr, strlen(currentPtr)); */
endPtr = currentPtr + (strlen(currentPtr)-1);
/* printf("currentPtr = %s, strlen = %d, endPtr[0] = %c\n", currentPtr, strlen(currentPtr), endPtr[0]); */


for(i = 0; i < 5; i++)
{
	palindrome = 1;
	for(k = 0; k < strlen(currentPtr); k++)
	{
		if(currentPtr[k] != endPtr[0-k]) /* Character mismatch; not a palindrome */
		{
			printf("Not a palindrome at index = %d: %c != %c\n", (index), currentPtr[k], endPtr[0-k]);
			palindrome = 0;
			break;
		}
	}

	if(palindrome)
	{
		printf("Palindrome at index = %d: %s\n", index, currentPtr);
	}
	
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
	sleep(rand()%2);
	printf("%d\t%d\t%s\n", pid, index, (shmArrayPtr + (index*BUFFER))); /* Critical Section */
	sleep(rand()%2);
	/*  Exit section */
	j = ((*turn + 1) % 19) + 1;
	while(*(flag + j*4) == idle)
		j = ((j + 1) % 19) + 1;
	/*  Assign turn to next waiting process; change own flag to idle */
	*turn = j; *(flag + id*4) = idle;
	/* printf("currentPtr = %s, strlen = %d, endPtr[0] = %c\n", currentPtr, strlen(currentPtr), endPtr[0]); */
	index++; /* Remainder Section */
	currentPtr = strtok(startPtr+index*1000, "\n");
	endPtr = currentPtr + (strlen(currentPtr) - 1);
	
}

errno = shmdt(shmArrayPtr);
if(errno == -1)
{
	perror("PALIN: shmdt: shmArrayPtr");
}
/*errno = shmctl(arrayKey, IPC_RMID, NULL);
if(errno == -1)
{
	perror("PALIN: shmctl: arrayKey");
}*/

return 0;
}

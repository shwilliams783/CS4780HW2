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





/* process(const int i ) {
	do {
		
			
	} while ( 1 );
} */

int main (int argc, char *argv[]) {
enum state { idle, want_in, in_cs };
int i;
int j;
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
char *shmArrayPtr[BUFFER];
char *startPtr;
char *endPtr;
int *turn;
enum state *flag;
FILE *fp;

/* Point shmArrayPtr to shared memory */
*shmArrayPtr = shmat(arrayKey, NULL, 0);
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
for(i = 0; i < 5; i++)
{
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
	printf("%d\t%d\t%s\n", pid, index, (*shmArrayPtr + (index*BUFFER))); /* Critical Section */
	/*  Exit section */
	j = ((*turn + 1) % 19) + 1;
	while(*(flag + j*4) == idle)
		j = ((j + 1) % 19) + 1;
	/*  Assign turn to next waiting process; change own flag to idle */
	*turn = j; *(flag + id*4) = idle;
	index++; /* Remainder Section */
}

errno = shmdt(*shmArrayPtr);
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

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
		do {
			flag[i] = want_in; // Raise my flag
			j = turn; // Set local variable
			// wait until its my turn
			while ( j != i )
				j = ( flag[j] != idle ) ? turn : ( j + 1 ) % n;
			// Declare intention to enter critical section
			flag[i] = in_cs;
			// Check that no one else is in critical section
			for ( j = 0; j < n; j++ )
				if ( ( j != i ) && ( flag[j] == in_cs ) )
					break;
		} while ( ( j < n ) || ( turn != i && flag[turn] != idle ) );
		// Assign turn to self and enter critical section
		turn = i;
		critical_section();
		// Exit section
		j = (turn + 1) % n;
		while (flag[j] == idle)
		j = (j + 1) % n;
		// Assign turn to next waiting process; change own flag to idle
		turn = j; flag[i] = idle;
		remainder_section();
	} while ( 1 );
} */

int main (int argc, char *argv[]) {
enum state { idle, want_in, in_cs };
int i;
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
char *startPtr, *endPtr;
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
	printf("%d\t%d\t%s\n", pid, index, (*shmArrayPtr + (index*BUFFER)));
	index++;
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

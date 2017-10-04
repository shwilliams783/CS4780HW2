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
int errno;
int index = 0;
pid_t pid = getpid();
int shmidArray;
key_t keyArray = 8675;
char *shmArrayPtr[BUFFER];
char shmArray[MAXPROC][BUFFER];

printf("\nPalin.c Executed!\n");

return 0;
}

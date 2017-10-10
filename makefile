all : master palin
clean :
	rm master.o palin.o palin.out nopalin.out
master : master.o
	gcc -g -o master master.o
master.o : master.c
	gcc -c -g master.c
palin : palin.o
	gcc -g -o palin palin.o
palin.o : palin.c
	gcc -c -g palin.c

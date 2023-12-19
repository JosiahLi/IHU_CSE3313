CC=gcc
CFALGS=-O

all : smallsh

main.o : main.c smallsh.h
	$(CC) $(CFALGS) -g -c -o $@ $< 
smallsh.o : smallsh.c smallsh.h
	$(CC) $(CFALGS) -g -c -o $@ $<
smallsh : main.o smallsh.o
	$(CC) $(CFALGS) -g -o $@ main.o smallsh.o 

clean:
	rm -rf *.o smallsh 
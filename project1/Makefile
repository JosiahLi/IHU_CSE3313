CC=gcc
CFALGS=-O

all : project1

main.o : main.c smallsh.h
	$(CC) $(CFALGS) -g -c -o $@ $< 
smallsh.o : smallsh.c smallsh.h
	$(CC) $(CFALGS) -g -c -o $@ $<
project1 : main.o smallsh.o
	$(CC) $(CFALGS) -g -o $@ main.o smallsh.o 

clean:
	rm -rf *.o project1
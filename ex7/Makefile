CC=gcc
CFLAGS=-O

all: ex_div0 ex_sigint ex_kill

ex_% : ex_%.o 
	$(CC) -o $@ $<
%.o : %.c
	$(CC) -c -o $@ $<

clean :
	rm -f *.o ex_div0 ex_sigint ex_kill
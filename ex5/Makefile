CC=gcc
CFLAGS= -O

all: ex_ls ex_getpid ex_fork

ex_% : ex_%.o
	$(CC) $(CFLAGS) -o $@ $<

%.o : %.c
	$(CC) $(CFLAGS) -g -c -o $@ $<

clean: 
	rm -f *.o ex_ls ex_getpid ex_fork
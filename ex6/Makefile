CC=gcc
CFLAGS=-O

all : ex_exec ex_exit ex_zombie

ex_% : ex_%.o
	$(CC) $(CFLAGS) -o $@ $<
ex_%.o : ex_%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean :
	rm -f *.o ex_exit ex_exec ex_zombie
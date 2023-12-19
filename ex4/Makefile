CC=gcc
CFLAGS=-O

all : ex_ls

ex_% : ex_%.o
	$(CC) $(CFLAGS) -o $@ $<
ex_%.o : ex_%.c
	$(CC) $(CFLAGS) -c -o $@ $<


clean : 
	rm -rf *.o ex_ls
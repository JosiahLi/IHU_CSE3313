CC = gcc

all : ex_sigint ex_sigjmp ex_kill ex_test

ex_% : ex_%.o
	$(CC) -o $@ $<
%.o : %.c
	$(CC) -c -o $@ $<

clean : 
	rm -f *.o ex_sigint ex_sigjmp ex_kill ex_test
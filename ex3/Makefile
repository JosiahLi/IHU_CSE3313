CC=gcc
CFLAGF=-O

all : ex_umask ex_getopt ex_chmod

ex_% : ex_%.o
	$(CC) $(CFLAG) -o $@ $<
%.o : %.c 
	$(CC) $(CFLAG) -c $<

clean :
	rm -f *.o ex_chmod ex_getopt ex_umask
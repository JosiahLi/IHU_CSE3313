CC = gcc
CFLAGS = -Wall -g

all : ex_pipe1 ex_pipe2 ex_receiver ex_sender ex_sender2 ex_receiver2

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<
% : %.o
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf *.o ex_pipe1 ex_pipe2 ex_receiver ex_sender ex_sender2 ex_receiver2
CC = gcc
CFLAGS = -Wall -g

all : msg_receiver  msg_receiver2 msg_sender sem_main

msg_receiver: msg_receiver.c msg_header.h 
	$(CC) $(CFLAGS) -o $@ $<
msg_receiver2: msg_receiver2.c msg_header.h 
	$(CC) $(CFLAGS) -o $@ $<
msg_sender: msg_sender.c msg_header.h 
	$(CC) $(CFLAGS) -o $@ $<
sem_main: sem_main.c sem_ctl.c sem_header.h
	$(CC) $(CFLAGS) -o $@ sem_main.c sem_ctl.c

clean:
	rm -r msg_receiver msg_receiver2 msg_sender sem_main
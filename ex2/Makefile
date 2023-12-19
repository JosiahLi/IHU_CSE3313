CC=gcc

all: ex_open ex_copy ex_dup hotel

ex_open : ex_open.o
	$(CC) -o $@ $<
ex_copy : ex_copy.o
	$(CC) -o $@ $<
ex_dup : ex_dup.o
	$(CC) -o $@ $<
hotel : hotel.o
	$(CC) -o $@ $<

%.o : %.c
	$(CC) -c $< 

clean : 
	rm  ex_open ex_copy ex_dup hotel *.o
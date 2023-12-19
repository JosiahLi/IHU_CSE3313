CC=gcc
CFLAGS=-g -O
OBJS=main.o func_one.o func_two.o
TARGET=print_func

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o : header.h main.c
	gcc -c -o main.o main.c
func_one.o : header.h func_one.c
	gcc -c -o func_one.o func_one.c
func_two.o : header.h func_two.c
	gcc -c -o func_two.o func_two.c

clean : 
	rm -f *.o print_func

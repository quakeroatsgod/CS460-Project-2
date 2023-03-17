CC=gcc
CFLAGS= -lpthread -Wall -Werror

all: exec

run:
	./exec
run2:
	./exec -alg FCFS -quantum 2 -input input
exec: main.o input.o linked_list.o
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c main.h
	$(CC) $(CFLAGS) -c $<

input.o: input.c main.h
	$(CC) $(CFLAGS) -c $<

linked_list.o: linked_list.c main.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o exec
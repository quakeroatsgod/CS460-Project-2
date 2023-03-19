CC=gcc
CFLAGS= -lpthread -pthread -Wall -Werror
OBJDIR=obj
BINDIR=bin

all:	$(OBJDIR) $(BINDIR) $(BINDIR)/exec

run:
	./$(BINDIR)/exec
run2:
	./$(BINDIR)/exec -alg FCFS -quantum 2 -input input

$(BINDIR)/exec:	$(OBJDIR)/main.o $(OBJDIR)/input.o $(OBJDIR)/io.o $(OBJDIR)/cpu.o $(OBJDIR)/linked_list.o
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/main.o:	main.c main.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/input.o:	input.c main.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/io.o:	io.c main.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/cpu.o:	cpu.c main.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/linked_list.o:	linked_list.c main.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(BINDIR)
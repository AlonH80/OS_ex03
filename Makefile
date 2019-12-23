CFLAGS=-Wall -std=gnu11
LDFLAGS=-lm
CC=gcc -std=c99 -g
ECHO=echo "going to compile for target $@"
OS=$(shell uname)
ifneq (,$(findstring CYGWIN_NT,$(OS)))
	EXT=.exe
else
	EXT=
endif

PROG_OBJS=pipeSystem.o
MAIN_FILE=ex3_q1
PROG=$(MAIN_FILE)$(EXT)

all:$(PROG) test

$(PROG): $(PROG_OBJS) subst
	@$(ECHO)
	$(CC) -o $(PROG) $(CFLAGS) $(PROG_OBJS)

subst: subst.o
	@$(ECHO)
	$(CC) -o subst $(CFLAGS) subst.o

%.o: %.c
	@$(ECHO)
	$(CC) $(CFLAGS) -c $<

test:
	@echo going to run test...
	./ex3_q1 abc xyz out.log < in.txt 2> err.log

clean:
	rm -vf *.o *.gch subst $(PROG) *.log *.tmp

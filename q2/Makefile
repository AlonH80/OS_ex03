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

PROG_OBJS=ex3_q2.o ex3_q2_given.o
MAIN_FILE=ex3_q2
PROG=$(MAIN_FILE)$(EXT)

all:$(PROG) test

$(PROG): $(PROG_OBJS)
	@$(ECHO)
	$(CC) -o $(PROG) $(CFLAGS) $^

%.o: %.c
	@$(ECHO)
	$(CC) $(CFLAGS) -c $^

test:
	@echo going to run test...
	./ex3_q2  > out.log 2> err.log

clean:
	rm -vf *.o $(PROG) *.log *.tmp

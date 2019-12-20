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

PROG_OBJS=

PROG=$(MAIN_FILE)$(EXT)

subst: subst.o
	@$(ECHO)
	$(CC) -o subst $(CFLAGS) subst.o

%.o: %.c
	@$(ECHO)
	$(CC) $(CFLAGS) -c $<

all:$(PROG) test

test:
	@echo going to run test...
	./$(PROG) < in.txt > out.log

clean:
	rm -vf *.o *.gch subst *.log *.tmp
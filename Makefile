# Lander Makefile

HSFILE := /usr/local/games/lib/lander.hs
LIBS := -lcurses -lm

OBJS := land.o screen.o move.o score.o
SRC := land.c screen.c move.c score.c
INC := consts.h funcs.h
BIN := /usr/local/bin
OPT := -O2
HSSTRING := '"$(HSFILE)"'
CFLAGS := -Wall $(OPT) -DHS_FILE=$(HSSTRING)

lander: $(OBJS)
	cc -o lander $(OBJS) $(LIBS)

$(OBJS): consts.h funcs.h

clean:
	rm -f $(OBJS) lander

install: lander
	sudo rm -f $(BIN)/lander
	sudo cp lander $(BIN)
	sudo chown stacey $(BIN)/lander

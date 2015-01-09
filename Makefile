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

lander: $(OBJS) lander.man
	cc -o lander $(OBJS) $(LIBS)

$(OBJS): consts.h funcs.h

lander.man: lander.6
	nroff -man lander.6 > lander.man

clean:
	rm -f $(OBJS) lander lander.man

install: lander
	sudo rm -f $(BIN)/lander
	sudo cp lander $(BIN)
	sudo chown stacey $(BIN)/lander

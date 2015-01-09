# Lander Makefile

# System 5.3
#
OSV=SYS5_3
LIBS= -lcurses -lm
RAND_L=RAND_SYS5

# BSD Unix
#
#OSV=BSD
#LIBS= -lcurses -ltermcap -lm
#RAND_L=RAND_BSD

# System 5.2 or earlier (untested but should work)
#
# OSV=BSD
# LIBS= -lcurses -lm
# RAND_L=RAND_SYS5

# high score file name - change for your system
HSFILE= /usr/local/games/lib/lander.hs

OBJS= land.o screen.o move.o score.o
SRC= land.c screen.c move.c score.c
INC= consts.h funcs.h
BIN= /usr/local/bin
OPT= -O2
HSSTRING='"$(HSFILE)"'
CFLAGS= -Wall $(OPT) -D$(RAND_L) -DHS_FILE=$(HSSTRING) -D$(OSV)

lander: $(OBJS)
	cc -o lander $(CFLAGS) $(OBJS) $(LIBS)

$(OBJS): consts.h

clean:
	rm -f $(OBJS) lander

shar:
	xshar README lander.6 Makefile $(SRC) $(INC) > lander.shar

lint:
	lint $(CFLAGS) $(SRC)

install: lander
	sudo rm -f $(BIN)/lander
	sudo cp lander $(BIN)
	sudo chown stacey $(BIN)/lander

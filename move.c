#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <curses.h>
#include "consts.h"
#include "funcs.h"

#define NO_INP EOF
#define FABS_M(x) ((x) >= -1.0 ? 1.0 : -(x))
#define FUEL_INIT 5000.0
#define FUEL_DRAIN 450.0
#define FUEL_MIN 100.0

double Fuel;
double FuelDec = 0.0;

static double Power;
static const double PowerSet[] = {0.0, 0.01, 0.1, 0.5, 1.0, 2.0, 5.0, 10.0, 15.0, 20.0};

static double att1p, att2p, att3p;

void
InitMoves(WINDOW * screen)
{
	att1p = 0.0;
	att2p = 0.0;
	att3p = 0.0;
	Power = 0.0;
	Fuel = FUEL_INIT - FuelDec;
	if (Fuel < FUEL_MIN)
		Fuel = FUEL_MIN;
	FuelDec += FUEL_DRAIN;
	nodelay(screen, TRUE);
}

void
GetMove(WINDOW *screen, double *y_delta, double *x_delta)
{
	int ch, index;
	double y_delta_inc;

	if (Fuel > 0.0)
	{
		wrefresh(screen);
		while ((ch = wgetch(screen)) != NO_INP)
		{
			if (isdigit(ch))
			{
				index = ch - '0';
				Power = PowerSet[index];
			}
			else
				switch (ch)
				{
				case 'z':
					att1p = Power;
					break;
				case 'x':
					att2p = Power;
					break;
				case 'c':
					att3p = Power;
					break;
				default:
					flash();
					break;
				}
		}
		*x_delta += att1p;
		y_delta_inc = att2p * (log10(FABS_M(*y_delta)) / GRAVITY);
		*y_delta += y_delta_inc;
		*x_delta -= att3p;
		Fuel -= att3p + att2p + att1p;
		if (Fuel < 0.0)
			att3p = att2p = att1p = Power = Fuel = 0.0;
	}
	wmove(screen, LINES - 1, 0);
	wclrtoeol(screen);
	wprintw(screen, "Thrust - L: %5.2f vert: %7.4f R: %5.2f Pow: %5.2f Fuel: %7.2f", att1p, att2p, att3p, Power, Fuel);
	wrefresh(screen);
}

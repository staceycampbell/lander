#include <stdio.h>
#include <math.h>
#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "consts.h"
#include "funcs.h"

#define INIT_DX_VAL 166.0	/* initial horizontal velocity */
#define INIT_DX_INC 13.0	/* rate of init hor vel increase between landings */
#define INIT_DY_VAL 0.0		/* initial vertical velocity */
#define INIT_DY_INC 3.0		/* rate of decrease in vert velocity b/w landings */

extern int LastLegalY, LastLegalX;
extern int Score, BestScore;
extern double FuelDec;

int Landings;
int BSLandings = 0;

/*
** StartLander() - initialise a new game.
*/

extern int Score;

static void
StartLander(double *init_dy, double *init_dx)
{
	Landings = 0;
	Score = 0;
	FuelDec = 0.0;
	*init_dy = INIT_DY_VAL;
	*init_dx = INIT_DX_VAL;
}

/*
** EndCurses() - final cleanup before program exit.
*/

static void
EndCurses(WINDOW *screen)
{
	nodelay(screen, FALSE);	/* switch off nodelay */
	wmove(screen, 1, 0);
	wclrtoeol(screen);	/* display best score */
	wprintw(screen, "--Best Score: %d ", BestScore);
	wprintw(screen, "with %d landing%s - press space--", BSLandings, BSLandings == 1 ? "" : "s");
	wrefresh(screen);
	while (wgetch(screen) != ' ');
	endwin();		/* shut down curses */
}

static int
CleanUp(WINDOW *screen)
{
	char ch;

	nodelay(screen, FALSE);
	if (Score > BestScore)
	{
		BestScore = Score;
		BSLandings = Landings;
	}
	ScWrite(screen);
	ScReadDisplay(screen);
	wmove(screen, 1, 0);
	wprintw(screen, "--Game Over - Safe Landings: %d - another game?--", Landings);
	wrefresh(screen);
	do
	{
		ch = wgetch(screen);
	} while (ch != 'Y' && ch != 'y' && ch != ' ' && ch != 'n');

	return ch == 'Y' || ch == 'y' || ch == ' ';
}

#define SEQ_COUNT (sizeof(sequenceA) / sizeof(sequenceA[0]))
#define AVERAGE 9
#define DEVIATION 4
#define MAX_PARTS (AVERAGE + DEVIATION)
static const char sequenceA[] = { '-', '/', '|', '\\' };
static const char sequenceB[] = { '-', '\\', '|', '/' };

static void
Explode(WINDOW *screen, int Y_bang, int X_bang)
{
	int particles, i, new_y, new_x, draw_y, draw_x, touched, toy, tox;
	int overlay[SCR_Y][SCR_X];
	chtype old_chs[SCR_Y][SCR_X];
	double x_inc;
	struct paths_t
	{
		double x_mult;
		double y_mult;
		double x;
		int old_y;
		int old_x;
		int seq_no;
		const char *sequence;
	} paths[MAX_PARTS];
	struct paths_t *path;

	wstandout(screen);
	mvwaddch(screen, SCR_ADJ(Y_bang), X_bang, '*');
	wstandend(screen);
	wrefresh(screen);
	srand48(time((long *)0));
	memset((char *)overlay, 0, sizeof(overlay));
	particles = (AVERAGE + lrand48() % DEVIATION);
	x_inc = M_PI_2 * (drand48() * 0.01 + 0.01);
	for (i = 0; i < particles; ++i)
	{
		path = &paths[i];
		path->x = 0.0;
		path->x_mult = drand48() * (double)SCR_X * 0.2;
		path->y_mult = drand48() * (double)SCR_Y * 1.1 + 5.0;
		path->old_y = -1;
		path->old_x = -1;
		path->seq_no = lrand48() % SEQ_COUNT;
		path->sequence = i & 1 ? sequenceA : sequenceB;
		flash();
	}
	while (paths[0].x < M_PI)
	{
		for (i = 0; i < particles; ++i)
		{
			touched = 0;
			path = &paths[i];
			new_x = path->x * path->x_mult + 0.5;
			if (i & 1)
				new_x = -new_x;
			new_y = sin(path->x) * path->y_mult + 0.5;
			draw_y = Y_bang - new_y;
			draw_x = X_bang + new_x;
			toy = path->old_y;
			tox = path->old_x;
			if (LEGAL_YX(toy, tox))
				if (!(--overlay[toy][tox]))
				{
					touched = 1;
					mvwaddch(screen, SCR_ADJ(toy), tox, old_chs[toy][tox]);
				}
			if (LEGAL_YX(draw_y, draw_x))
			{
				wmove(screen, SCR_ADJ(draw_y), draw_x);
				if (!overlay[draw_y][draw_x])
					old_chs[draw_y][draw_x] = winch(screen);
				waddch(screen, path->sequence[path->seq_no++ % SEQ_COUNT]);
				++overlay[draw_y][draw_x];
				touched = 1;
			}
			if (touched)
			{
				flushinp();
				wrefresh(screen);
			}
			path->old_y = draw_y;
			path->old_x = draw_x;
			path->x += x_inc;
		}
		usleep(20 * 1000); /* Added in 2022. In 1989 the sin() call above achieved the same result. */
	}
	for (i = 0; i < particles; ++i)
	{
		path = &paths[i];
		toy = path->old_y;
		tox = path->old_x;
		if (LEGAL_YX(toy, tox))
			mvwaddch(screen, SCR_ADJ(toy), tox, old_chs[toy][tox]);
	}
	wrefresh(screen);
}

static int
FlyLander(WINDOW *screen, double y_move, double x_move)
{
	int land_stat;
	double altitude = ALTITUDE_INIT;
	double longditude = 0.0;

	for (land_stat = FLYING; land_stat == FLYING;)
	{
		sleep(1);
		GetMove(screen, &y_move, &x_move);
		land_stat = MoveLander(screen, altitude, longditude);
		switch (land_stat)
		{
		case FLYING:
			y_move -= GRAVITY;
			altitude += y_move;
			if (altitude < 0.0)
				altitude = 0.0;
			longditude += x_move;
			break;
		case LANDED:
			if (y_move < -(ACCEPTABLE))
			{
				Explode(screen, LastLegalY, LastLegalX);
				land_stat = CRASH;
			}
			else
				UpdateScore(screen);
			break;
		case CRASH:
			Explode(screen, LastLegalY, LastLegalX);
			break;
		}
		wmove(screen, 0, 0);
		wclrtoeol(screen);
		wprintw(screen, "alt: %8.3f X: %8.2f dY: %7.3f dX: %7.3f Score: %5d", altitude, longditude, y_move, x_move, Score);
		wrefresh(screen);
	}
	if (land_stat == LANDED)
	{
		++Landings;
		nodelay(screen, FALSE);
		wmove(screen, 1, 0);
		wprintw(screen, "--Safe Landing Number: %d", Landings);
		waddstr(screen, " - press space bar--");
		while (wgetch(screen) != ' ');
		nodelay(screen, TRUE);
	}
	return land_stat;
}

int
main(int argc, char *argv[])
{
	double init_dy;		/* initial rate of fall per landing */
	double init_dx;		/* initial horizontal velocity per landing */
	WINDOW *screen;		/* main display curses window */
	int LanderStatus;	/* status of lander at end of landing */

	InitialiseScreen(&screen);	/* do basic screen init */
	InitScore();
	do
	{
		/* init screen and lander for game */
		StartLander(&init_dy, &init_dx);
		do
		{
			InitMoves(screen);	/* init lander for one landing */
			DrawScreen(screen);	/* init screen for one landing */
			LanderStatus = FlyLander(screen, init_dy, init_dx);
			init_dy -= INIT_DY_INC;	/* make landing harder */
			init_dx += INIT_DX_INC;
		}
		while (LanderStatus == LANDED);	/* until crash */
	}
	while (CleanUp(screen));	/* while user wants another game */
	EndCurses(screen);	/* final screen cleanup */
	return 0;
}

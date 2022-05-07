# Lunar Lander #

This is a C implementation of the old
["lunar lander"](https://en.wikipedia.org/wiki/Lunar_Lander_(1979_video_game))
game seen in amusement arcades in the seventies.

## History (modified from the original README) ##

I wrote this in 1989 while working for legendary Toronto Unix house
[HCR](https://en.wikipedia.org/wiki/HCR_Corporation). Former versions were
ported to various System 5.3 systems, namely 386/ix and HCR's port of
System 5.3 to a VAX.  It has also been ported to a version of BSD.

Lander utilizes some of the nifty SysV.3 curses capabilities such as line
drawing.  Anyone using an [ANSI compatible terminal](https://en.wikipedia.org/wiki/ANSI_escape_code)
should see the full effect. Assorted [Visual](https://www.vt100.net/visual/)
terminals will give the same results with a decent terminfo file.

This was posted to Usenet group comp.sources.games in 1989. I didn't keep
backups of my stuff back then, so I revived this source from a
[Usenet archive](https://ftp.sunet.se/mirror/archive/ftp.sunet.se/pub/usenet/ftp.uu.net/comp.sources.games/volume6/).

## Building ##

Lander compiles and runs on Debian, Ubuntu, and probably most other Linux ports.
A Curses library and an `nroff` work-alike are required.

```shell
sudo apt install groff-base libncurses-dev
```

If your terminal emulator (xterm, et al) doesn't display line drawing characters
correctly then invert the #if in `InitialiseScreen()` in `screen.c`.

Lander was written for VT100 style terminals. They had 80 columns and 24 rows.
For maximum nostalga resize your terminal emulator to 80x24.

Lander uses a high score file that is writable by all users running the program,
please edit the `Makefile` to ensure a valid name is used.  Lander will attempt to
create the HS file the first time the game is run and a successful landing is
completed.

## Controlling the Lander ##

The lander is fitted with three rockets:

```text
           /~\
           | |
    left  -+++-  right
            |
      vert. thrust
```

Press keys '0' - '9' to set the default power level, where '0' == no power and
'9' == full power.

Pressing 'z' sets the power output of the left rocket to the default, this has
the effect of pushing the lander to the right (if the default power level
is > 0).

Pressing 'c' sets the power output of the right rocket to the default, this has
the effect of pushing the lander to the left.

Pressing 'x' sets the vertical thrust of the middle rocket to the default level,
if this is non zero it will slow the rate of descent of the lander.  If it sets
it to zero the lander will drop like a brick!

## What's on the Screen ##

The top of the screen reads;

```text
    alt:  X:  dY:  dX:  Score:
```

- `alt` is the current altitude from "sea level" in metres
- `X` is how far you have moved horizontally (in meters)
- `dY` is your rate of change of vertical movement, (e.g. -10.0 means you
  fell 10 meters in the previous second)
- `dX` is your rate of change of horizonal movement, (e.g. -10.0 means you moved
  left 10 metres in the previous second)
- `Score` is your current score.

At the bottom of the screen;

```text
    Thrust - L:  vert:  R:  pow:  Fuel:
```

- `L` is thrust from the left rocket (pushes you to the right)
- `R` is thrust from the right rocket (pushes you to the left). Both 'L' and 'R'
  alter the `dX` value.
- `vert` is thrust from the vertical rocket (slows the lander's rate of descent
  i.e. alters `dY`)
- `pow` is the default power setting that will be used when you press 'z', 'x'
  or 'c'
- `Fuel` the amount of fuel left, when this reaches zero all thrust from rockets
  ceases.

## How to win ##

The two rules for a safe landing are;

- Try to get `dY` >= -5 metres per second, anything faster than -5 will cause
  the lander to explode.
- Make sure you are landing on a pad.

The score accumulates with each successful landing. A bonus is added on each
landing for conserved fuel.

For subsequent consecutive landing attempts the initial lander vertical and
horizontal velocity increases.
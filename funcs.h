extern void ScReadDisplay(WINDOW *back_win);
extern void ScWrite(WINDOW *back_win);
extern void GetMove(WINDOW *screen, double *y_delta, double *x_delta);
extern int MoveLander(WINDOW *screen, double land_y, double land_x);
extern void UpdateScore(WINDOW *screen);
extern void InitialiseScreen(WINDOW **init_scr);
extern void InitScore(void);
extern void InitMoves(WINDOW * screen);
extern void DrawScreen(WINDOW *screen);

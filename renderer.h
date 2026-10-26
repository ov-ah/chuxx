#ifndef RENDERER_H
#define RENDERER_H

#define STATUS_LEN 128
extern char status_line[STATUS_LEN]

#include "gamestate.h"

    void renderer_init(void);
void renderer_shutdown(void);
void renderer_draw(const GameState *gs, int sel_row, int sel_col);
void renderer_set_perspective(PieceColor side);
void renderer_screen_delta(int *dr, int *dc);
int renderer_screen_to_square(int y, int x, int *row, int *col);

#endif

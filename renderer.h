#ifndef RENDERER_H
#define RENDERER_H

#include "gamestate.h"

void renderer_init(void);
void renderer_shutdown(void);
void renderer_draw(const GameState *gs, int sel_row, int sel_col);
void renderer_set_perspective(PieceColor side);
int renderer_screen_to_square(int y, int x, int *row, int *col);

#endif

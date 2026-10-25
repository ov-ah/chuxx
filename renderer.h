#ifndef RENDERER_H
#define RENDERER_H

void renderer_init(void);
void renderer_shutdown(void);
void renderer_draw(const GameState *gs, int sel_row, int sel_col);
void renderer_screen_to_square(int y, int x, int *row, int *col);

#endif

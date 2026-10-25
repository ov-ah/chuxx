
#include "net.h"
#include "renderer.h"
#include <curses.h>

int main(void)
{
	// GameState gs;
	// gamestate_init(&gs);
	GameState gs = {0};
	gs.board[7][4] = (Piece){KING, WHITE};
	gs.board[0][4] = (Piece){KING, BLACK};

	renderer_init();
	renderer_draw(&gs, -1, -1);

	int ch;
	while ((ch = getch()) != 'q')
		if (ch == KEY_RESIZE)
			renderer_draw(&gs, -1, -1);

	renderer_shutdown();
	return 1;
}

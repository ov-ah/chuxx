
#include "net.h"
#include "renderer.h"
#include <curses.h>

static int clamp(int v, int lo, int hi)
{
	return v < lo ? lo : (v > hi ? hi : v);
}

int main(void)
{
	// GameState gs;
	// gamestate_init(&gs);
	GameState gs = {0};
	gs.board[7][4] = (Piece){KING, WHITE};
	gs.board[0][4] = (Piece){KING, BLACK};

	renderer_init();
	int sel_row = 7, sel_col = 4;
	renderer_draw(&gs, sel_row, sel_col);

	int ch;
	while ((ch = getch()) != 'q')
	{
		switch (ch)
		{
		case KEY_UP:
		case 'k':
			sel_row--;
			break;
		case KEY_DOWN:
		case 'j':
			sel_row++;
			break;
		case KEY_LEFT:
		case 'h':
			sel_col--;
			break;
		case KEY_RIGHT:
		case 'l':
			sel_col++;
			break;
		case KEY_RESIZE:
			break;
		default:
			continue;
		}

		sel_row = clamp(sel_row, 0, BOARD_ROWS - 1);
		sel_col = clamp(sel_col, 0, BOARD_COLS - 1);

		renderer_draw(&gs, sel_row, sel_col);
	}

	renderer_shutdown();
	return 1;
}

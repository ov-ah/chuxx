#include "net.h"
#include "renderer.h"
#include <curses.h>

static int clamp(int v, int lo, int hi)
{
	return v < lo ? lo : (v > hi ? hi : v);
}

int main(void)
{
	GameState gs;
	gamestate_init(&gs);

	renderer_init();
	int sel_row = 7, sel_col = 4;
	int mov_row = 0, mov_col = 0;
	int mov_state = 0;
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
		case KEY_ENTER:
		case '\n':
		case '\r':
			if (!mov_state)
			{
				mov_row = sel_row;
				mov_col = sel_col;
				mov_state = 1;
				break;
			}

			fprintf(stderr, "[MOVE] from: %d, %d\t to: %d, %d", mov_row,
			        mov_col, sel_row, sel_col);
			apply_move(&gs, (Move){mov_row, mov_col, sel_row, sel_col});
			mov_state = 0;
			break;
			// TODO: update renderer_draw to select more than one ro
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

#include "net.h"
#include "renderer.h"
#include <curses.h>
#include <string.h>

static int clamp(int v, int lo, int hi)
{
	return v < lo ? lo : (v > hi ? hi : v);
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "usage: chuxx [host|client]\n");
		return 1;
	}

	if (strcmp(argv[1], "host") == 0)
	{
		// start host
	}
	else if (strcmp(argv[1], "client") == 0)
	{
		if (argc < 3)
		{
			fprintf(stderr, "usage: chuxx client IP:PORT\n");
			return 1;
		}
		// parse argv[2] as IP:PORT
	}
	else
	{
		fprintf(stderr, "unknown mode '%s'\nusage: chuxx [host|client]\n",
		        argv[1]);
		return 1;
	}
	if (argc < 2)
	{
		fprintf(stderr, "usage: chuxx [host/client]\n");
		return 1;
	}

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
			Move mov = {mov_row, mov_col, sel_row, sel_col};
			char mov_str[8];
			move_to_string(mov, mov_str);

			if (is_legal(&gs, mov) == 1)
				apply_move(&gs, mov);
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

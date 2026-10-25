#include "renderer.h"
#include <curses.h>

// pieces are black/white
// board black = blue and white = green

/*
R N B Q K B N R

x x x x x x x x
x x x x x x x x
x x x x x x x x
x x x x x x x x

R N B Q K B N R
*/

#define SQ_W 5
#define SQ_H 2

#define LABEL_W 2
#define LABEL_H 1

#define LIGHT_BG COLOR_GREEN
#define DARK_BG COLOR_BLUE
#define SELECTED_BG COLOR_YELLOW
#define WHITE_PIECE COLOR_WHITE
#define BLACK_PIECE COLOR_BLACK

static int board_y, board_x;
static int board_visible = 0; // treat as bool

typedef enum
{
	LIGHT,
	DARK,
	SELECTED,
	SHADE_COUNT
} Shade;

#define LABEL_PAIR (1 + SHADE_COUNT * 2)

static int pair_for(int shade, int pcolor)
{
	if (pcolor == NONE)
		pcolor = WHITE;
	return 1 + shade * 2 + pcolor;
}

static void init_colors(void)
{
	if (!has_colors())
		return;

	start_color();
	for (int s = LIGHT; s < SHADE_COUNT; s++)
	{
		short bg = (s == LIGHT)  ? LIGHT_BG
		           : (s == DARK) ? DARK_BG
		                         : SELECTED_BG;

		for (int p = WHITE; p <= BLACK; p++)
		{
			int fg = (p == WHITE) ? WHITE_PIECE : BLACK_PIECE;
			init_pair(pair_for(s, p), fg, bg);
		}
	}
	init_pair(LABEL_PAIR, COLOR_YELLOW, COLOR_BLACK);
}

static int square_attr(int row, int col, PieceColor pc, int selected)
{
	Shade shade = selected ? SELECTED : ((row + col) % 2 == 0) ? LIGHT : DARK;

	int attr = COLOR_PAIR(pair_for(shade, pc));
	if (pc == WHITE)
		attr |= A_BOLD;

	return attr;
}

static void fill_rect(int y, int x, int h, int w, chtype attr)
{
	for (int dy = 0; dy < h; dy++)
		mvhline(y + dy, x, ' ' | attr, w);
}

static char piece_glyph(Piece p)
{
	static const char glyphs[] = " PNBRQK";
	char g = glyphs[p.type];
	return (p.color == BLACK) ? g + ('a' - 'A') : g;
}

void renderer_draw(const GameState *gs, int sel_row, int sel_col)
{
	int board_w = BOARD_COLS * SQ_W;
	int board_h = BOARD_ROWS * SQ_H;
	int total_w = board_w + LABEL_W;
	int total_h = board_h + LABEL_H;

	int term_h, term_w;
	getmaxyx(stdscr, term_h, term_w);
	erase();

	if (total_w > term_w || total_h > term_h)
	{
		board_visible = 0;
		mvprintw(0, 0, "term too small: need %dx%d, have %dx%d", total_w,
		         total_h, term_w, term_h);
		refresh();
		return;
	}

	int top = (term_h - total_h) / 2;
	int left = (term_w - total_w) / 2;
	board_y = top;
	board_x = left + LABEL_W;
	board_visible = 1;

	for (int r = 0; r < BOARD_ROWS; r++)
	{
		for (int c = 0; c < BOARD_COLS; c++)
		{
			Piece p = gs->board[r][c];
			int selected = (r == sel_row && c == sel_col);
			chtype attr = square_attr(r, c, p.color, selected);

			int y = board_y + r * SQ_H;
			int x = board_x + c * SQ_W;
			fill_rect(y, x, SQ_H, SQ_W, attr);

			if (p.type != EMPTY)
				mvaddch(y + SQ_H / 2, x + SQ_W / 2, piece_glyph(p) | attr);
		}
	}

	attron(COLOR_PAIR(LABEL_PAIR));
	for (int r = 0; r < BOARD_ROWS; r++)
		mvprintw(board_y + r * SQ_H + SQ_H / 2, left, "%d", BOARD_ROWS - r);
	for (int c = 0; c < BOARD_COLS; c++)
		mvaddch(board_y + board_h, board_x + c * SQ_W + SQ_W / 2, 'a' + c);
	attroff(COLOR_PAIR(LABEL_PAIR));

	refresh();
}

int renderer_screen_to_square(int y, int x, int *row, int *col)
{
	if (!board_visible)
		return 0;

	int dy = y - board_y;
	int dx = x - board_x;

	if (dy < 0 || dx < 0 || dy >= BOARD_ROWS * SQ_H || dx >= BOARD_COLS * SQ_W)
		return 0;

	*row = dy / SQ_H;
	*col = dx / SQ_W;
	return 1;
}

void renderer_init()
{
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
	init_colors();
}

void renderer_shutdown() { endwin(); }

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

#define ROWS 8
#define COLS 8
#define SQ_W 7
#define SQ_H 3

#define LABEL_W 2
#define LABEL_H 1

#define LIGHT_BG COLOR_GREEN
#define DARK_BG COLOR_BLUE
#define SELECTED_BG COLOR_YELLOW
#define WHITE_PIECE COLOR_WHITE
#define BLACK_PIECE COLOR_BLACK

typedef enum
{
	LIGHT,
	DARK,
	SELECTED,
	SHADE_COUNT
} Shade;

typedef enum
{
	WHITE,
	BLACK,
	NONE,
	PCOLOR_COUNT
} PieceColor;

#define LABEL_PAIR (1 + SHADE_COUNT * 2)

// pair # = 1 + shade*2 + pcolor -> 1..4
int pair_for(int shade, int pcolor)
{
	if (!pcolor)
		pcolor = WHITE;
	return 1 + shade * 2 + pcolor;
}

void init_colors()
{
	if (!has_colors())
		return;

	start_color();
	for (int s = LIGHT; s < SHADE_COUNT; s++)
	{
		short bg = (s == LIGHT)  ? LIGHT_BG
		           : (s == DARK) ? DARK_BG
		                         : SELECTED_BG;

		for (int p = WHITE; p < BLACK; p++)
		{
			int fg = (p == WHITE) ? WHITE_PIECE : BLACK_PIECE;
			init_pair(pair_for(s, p), fg, bg);
		}
	}
	init_pair(LABEL_PAIR, COLOR_YELLOW, COLOR_BLACK);
}

int square_attr(int row, int col, PieceColor pc, int selected)
{
	Shade shade = selected ? SELECTED : ((row + col) % 2 == 0) ? LIGHT : DARK;

	int attr = COLOR_PAIR(pair_for(shade, pc));
	if (pc == WHITE)
		attr |= A_BOLD;

	return attr;
}

void fill_rect(int y, int x, int h, int w, chtype attr)
{
	for (int dy = 0; dy < h; dy++)
		mvhline(y + dy, x, ' ' | attr, w);
}

void draw_board(void)
{
	int board_w = COLS * SQ_W;
	int board_h = ROWS * SQ_H;
	int total_w = board_w + LABEL_W;
	int total_h = board_h + LABEL_H;

	int term_h, term_w;
	getmaxyx(stdscr, term_h, term_w);
	erase();

	if (total_w > term_w || total_h > term_h)
	{
		mvprintw(0, 0, "term too small: need %dx%d, have %dx%d", total_w,
		         total_h, term_w, term_h);
		refresh();
		return;
	}

	int top = (term_h - total_h) / 2;
	int left = (term_w - total_w) / 2;
	int board_y = top;
	int board_x = left + LABEL_W;

	for (int r = 0; r < ROWS; r++)
		for (int c = 0; c < COLS; c++)
			fill_rect(board_y + r * SQ_H, board_x + c * SQ_W, SQ_H, SQ_W,
			          square_attr(r, c, NONE, false));

	attron(COLOR_PAIR(LABEL_PAIR));
	for (int r = 0; r < ROWS; r++)
		mvprintw(board_y + r * SQ_H + SQ_H / 2, left, "%d", ROWS - r);
	for (int c = 0; c < COLS; c++)
		mvaddch(board_y + board_h, board_x + c * SQ_W + SQ_W / 2, 'a' + c);
	attroff(COLOR_PAIR(LABEL_PAIR));

	refresh();
}

int main()
{
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
	init_colors();

	draw_board();

	int ch;
	while ((ch = getch()) != 'q')
		if (ch == KEY_RESIZE)
			draw_board();

	endwin();
	return 0;
}

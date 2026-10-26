#include "net.h"
#include "renderer.h"
#include <curses.h>
#include <errno.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int clamp(int v, int lo, int hi)
{
	return v < lo ? lo : (v > hi ? hi : v);
}

static void set_turn_status(const GameState *gs, PieceColor me)
{
	snprintf(status_line, sizeof status_line, "%s to move (%s)",
	         gs->to_move == WHITE ? "White" : "Black",
	         gs->to_move == me ? "your move" : "waiting...");
}

static void set_bad_move_status(Move mv)
{
	char mv_str[16];
	move_to_string(mv, mv_str);
	snprintf(status_line, sizeof status_line, "%s is illegal", mv_str);
}

int main(int argc, char **argv)
{
	int fd = -1;
	PieceColor my_color;

	if (argc < 2)
	{
		fprintf(stderr, "usage: chuxx [host|client]\n");
		return 1;
	}

	if (strcmp(argv[1], "host") == 0)
	{
		int lfd = host_listen(8080);
		if (lfd < 0)
			return 1;
		fprintf(stderr, "waiting for opponent...\n");
		fd = host_accept(lfd);
		close(lfd);
		if (fd < 0)
			return 1;
		my_color = WHITE;
	}
	else if (strcmp(argv[1], "client") == 0)
	{
		if (argc < 3)
		{
			fprintf(stderr, "usage: chuxx client IP:PORT\n");
			return 1;
		}
		char ip[64];
		int port = 8080;
		char *colon = strrchr(argv[2], ':');
		size_t n = colon ? (size_t)(colon - argv[2]) : strlen(argv[2]);
		if (n >= sizeof(ip))
			return 1;
		memcpy(ip, argv[2], n);
		ip[n] = '\0';
		if (colon)
			port = atoi(colon + 1);

		fd = client_connect(ip, port);
		if (fd < 0)
			return 1;
		my_color = BLACK;
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
	renderer_set_perspective(my_color);
	set_turn_status(&gs, my_color);
	int sel_row = 7, sel_col = 4;
	int mov_row = 0, mov_col = 0;
	int mov_state = 0;
	renderer_draw(&gs, sel_row, sel_col);

	int ch;
	while (1)
	{
		renderer_draw(&gs, sel_row, sel_col);

		struct pollfd fds[2] = {
		    {.fd = STDIN_FILENO, .events = POLLIN},
		    {.fd = fd, .events = POLLIN},
		};

		if (poll(fds, 2, -1) < 0)
		{
			if (errno == EINTR)
				continue;
			break;
		}

		if (fds[1].revents & (POLLIN | POLLHUP | POLLERR))
		{
			char buf[16];
			int n = recv_line(fd, buf, sizeof(buf));
			if (n <= 0)
				break;
			if (gs.to_move == my_color)
				break;
			Move om;
			if (!parse_move(buf, &om) || !is_legal(&gs, om))
				break;
			apply_move(&gs, om);
			continue;
		}

		if (fds[0].revents & POLLIN)
		{
			ch = getch();
			if (ch == 'q')
				break;

			if (gs.to_move != my_color)
				continue;

			int dr = 0, dc = 0;
			switch (ch)
			{
			case KEY_UP:
			case 'k':
				dr = -1;
				break;
			case KEY_DOWN:
			case 'j':
				dr = 1;
				break;
			case KEY_LEFT:
			case 'h':
				dc = 1;
				break;
			case KEY_RIGHT:
			case 'l':
				dc = -1;
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
				{
					Move mov = {mov_row, mov_col, sel_row, sel_col, EMPTY};
					mov_state = 0;
					if (is_legal(&gs, mov))
					{
						apply_move(&gs, mov);
						char mov_str[8];
						move_to_string(mov, mov_str);
						if (send_line(fd, mov_str) < 0)
							goto done;
					}
					set_bad_move_status(mov);
				}
				break;
			case KEY_RESIZE:
				break;
			default:
				continue;
			}

			if (dr || dc)
			{
				renderer_screen_delta(&dr, &dc);
				sel_row = clamp(sel_row + dr, 0, BOARD_ROWS - 1);
				sel_col = clamp(sel_col + dc, 0, BOARD_COLS - 1);
			}
		}
	}
done:
	renderer_shutdown();
	close(fd);
	return 0;
}

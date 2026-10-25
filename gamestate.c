#include "gamestate.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void gamestate_init(GameState *gs)
{
    for (int r = 0; r < BOARD_ROWS; r++)
    {
        for (int c = 0; c < BOARD_COLS; c++)
        {
            gs->board[r][c] = (Piece){EMPTY, NONE};
        }
    }

    PieceType back[BOARD_COLS] = {
        ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK};

    for (int c = 0; c < BOARD_COLS; c++)
    {
        gs->board[0][c] = (Piece){back[c], WHITE};
        gs->board[1][c] = (Piece){PAWN, WHITE};
        gs->board[6][c] = (Piece){PAWN, BLACK};
        gs->board[7][c] = (Piece){back[c], BLACK};
    }

    gs->to_move = WHITE;
    gs->castle_wk = 1;
    gs->castle_wq = 1;
    gs->castle_bk = 1;
    gs->castle_bq = 1;
    gs->ep_row = -1;
    gs->ep_col = -1;
}

void parse_move(const char *s, Move *m)
{
    size_t len = strlen(s);
    if (s == NULL || (len != 4 && len != 5))
    {
        fprintf(stderr, "Bad text string! Should be in format (from square)(to square)(flags)");
        return;
    }

    int fr = s[0] - '1';
    int fc = s[1] - 'a';
    int tr = s[2] - '1';
    int tc = s[3] - 'a';

    if (fr < 0 || fr > 7 || fc < 0 || fc > 7 ||
        tr < 0 || tr > 7 || tc < 0 || tc > 7)
    {
        fprintf(stderr, "Parsed bad sqaure!");
        return;
    }

    PieceType promo = EMPTY; // no promotion by default
    if (len == 5)
    {
        switch (s[4])
        {
            case 'q':
                promo = QUEEN;
                break;
            case 'r':
                promo = ROOK;
                break;
            case 'b':
                promo = BISHOP;
                break;
            case 'n':
                promo = KNIGHT;
                break;
            default:
                fprintf(stderr, "Parsed bad flag!\n");
                return;
        }
    }
    m->from_row = fr;
    m->from_col = fc;
    m->to_row = tr;
    m->to_col = tc;
    m->promo = promo;
}

void move_to_string(Move m, char *buf)
{
    buf[0] = 'a' + m.from_row;
    buf[1] = '1' + m.from_col;
    buf[2] = 'a' + m.to_row;
    buf[3] = '1' + m.to_col;

    int i = 4;
    switch (m.promo)
    {
        case QUEEN:
            buf[i++] = 'q';
            break;
        case ROOK:
            buf[i++] = 'r';
            break;
        case BISHOP:
            buf[i++] = 'b';
            break;
        case KNIGHT:
            buf[i++] = 'n';
            break;
        default:
            break;
    }
    buf[i] = '\0';
}

void apply_move(GameState *gs, Move m)
{
    Piece p = gs->board[m.from_row][m.from_col];

    int is_ep = p.type == PAWN && gs->board[m.to_row][m.to_col].type == EMPTY && m.from_col != m.to_col;
    int is_castle = p.type == KING && abs((m.to_col) - (m.from_col)) == 2;
    int last_row = (p.color == WHITE) ? 7 : 0;
    int is_promo = p.type == PAWN && m.to_row == last_row;
    int is_double = p.type == PAWN && abs((m.to_row) - (m.from_row)) == 2;

    gs->board[m.to_row][m.to_col] = p;
    gs->board[m.from_row][m.from_col] = (Piece){EMPTY, NONE};

    if (is_ep)
    {
        gs->board[m.from_row][m.to_col] = (Piece){EMPTY, NONE};
    }
    if (is_castle)
    {
        int rook_from, rook_to;
        if (m.to_col > m.from_col)
        {
            rook_from = 7;
            rook_to = 5;
        }
        else
        {
            rook_from = 0;
            rook_to = 3;
        }
        gs->board[m.to_row][rook_to] = gs->board[m.to_row][rook_from];
        gs->board[m.to_row][rook_from] = (Piece){EMPTY, NONE};
    }
    if (is_promo)
    {
        if (m.promo != EMPTY)
            gs->board[m.to_row][m.to_col].type = m.promo;
        else
            gs->board[m.to_row][m.to_col].type = QUEEN;
    }

    if (p.type == KING)
    {
        switch (p.color)
        {
            case WHITE:
                gs->castle_wk = gs->castle_wq = 0;
                break;
            case BLACK:
                gs->castle_bk = gs->castle_bq = 0;
                break;
            default:
                break;
        }
    }

    if (p.type == ROOK)
    {
        if (p.color == WHITE && m.from_row == 0)
        {
            if (m.from_col == 0)
                gs->castle_wq = 0;
            if (m.from_col == 7)
                gs->castle_wk = 0;
        }
        else if (p.color == BLACK && m.from_row == 7)
        {
            if (m.from_col == 0)
                gs->castle_bq = 0;
            if (m.from_col == 7)
                gs->castle_bk = 0;
        }
    }

    if (m.to_row == 0 && m.to_col == 0)
        gs->castle_wq = 0;
    if (m.to_row == 0 && m.to_col == 7)
        gs->castle_wk = 0;
    if (m.to_row == 7 && m.to_col == 0)
        gs->castle_bq = 0;
    if (m.to_row == 7 && m.to_col == 7)
        gs->castle_bk = 0;

    gs->ep_row = -1;
    gs->ep_col = -1;

    if (is_double)
    {
        gs->ep_row = (m.from_row + m.to_row) / 2;
        gs->ep_col = m.from_col;
    }

    gs->to_move = (gs->to_move == WHITE) ? BLACK : WHITE;
}

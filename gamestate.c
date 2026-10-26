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

int parse_move(const char *s, Move *m)
{
    size_t len = strlen(s);
    if (s == NULL || (len != 4 && len != 5))
    {
        fprintf(stderr, "Bad text string! Should be in format (from square)(to square)(flags)\n");
        return 0;
    }

    int fc = s[0] - 'a';
    int fr = s[1] - '1';
    int tc = s[2] - 'a';
    int tr = s[3] - '1';

    if (fr < 0 || fr > 7 || fc < 0 || fc > 7 ||
        tr < 0 || tr > 7 || tc < 0 || tc > 7)
    {
        fprintf(stderr, "Parsed bad sqaure!\n");
        return 0;
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
                return 0;
        }
    }
    m->from_row = fr;
    m->from_col = fc;
    m->to_row = tr;
    m->to_col = tc;
    m->promo = promo;
    return 1;
}

void move_to_string(Move m, char *buf)
{
    buf[0] = 'a' + m.from_col;
    buf[1] = '1' + m.from_row;
    buf[2] = 'a' + m.to_col;
    buf[3] = '1' + m.to_row;

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

static int path_clear(const GameState *gs, Move m)
{
    int step_row, step_col;
    int row_distance = m.to_row - m.from_row;
    int col_distance = m.to_col - m.from_col;

    if (row_distance < 0)
        step_row = -1;
    else if (row_distance == 0)
        step_row = 0;
    else
        step_row = 1;

    if (col_distance < 0)
        step_col = -1;
    else if (col_distance == 0)
        step_col = 0;
    else
        step_col = 1;

    int r = m.from_row + step_row;
    int c = m.from_col + step_col;

    while (r != m.to_row || c != m.to_col)
    {
        if (gs->board[r][c].type != EMPTY)
            return 0;
        r += step_row;
        c += step_col;
    }
    return 1;
}

static int is_pseudo_legal(const GameState *gs, Move m)
{
    if (m.from_row < 0 || m.from_row > 7 || m.to_row < 0 || m.to_row > 7 ||
        m.from_col < 0 || m.from_col > 7 || m.to_col < 0 || m.to_col > 7)
        return 0;

    if (m.from_row == m.to_row && m.from_col == m.to_col)
        return 0;

    Piece p = gs->board[m.from_row][m.from_col];
    Piece target = gs->board[m.to_row][m.to_col];

    if (p.type == EMPTY)
        return 0;

    if (p.color != gs->to_move)
        return 0;

    if (target.color == p.color)
        return 0;

    int dr = m.to_row - m.from_row;
    int dc = m.to_col - m.from_col;
    int dir = (p.color == WHITE) ? 1 : -1;

    switch (p.type)
    {
        case KNIGHT:
            return (abs(dr) == 1 && abs(dc) == 2) || (abs(dr) == 2 && abs(dc) == 1);
        case ROOK:
            return (dr == 0 || dc == 0) && path_clear(gs, m);
        case BISHOP:
            return (abs(dr) == abs(dc)) && path_clear(gs, m);
        case QUEEN:
            return ((dr == 0 || dc == 0) || (abs(dr) == abs(dc))) && path_clear(gs, m);
        case KING:
            return (abs(dr) <= 1 && abs(dc) <= 1);
        case PAWN:
            if (dc == 0 && dr == dir && target.type == EMPTY)
                return 1;
            if (dc == 0 && dr == 2 * dir && target.type == EMPTY &&
                m.from_row == ((p.color == WHITE) ? 1 : 6) && path_clear(gs, m))
                return 1;
            if (abs(dc) == 1 && dr == dir && target.type != EMPTY)
                return 1;
            if (abs(dc) == 1 && dr == dir && m.to_row == gs->ep_row && m.to_col == gs->ep_col)
                return 1;
            return 0;
        default:
            return 0;
    }
}

int is_legal(const GameState *gs, Move m)
{
    return is_pseudo_legal(gs, m);
}

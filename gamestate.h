#ifndef GAMESTATE_H
#define GAMESTATE_H

#define BOARD_ROWS 8
#define BOARD_COLS 8

typedef enum
{
    EMPTY,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
} PieceType;

typedef enum
{
    WHITE,
    BLACK,
    NONE
} PieceColor;

typedef struct
{
    PieceType type;
    PieceColor color;
} Piece;

typedef struct
{
    int from_row, from_col;
    int to_row, to_col;
    PieceType promo; // only for pawn promotion
} Move;

typedef struct
{
    Piece board[BOARD_ROWS][BOARD_COLS];
    PieceColor to_move;
    int castle_wk, castle_wq; // treat as bool: white king/queen castle allowed?
    int castle_bk, castle_bq; // same for black
    int ep_row, ep_col;
} GameState;

void gamestate_init(GameState *gs);
int parse_move(const char *s, Move *m);
void move_to_string(Move m, char *buf);
void apply_move(GameState *gs, Move m);
int is_legal(const GameState *gs, Move m); // treat as bool

#endif

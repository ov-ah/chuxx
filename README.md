# chuxx

multiplayer chess in almost any unix compliant terminal, with networking. written in pure C with ncurses.

**[2nd place winner at Hacktoberfest 2026 × QuackHacks](https://github.com/ov-ah/chuxx/tree/hacktoberfest)** 

## building

needs a C compiler, ncurses and linux.

```sh
make            # -> build/chuxx
```

## playing

```sh
./build/chuxx host
./build/chuxx client 192.168.1.20:8080
```

arrow keys or vim binds to move selection.
enter to pickup a piece and enter again to place the piece.
q to quit.

pawns auto-promote to a queen.

## how it works

- `gamestate.c` handles the board, move validation, castling/en passant
- `renderer.c` handles ncurses board, flipped to each player's side
- `net.c` handles TCP; each move is one line of text like `e2e4` which gamestate parses
- `main.c` with `poll()` loops over the keyboard and the socket

both sides validate every move and an illegal move from the opponent ends the game.

## to be implemented

check, checkmate, game clock, persistant player scores, network improvements for non LAN connections, and small rendering improvements like better color support for terminals which support it.

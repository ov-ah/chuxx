CC = clang
CFLAGS = -Wall -Wextra -g
LDLIBS = -lncurses

build/chuxx: main.c renderer.c gamestate.c net.c
	mkdir -p build
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@ 

clean: 
	rm -f build/chuxx

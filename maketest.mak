ALL:
	gcc -o test.out ./tests/test.c ./src/game.c -lX11 -L/usr/X11R6/lib

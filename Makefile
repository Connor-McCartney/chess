# make clean; make chess; ./chess

chess:
	gcc chess.c -o chess -lncursesw -Wall -Werror -Wextra -pedantic

clean:
	rm chess

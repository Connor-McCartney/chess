# make clean; make chess; ./chess

chess:
	gcc chess.c pieces.c moves.c engine.c -o chess -lncursesw -Wall -Werror -Wextra -pedantic

clean:
	rm chess

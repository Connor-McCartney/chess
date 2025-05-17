# make clean; make chess; ./chess

chess:
	gcc chess.c pieces.c moves.c linked_list.c -o chess -lncursesw -Wall -Werror -Wextra -pedantic

clean:
	rm chess

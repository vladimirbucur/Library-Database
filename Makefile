# Copyright 2022 Bucur Vladimir 311CA<vladimirbucur1@gmail.com>

build: main.c book.c user.c tops.c hashtable.c list.c
	gcc -Wall -Wextra -std=c99 main.c book.c user.c tops.c hashtable.c list.c -o main -g

run:
	valgrind ./main

clean:
	rm -f main

.PHONY: clean
// Copyright 2022 Bucur Vladimir 311CA<vladimirbucur1@gmail.com>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"
#include "list.h"
#include "book.h"
#include "user.h"
#include "tops.h"
#include "die.h"

#define COMMAND_MAX 100
#define HMAX 1

int main(void)
{
	// creeez hashtable ul in care voi retine cartile din biblioteca
    hashtable_t *library = ht_create(HMAX, hash_function_string,
				compare_function_strings);
	// creeez hashtable ul in care voi retine userii bibliotecii
	hashtable_t *users = ht_create(HMAX, hash_function_string,
				compare_function_strings);

	char command[COMMAND_MAX];
	fgets(command, COMMAND_MAX, stdin);
	while ((strstr(command, "EXIT") - command)) {
		strcpy(command + strlen(command) - 1, command + strlen(command));
		if ((strstr(command, "ADD_BOOK") - command) == 0) {
			command_add_book(library, command);
		} else if ((strstr(command, "GET_BOOK") - command) == 0) {
			command_get_book(library, command);
		} else if ((strstr(command, "RMV_BOOK") - command) == 0) {
			command_rmv_book(library, command);
		} else if ((strstr(command, "ADD_DEF") - command) == 0) {
			command_add_def(library, command);
		} else if ((strstr(command, "GET_DEF") - command) == 0) {
			command_get_def(library, command);
		} else if ((strstr(command, "RMV_DEF") - command) == 0) {
			command_rmv_def(library, command);
		} else if ((strstr(command, "ADD_USER") - command) == 0) {
			command_add_user(users, command);
		} else if ((strstr(command, "BORROW") - command) == 0) {
			command_borrow(library, users, command);
		} else if ((strstr(command, "RETURN") - command) == 0) {
			command_return(library, users, command);
		} else if ((strstr(command, "LOST") - command) == 0) {
			command_lost(library, users, command);
		} else {
			printf("Invalid command. Please try again.\n");
		}
		fgets(command, COMMAND_MAX, stdin);
	}

	top_book(library);
	top_users(users);

	free_library(&library);
	free_users(&users);

    return 0;
}

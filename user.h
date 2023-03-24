// Copyright 2022 Bucur Vladimir 311CA<vladimirbucur1@gmail.com>

#ifndef USER_H_
#define USER_H_

#include <stdio.h>
#include "hashtable.h"
#include "book.h"

#define BOOK_MAX 40
#define USER_MAX 20

typedef struct user_t {
    // se retine numele cartii imprumutate,iar in cazul in care nu
    // este nici o carte imprumutata, book_name va fi gol
    char user_name[USER_MAX], book_name[BOOK_MAX];
    // 1 daca este banat si 0 altfel
    int score, is_banned, no_days;
} user_t;

void
add_user(hashtable_t *ht_users, user_t *user, char *user_name);

void
command_add_user(hashtable_t* ht_users, char command[]);

void
command_borrow(hashtable_t *ht_library, hashtable_t *ht_users, char command[]);

void
command_return(hashtable_t *ht_library, hashtable_t *ht_users, char command[]);

void
command_lost(hashtable_t *ht_library, hashtable_t *ht_users, char command[]);

void
free_users(hashtable_t **users);

#endif  // USER_H_

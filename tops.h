// Copyright 2022 Bucur Vladimir 311CA<vladimirbucur1@gmail.com>

#ifndef TOPS_H_
#define TOPS_H_

#include <stdio.h>
#include "hashtable.h"
#include "book.h"

void
swap_book(book_t **x, book_t **y);

int
condition_rating(book_t *book1, book_t *book2);

void
top_book(hashtable_t *ht_libary);

void
swap_user(user_t **x, user_t **y);

int
condition_score(user_t *score1, user_t *score2);

void
top_users(hashtable_t *ht_users);

#endif  // TOPS_H_

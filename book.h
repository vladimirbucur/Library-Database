// Copyright 2022 Bucur Vladimir 311CA<vladimirbucur1@gmail.com>

#ifndef BOOK_H_
#define BOOK_H_

#include <stdio.h>
#include "hashtable.h"

typedef struct book_t {
    char* name;
    hashtable_t* ht_book;
    float rating;
    // 1 daca este imprumtata si 0 altfel
    // sum reprezinta suma tuturor ratingurilor
    int purchases, is_borrowed, sum;
} book_t;

void
add_book(hashtable_t* ht_library, book_t *book, char* book_name);

void
scan_def(info* def, char command[]);

void
command_add_book(hashtable_t* ht_library, char command[]);

book_t *
get_book(hashtable_t *library, char book_name[]);

void
command_get_book(hashtable_t *library, char command[]);

void
rmv_book(hashtable_t *ht_library, char *book_name);

void
command_rmv_book(hashtable_t *ht_library, char command[]);

void
add_def(hashtable_t* ht_book, info *def, unsigned int k_s, unsigned int v_s);

void
command_add_def(hashtable_t* ht_library, char command[]);

char *
get_def(hashtable_t *library, char book_name[], char def_key[]);

void
command_get_def(hashtable_t *library, char command[]);

void
rmv_def(hashtable_t *library, char book_name[], char def_name[]);

void
command_rmv_def(hashtable_t *ht_library, char command[]);

void
free_book(book_t **book);

void
free_library(hashtable_t **library);


#endif  // BOOK_H_

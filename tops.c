// Copyright 2022 Bucur Vladimir 311CA<vladimirbucur1@gmail.com>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "hashtable.h"
#include "book.h"
#include "user.h"
#include "tops.h"
#include "die.h"

#define MAX_STRING_SIZE	256
#define HMAX 10
#define COMMAND_MAX 100

// functie ce realizeaza interschimbarea a 2 carti
void
swap_book(book_t **x, book_t **y)
{
    book_t* aux = *x;
    *x = *y;
    *y = aux;
}

// returneaza 1 daca elementele nu sunt ordonare bine si 0 altfel
int
condition_rating(book_t *book1, book_t *book2)
{
    if (book1->rating < book2->rating)
        return 1;

    if (book1->rating == book2->rating &&
        book1->purchases < book2->purchases)
        return 1;

    if (book1->rating == book2->rating &&
        book1->purchases == book2->purchases &&
        strcmp(book1->name, book2->name) > 0)
        return 1;

    return 0;
}

void
top_book(hashtable_t *ht_libary)
{
    book_t *ratings[ht_libary->size];
    int nr = 0;  // numarul de elemente din ratings
    for (unsigned int i = 0; i < ht_libary->hmax; i++) {
        linked_list_t *bucket = ht_libary->buckets[i];
        ll_node_t *node = bucket->head;

        while (node) {
            book_t *book = ((info *)(node->data))->value;
            ratings[nr++] = book;

            node = node->next;
        }
    }

    // sortez vectorul
    for (int i = 0; i < nr; i++) {
        for (int j = i + 1; j < nr; j++) {
            // in cazul in care conditiile cerute nu sunt indeplinite
            // se va realiza interschimbarea cartilor in vector
            if (condition_rating(ratings[i], ratings[j])) {
                    swap_book(&ratings[i], &ratings[j]);
            }
        }
    }

    // se afiseaza topul cartilor
    printf("Books ranking:\n");
    for (int i = 0; i < nr; i++)
        printf("%d. Name:%s Rating:%.3f Purchases:%d\n", i+1, ratings[i]->name,
               ratings[i]->rating, ratings[i]->purchases);
}

// functie ce realizeaza interschimbarea a 2 useri
void
swap_user(user_t **x, user_t **y)
{
    user_t* aux = *x;
    *x = *y;
    *y = aux;
}

// returneaza 1 daca elementele nu sunt ordonare bine si 0 altfel
int
condition_score(user_t *user1, user_t *user2)
{
    if (user1->score < user2->score)
        return 1;

    if (user1->score == user2->score &&
        strcmp(user1->user_name, user2->user_name) > 0)
        return 1;

    return 0;
}

// functie ce afiseaza userii in ordinea mentionata in cerinta
void
top_users(hashtable_t *ht_users)
{
    user_t *score[ht_users->size];
    int nr = 0;  // numarul de elemente din scores
    for (unsigned int i = 0; i < ht_users->hmax; i++) {
        linked_list_t *bucket = ht_users->buckets[i];
        ll_node_t *node = bucket->head;

        // se creaza vectorul de useri
        while (node) {
            user_t *u = ((info *)(node->data))->value;
            score[nr++] = u;

            node = node->next;
        }
    }

    // sortez vectorul
    for (int i = 0; i < nr; i++) {
        for (int j = i + 1; j < nr; j++) {
            // in cazul in care conditiile cerute nu sunt indeplinite
            // se va realiza interschimbarea userilor in vector
            if (condition_score(score[i], score[j])) {
                    swap_user(&score[i], &score[j]);
            }
        }
    }

    // se afiseaza topul userilor
    printf("Users ranking:\n");
    int index = 1;
    for (int i = 0; i < nr; i++) {
        if (score[i]->is_banned) {
            index--;
        } else {
            printf("%d. Name:%s Points:%d\n", index, score[i]->user_name,
                   score[i]->score);
        }
        index++;
    }
}

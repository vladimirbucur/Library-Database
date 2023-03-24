// Copyright 2022 Bucur Vladimir 311CA<vladimirbucur1@gmail.com>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"
#include "list.h"
#include "book.h"
#include "user.h"
#include "die.h"

#define MAX_STRING_SIZE	256
#define HMAX 1
#define COMMAND_MAX 100

// functie ce adauga un user in ht ul de useri
// si in caz ca e nevoie ii face resize
void
add_user(hashtable_t *ht_users, user_t *user, char *user_name)
{
    ht_put(ht_users, user_name, strlen(user_name) + 1, user, sizeof(user_t));
    if (ht_users->size / ht_users->hmax > 1)
        ht_resize(&ht_users, 3);
}

// functie ce realizeaza comanda de adaugarea unui nou user
void
command_add_user(hashtable_t* ht_users, char command[])
{
    // se separa comanda si se retine numele userului
    char aux[COMMAND_MAX], *word;
    char user_name[COMMAND_MAX];
	strcpy(aux, command);
	word = strtok(aux, " ");
    word = strtok(0, " ");
    strcpy(user_name, word);

    if (ht_has_key(ht_users, user_name)) {
        printf("User is already registered.\n");
        return;
    }

    // se aloca memorie pentru un nou user
    user_t *user = malloc(sizeof(user_t));
    DIE(!user, "Malloc failed!");

    // se retine numele userului si se fac initializarile asociate acestuia
    strcpy(user->user_name, user_name);
    strcpy(user->book_name, "");
    user->score = 100;
    user->is_banned = 0;
    user->no_days = 0;

    // se adauga userul in ht ul de useri
    add_user(ht_users, user, user_name);

    free(user);
}

// functie ce realizeaza comanda de imprumutare a unei carti
void
command_borrow(hashtable_t *ht_library, hashtable_t *ht_users, char command[])
{
    // se separa comanda si se retine userul, numele cartii
    // si nr de zile pana la retur
    char aux[COMMAND_MAX], *word;
    char user_name[COMMAND_MAX];
    char book_name[COMMAND_MAX];

	strcpy(aux, command);
	word = strtok(aux, " ");
    word = strtok(0, " \"");
    strcpy(user_name, word);
    word = strtok(0, "\"");
    strcpy(book_name, word);
    word = strtok(0, " \n");
    int days_available = atoi(word);

    // aflu userul cu numele precizat in comanda
    user_t *user = ht_get(ht_users, user_name);
    if (!ht_has_key(ht_users, user_name)) {
        printf("You are not registered yet.\n");
        return;
    }

    if (strcmp(user->book_name, "")) {
        printf("You have already borrowed a book.\n");
        return;
    }

    if (user->is_banned) {
        printf("You are banned from this library.\n");
        return;
    }

    // aflu cartea cu numele precizat in comanda
    book_t *book = get_book(ht_library, book_name);
    if (!book) {
        return;
    }

    if (book->is_borrowed) {
        printf("The book is borrowed.\n");
    } else {
        book->is_borrowed = 1;
        strcpy(user->book_name, book_name);
        user->no_days = days_available;
    }
}

// functie ce realizeaza comanda de returnare a unei carti
void
command_return(hashtable_t *ht_library, hashtable_t *ht_users, char command[])
{
    // se separa comanda si se retine userul, numele cartii, nr de zile de cand
    // a fost inchiriata cartea si ratingul acesteia dat de user
    char aux[COMMAND_MAX], *word;
    char user_name[COMMAND_MAX];
    char book_name[COMMAND_MAX];

	strcpy(aux, command);
	word = strtok(aux, " ");
    word = strtok(0, " \"");
    strcpy(user_name, word);
    word = strtok(0, "\"");
    strcpy(book_name, word);
    word = strtok(0, " ");
    int days_since_borrow = atoi(word);
    word = strtok(0, " \n");
    float rating = atof(word);

    // aflu userul si cartea cu numele precizate in comanda
    book_t *book = get_book(ht_library, book_name);
    user_t *user = ht_get(ht_users, user_name);

    if (!ht_has_key(ht_users, user_name)) {
        printf("You are not registered yet.\n");
    } else if (user->is_banned) {
        printf("You are banned from this library.\n");
    } else if (strcmp(book_name, user->book_name)) {
        printf("You didn't borrow this book.\n");
    } else if (!strcmp(user->book_name, "")) {
        printf("You didn't borrow this book.\n");
    } else {
        // se fac modificarile necesare si se calculeaza ratingul cartii
        book->is_borrowed = 0;
        book->purchases++;
        book->sum += rating;
        book->rating = (float)book->sum / (float)book->purchases;

        // in functie de zilele dupa care a dat retur la carte,
        // se calculeaza scorul userului
        if (user->no_days < days_since_borrow) {
            user->score -= 2 * (days_since_borrow - user->no_days);
        } else if (user->no_days > days_since_borrow) {
            user->score += user->no_days - days_since_borrow;
        }

        strcpy(user->book_name, "");
        user->no_days = 0;

        // daca scorul ajunge negativ, userul va fi banat
        if (user->score < 0) {
            user->is_banned = 1;
            printf("The user %s has been banned from this library.\n",
                   user_name);
        }
    }
}

// functie ce realizeaza comanda de pierdere a unei carti
void
command_lost(hashtable_t *ht_library, hashtable_t *ht_users, char command[])
{
    // se separa comanda si se retine userul si numele cartii
    char aux[COMMAND_MAX], *word;
    char user_name[COMMAND_MAX];
    char book_name[COMMAND_MAX];

	strcpy(aux, command);
	word = strtok(aux, " ");
    word = strtok(0, " \"");
    strcpy(user_name, word);
    word = strtok(0, "\"");
    strcpy(book_name, word);

    // aflu userul cu numele precizat in comanda
    user_t *user = ht_get(ht_users, user_name);

    if (!ht_has_key(ht_users, user_name)) {
        printf("You are not registered yet.\n");
    } else if (user->is_banned) {
        printf("You are banned from this library.\n");
    } else {
        // sterg cartea din biblioteca si fac modificarile necesare la datele
        // despre userul actual
        rmv_book(ht_library, book_name);
        user->score -= 50;
        strcpy(user->book_name, "");
        user->no_days = 0;

        // daca scorul ajunge negativ, userul va fi banat
        if (user->score < 0) {
            user->is_banned = 1;
            printf("The user %s has been banned from this library.\n",
                   user_name);
        }
    }
}

// functie ce realizeaza eliberarea de memorie a userilor
void
free_users(hashtable_t **users)
{
    for (unsigned int i = 0; i < (*users)->hmax; i++) {
        linked_list_t *bucket = (*users)->buckets[i];
        ll_node_t *node = bucket->head, *prev;
        while (node) {
            prev = node;
            char* key = ((info *)(node->data))->key;
            free(key);
            user_t *u = ((info *)(node->data))->value;
            free(u);

            node = node->next;
            free(prev->data);
            free(prev);
        }
        free(bucket);
    }
    free((*users)->buckets);
    free(*users);
}

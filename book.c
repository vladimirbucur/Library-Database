// Copyright 2022 Bucur Vladimir 311CA<vladimirbucur1@gmail.com>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "hashtable.h"
#include "book.h"
#include "die.h"

#define MAX_STRING_SIZE	256
#define HMAX 1
#define COMMAND_MAX 100

// functie ce adauga o carte in ht ul bibliotecii
// si in caz ca e nevoie ii face resize
void
add_book(hashtable_t* ht_library, book_t *book, char* book_name) {
    ht_put(ht_library, book_name, strlen(book_name) + 1, book, sizeof(book_t));
    if (ht_library->size / ht_library->hmax > 1)
        ht_resize(&ht_library, 1);
}

// functie ce citeste definitita(cheua si valoarea),
// ii aloca memorie si o retine
void
scan_def(info* def, char command[]) {
    char aux[COMMAND_MAX], *word;
    // se citeste de la tastatura perechea cheie valoare pentru
    // fiecare definitie
	fgets(command, COMMAND_MAX, stdin);
	strcpy(command + strlen(command) - 1, command + strlen(command));

    // se aloca memorie si se memoreaza definitia(cheia si valoarea)
	strcpy(aux, command);
	word = strtok(aux, " ");
    def->key = malloc((strlen(word) + 1) * sizeof(char));
    DIE(!def->key, "Malloc failed!");
    memcpy(def->key, word, (strlen(word) + 1) * sizeof(char));

    word = strtok(0, " ");
    def->value = malloc((strlen(word) + 1) * sizeof(char));
    DIE(!def->value, "Malloc failed!");
    memcpy(def->value, word, (strlen(word) + 1) * sizeof(char));
}

// functie ce realizeaza comanda de adaugare de carte
void
command_add_book(hashtable_t* ht_library, char command[])
{
    // se separa comanda si se retine numele cartii si numarul de definitii
    char aux[100], *word;
    char book_name[100];
    int def_no;
	strcpy(aux, command);

	word = strtok(aux, "\"");
    word = strtok(0, "\"");
    strcpy(book_name, word);

    word = strtok(0, " \n");
    def_no = atoi(word);

    // daca mai exista o carte cu acelasi nume se sterge
    // pentru a putea fi actualizata
    if (ht_has_key(ht_library, book_name)) {
        rmv_book(ht_library, book_name);
    }

    // se aloca memorie pentru o noua carte
    book_t *book = malloc(sizeof(book_t));
    DIE(!book, "Malloc failed!");

    book->ht_book = ht_create(HMAX, hash_function_string,
	    compare_function_strings);

    // se aloca memorie pentru o noua carte definitie
    info *def = malloc(sizeof(info));
    DIE(!def, "Malloc failed!");

    for (int i = 0; i < def_no; i++) {
        scan_def(def, command);

        // daca mai exista o definitie cu aceeasi cheie se sterge
        // pentru a putea fi actualizata
        if (ht_has_key(book->ht_book, def->key)) {
            ht_remove_entry(book->ht_book, def->key, 2);
        }
        // se adauga definitia in carte
        add_def(book->ht_book, def, strlen((char*)def->key) + 1,
                strlen((char*)def->value) + 1);

        free(def->key);
        free(def->value);
    }

    free(def);

    // se initializeaza datele despre carte
    book->name = malloc(strlen(book_name) + 1);
    DIE(!book->name, "Malloc failed!");
    strcpy(book->name, book_name);
    book->rating = 0;
    book->purchases = 0;
    book->sum = 0;
    book->is_borrowed = 0;

    // se adauga cartea in biblioteca
    add_book(ht_library, book, book_name);

    free(book);
}

book_t *
get_book(hashtable_t *library, char book_name[]) {
    if (!ht_has_key(library, book_name)) {
        printf("The book is not in the library.\n");
        return NULL;
    }

    // ne folosim de ht_get pentru a afla cartea cu numele
    // book_name din biblioteca
    book_t *book = ht_get(library, book_name);

    return book;
}

// functie ce realizeaza comanda de afisare a informatiilor unei carti
void
command_get_book(hashtable_t *library, char command[]) {
    // se separa comanda si se retine numele cartii
	char aux[COMMAND_MAX], *word;
    char book_name[COMMAND_MAX];
	strcpy(aux, command);
	word = strtok(aux, "\"");
    word = strtok(0, "\"");
    strcpy(book_name, word);

    // aflu cartea cu numele precizat in comanda
    book_t *book = get_book(library, book_name);
    if (book) {
        // pentru a se evita impartirea la 0, am initializat ratingul cu 0
        // in cazul in care numarul de imprumuturi ale cartii este 0
        if (!book->purchases) {
            book->rating = 0;
        } else {
            // se calculeaza ratingul cartii
            book->rating = (float)book->sum / (float)book->purchases;
        }

        // afisez informatiile cerute despre carte
        printf("Name:%s Rating:%.3f Purchases:%d\n", book->name, book->rating,
               book->purchases);
    }
}

// functie ce realizeaza stergerea unei carti din biblioteca
void
rmv_book(hashtable_t *ht_library, char *book_name)
{
    if (!ht_has_key(ht_library, book_name)) {
        printf("The book is not in the library.\n");
        return;
    }

    book_t *book =  get_book(ht_library, book_name);

    ht_remove_entry(ht_library, book->name, 1);
}

// functie ce realizeaza comanda de stergerea unei carti din biblioteca
void
command_rmv_book(hashtable_t *ht_library, char command[])
{
    // se separa comanda si se retine numele cartii
    char aux[COMMAND_MAX], *word;
    char book_name[COMMAND_MAX];
	strcpy(aux, command);
	word = strtok(aux, "\"");
    word = strtok(0, "\"");

    strcpy(book_name, word);

    // sterg cartea din biblioteca
    rmv_book(ht_library, book_name);
}

// functie ce adauga o definitie in ht ul de carti
// si in caz ca e nevoie ii face resize
void
add_def(hashtable_t* ht_book, info *def, unsigned int k_s, unsigned int v_s) {
    ht_put(ht_book, def->key, k_s,  def->value, v_s);
    if (ht_book->size / ht_book->hmax > 1)
        ht_resize(&ht_book, 2);
}

// functie ce realizeaza comanda de adaugare a unei definitii intr-o carte
void
command_add_def(hashtable_t* ht_library, char command[])
{
    // se separa comanda si se retine numele cartii, cheia si
    // valoarea definititiei ce urmeaza sa fie adaugate
    char aux[COMMAND_MAX], *word;
    char book_name[COMMAND_MAX];
    char def_key[COMMAND_MAX];
    char def_val[COMMAND_MAX];
	strcpy(aux, command);
	word = strtok(aux, "\"");
    word = strtok(0, "\"");
    strcpy(book_name, word);
    word = strtok(0, " ");
    strcpy(def_key, word);
    word = strtok(0, " \n");
    strcpy(def_val, word);

    if (!ht_has_key(ht_library, book_name)) {
        printf("The book is not in the library.\n");
    } else {
        // aflu cartea cu numele precizat in comanda
        book_t *book =  get_book(ht_library, book_name);

        // daca mai exista o definitie cu aceeasi cheie se sterge
        // pentru a putea fi actualizata
        if (ht_has_key(book->ht_book, def_key)) {
            rmv_def(ht_library, book_name, def_key);
        }

        // aloc memorie mentru o noua carte (+ cheie si valoare)
        info* def = malloc(sizeof(info));
        DIE(!def, "Malloc failed!");

        def->key = malloc((strlen(def_key) + 1) * sizeof(char));
        DIE(!def->key, "Malloc failed!");
        memcpy(def->key, def_key, (strlen(def_key) + 1) * sizeof(char));

        def->value = malloc((strlen(def_val) + 1) * sizeof(char));
        DIE(!def->value, "Malloc failed!");
        memcpy(def->value, def_val, (strlen(def_val) + 1) * sizeof(char));

        // adaug definitia in carte
        add_def(book->ht_book, def, strlen((char*)def->key) + 1,
                strlen((char*)def->value) + 1);

        free(def->key);
        free(def->value);
        free(def);
    }
}

// functie ce returneaza valoarea unei definitii
char *
get_def(hashtable_t *library, char book_name[], char def_key[]) {
    if (!ht_has_key(library, book_name)) {
        printf("The book is not in the library.\n");
        return NULL;
    }
    // aflu cartea cu numele precizat in comanda
    book_t *book =  get_book(library, book_name);
    if (!ht_has_key(book->ht_book, def_key)) {
        printf("The definition is not in the book.\n");
        return NULL;
    }

    // aflu si returnez definitia din cartea din comanda
    char *def_value = ht_get(book->ht_book, def_key);
    return def_value;
}

// functie ce realizeaza comanda de afisare a unei definitii careia i se da
// definitia si cartea unde se gaseste
void
command_get_def(hashtable_t *library, char command[])
{
    // se separa comanda si se retine numele cartii si cheia definitiei
    char aux[COMMAND_MAX], *word;
    char book_name[COMMAND_MAX];
    char def_name[COMMAND_MAX];
	strcpy(aux, command);
	word = strtok(aux, "\"");
    word = strtok(0, "\"");
    strcpy(book_name, word);
    word = strtok(0, " \n");
    strcpy(def_name, word);

    // aflu definitia din cartea din comanda
    char *def = get_def(library, book_name, def_name);
    if (def)
        printf("%s\n", def);
}

// functie ce realizeaza stergerea unei definitii dintr-o carte
void
rmv_def(hashtable_t *ht_library, char book_name[], char def_key[])
{
    if (!ht_has_key(ht_library, book_name)) {
        printf("The book is not in the library.\n");
        return;
    }

    // aflu cartea cu numele precizat in comanda
    book_t *book =  get_book(ht_library, book_name);
    if (!ht_has_key(book->ht_book, def_key)) {
        printf("The definition is not in the book.\n");
        return;
    }

    // sterg definitia din carte
    ht_remove_entry(book->ht_book, def_key, 2);
}

// functie ce realizeaza comanda de stergerea unei definitii dintr-o carte
void
command_rmv_def(hashtable_t *ht_library, char command[])
{
    // se separa comanda si se retine numele cartii sicheia definititei
    // ce urmeaza sa fie stearsa
    char aux[COMMAND_MAX], *word;
    char book_name[COMMAND_MAX];
    char def_key[COMMAND_MAX];
	strcpy(aux, command);
	word = strtok(aux, "\"");
    word = strtok(0, "\"");
    strcpy(book_name, word);
    word = strtok(0, " \n");
    strcpy(def_key, word);

    // sterg definitia din carte
    rmv_def(ht_library, book_name, def_key);
}

// functie ce realizeaza eliberarea de memorie a unei carti
void
free_book(book_t **book)
{
    free((*book)->name);
    ht_free((*book)->ht_book);
    free(*book);
}

// functie ce realizeaza eliberarea de memorie a inregii biblioteci
void
free_library(hashtable_t **library)
{
    for (unsigned int i = 0; i < (*library)->hmax; i++) {
        linked_list_t *bucket = (*library)->buckets[i];
        ll_node_t *node = bucket->head, *prev;
        while (node) {
            prev = node;
            char* key = ((info *)(node->data))->key;
            free(key);

            book_t *book = ((info *)(node->data))->value;
            free_book(&book);
            node = node->next;

            free(prev->data);
            free(prev);
        }
        free(bucket);
    }
    free((*library)->buckets);
    free(*library);
}

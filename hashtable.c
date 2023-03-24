// Copyright 2022 Bucur Vladimir 311CA<vladimirbucur1@gmail.com>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "book.h"
#include "hashtable.h"
#include "list.h"
#include "user.h"
#include "die.h"

#define MAX_STRING_SIZE	256
#define HMAX 10

/*
 * Functii de comparare a cheilor:
 */
int
compare_function_ints(void *a, void *b)
{
	int int_a = *((int *)a);
	int int_b = *((int *)b);

	if (int_a == int_b) {
		return 0;
	} else if (int_a < int_b) {
		return -1;
	} else {
		return 1;
	}
}

int
compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

/*
 * Functii de hashing:
 */
unsigned int
hash_function_int(void *a)
{
	/*
	 * Credits: https://stackoverflow.com/a/12996028/7883884
	 */
	unsigned int uint_a = *((unsigned int *)a);

	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = (uint_a >> 16u) ^ uint_a;
	return uint_a;
}

unsigned int
hash_function_string(void *a)
{
	/*
	 * Credits: http://www.cse.yorku.ca/~oz/hash.html
	 */
	unsigned char *puchar_a = (unsigned char*) a;
	int64_t hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

	return hash;
}

/*
 * Functie apelata dupa alocarea unui hashtable pentru a-l initializa.
 * Trebuie alocate si initializate si listele inlantuite.
 */
hashtable_t *ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*))
{
	hashtable_t *ht = malloc(sizeof(hashtable_t));
	DIE(!ht, "Malloc failed!");
    ht->buckets = malloc(hmax * sizeof(*ht->buckets));
    DIE(!ht->buckets, "Malloc failed!");

	for (unsigned int i = 0; i < hmax; i++)
        ht->buckets[i] = ll_create(sizeof(info));
    ht->size = 0;
    ht->hmax = hmax;
    ht->hash_function = hash_function;
    ht->compare_function = compare_function;

	return ht;
}

void *
ht_get(hashtable_t *ht, void *key)
{
	unsigned int i = ht->hash_function(key) % ht->hmax;

        ll_node_t *aux = ht->buckets[i]->head;
        while (aux != NULL) {
            if (!ht->compare_function(key, ((info*)(aux->data))->key))
                return ((info*)(aux->data))->value;

            aux = aux->next;
        }

	return NULL;
}

/*
 * Functie care intoarce:
 * 1, daca pentru cheia key a fost asociata anterior o valoare in hashtable folosind functia put
 * 0, altfel.
 */
int
ht_has_key(hashtable_t *ht, void *key)
{
	return ht_get(ht, key) != NULL;
}

/*
 * Atentie! Desi cheia este trimisa ca un void pointer (deoarece nu se impune tipul ei), in momentul in care
 * se creeaza o noua intrare in hashtable (in cazul in care cheia nu se gaseste deja in ht), trebuie creata o copie
 * a valorii la care pointeaza key si adresa acestei copii trebuie salvata in structura info asociata intrarii din ht.
 * Pentru a sti cati octeti trebuie alocati si copiati, folositi parametrul key_size.
 *
 * Motivatie:
 * Este nevoie sa copiem valoarea la care pointeaza key deoarece dupa un apel put(ht, key_actual, value_actual),
 * valoarea la care pointeaza key_actual poate fi alterata (de ex: *key_actual++). Daca am folosi direct adresa
 * pointerului key_actual, practic s-ar modifica din afara hashtable-ului cheia unei intrari din hashtable.
 * Nu ne dorim acest lucru, fiindca exista riscul sa ajungem in situatia in care nu mai stim la ce cheie este
 * inregistrata o anumita valoare.
 */
void
ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	unsigned int i = ht->hash_function(key) % ht->hmax;

	info data;
	data.key = malloc(key_size);
	DIE(!data.key, "Malloc failed!");
	memcpy(data.key, key, key_size);

	data.value = malloc(value_size);
	DIE(!data.value, "Malloc failed!");
	memcpy(data.value, value, value_size);

	ll_add_nth_node(ht->buckets[i], ht->buckets[i]->size, &data);
	ht->size++;
}

/*
 * Procedura care elimina din hashtable intrarea asociata cheii key.
 * Atentie! Trebuie avuta grija la eliberarea intregii memorii folosite pentru o intrare din hashtable (adica memoria
 * pentru copia lui key --vezi observatia de la procedura put--, pentru structura info si pentru structura Node din
 * lista inlantuita).
 */
void
ht_remove_entry(hashtable_t *ht, void *key, int type)
{
	unsigned int i = ht->hash_function(key) % ht->hmax;
	ll_node_t *aux = ht->buckets[i]->head;
	int pos = 0;
    while (aux) {
		if (type == 1) {
			if (ht->compare_function(key, ((info*)(aux->data))->key) == 0) {
			ll_node_t *removed = ll_remove_nth_node(ht->buckets[i], pos);
			free_book((book_t**)(&(*(info *)removed->data).value));
			free(((info *)removed->data)->key);
			free(removed->data);
			free(removed);
			ht->size--;
			return;
			}
		} else if (type == 2){
			if (ht->compare_function(key, ((info*)(aux->data))->key) == 0) {
			ll_node_t *removed = ll_remove_nth_node(ht->buckets[i], pos);
			free(((info *)removed->data)->value);
			free(((info *)removed->data)->key);
			free(removed->data);
			free(removed);
			ht->size--;
			return;
			}
		}

        aux = aux->next;
		pos++;
    }
}

/*
 * Procedura care elibereaza memoria folosita de toate intrarile din hashtable, dupa care elibereaza si memoria folosita
 * pentru a stoca structura hashtable.
 */
void
ht_free(hashtable_t *ht)
{
    ll_node_t *aux;
	for (unsigned int i = 0; i < ht->hmax; i++) {
		if (ht->buckets[i]->head) {
			aux = ht->buckets[i]->head;
			while (aux) {
				free(((info *)aux->data)->value);
				free(((info *)aux->data)->key);
				aux = aux->next;
			}
		}
		ll_free(&ht->buckets[i]);
	}

	free(ht->buckets);
	free(ht);
}

unsigned int
ht_get_size(hashtable_t *ht)
{
	if (!ht)
		return 0;

	return ht->size;
}

unsigned int
ht_get_hmax(hashtable_t *ht)
{
	if (!ht)
		return 0;

	return ht->hmax;
}

void
ht_resize(hashtable_t **ht, int type)
{
	hashtable_t *new_ht = ht_create((*ht)->hmax * 2, (*ht)->hash_function,
	                                (*ht)->compare_function);

	for (unsigned int i = 0; i < (*ht)->hmax; i++) {
		ll_node_t *curr = (*ht)->buckets[i]->head;

		while (curr) {
			char *key = (char *)((struct info *)(curr->data))->key;

			if (type == 1) {
				book_t *stct = (book_t*)((struct info*)(curr->data))->value;
				ht_put(new_ht, key, strlen(key) + 1, stct, sizeof(book_t));
				curr = curr->next;
			} else if (type == 2) {
				char *stct = (char*)((struct info*)(curr->data))->value;
				ht_put(new_ht, key, strlen(key) + 1, stct,
				       (strlen(stct) + 1) *sizeof(char));
				curr = curr->next;
			} else if (type == 3) {
				user_t *stct = (user_t*)((struct info*)(curr->data))->value;
				ht_put(new_ht, key, strlen(key) + 1, stct, sizeof(user_t));
				curr = curr->next;
			}
		}
	}

	for (unsigned int i = 0; i < (*ht)->hmax; i++) {
		ll_node_t *curr = (*ht)->buckets[i]->head;

		while (curr) {
			free(((struct info *)(curr->data))->key);
			free(((struct info *)(curr->data))->value);
			curr = curr->next;
		}

		ll_free(&(*ht)->buckets[i]); /* Adresa unei liste inlantuite */
	}

	free((*ht)->buckets);
	(*ht)->buckets = new_ht->buckets;
	(*ht)->hmax = new_ht->hmax;
	free(new_ht);
}

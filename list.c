// Copyright 2022 Bucur Vladimir 311CA<vladimirbucur1@gmail.com>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "die.h"

#define MAX_STRING_SIZE	256
#define HMAX 1

linked_list_t* ll_create(unsigned int data_size)
{
    linked_list_t *list = malloc(sizeof(linked_list_t));
    DIE(!list, "Malloc failed!");
    list->head = NULL;
    list->data_size = data_size;
    list->size = 0;
    return list;
}

ll_node_t* create_new_node(const void* new_data, int data_size)
{
    ll_node_t *new_node =  malloc(sizeof(ll_node_t));
    DIE(!new_data, "Malloc failed!");
    new_node->data =  malloc(data_size);
    DIE(!new_node->data , "Malloc failed!");
    memcpy(new_node->data, new_data, data_size);
    new_node->next = NULL;
    return new_node;
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se creeaza un nou nod care e
 * adaugat pe pozitia n a listei reprezentata de pointerul list. Pozitiile din
 * lista sunt indexate incepand cu 0 (i.e. primul nod din lista se afla pe
 * pozitia n=0). Daca n >= nr_noduri, noul nod se adauga la finalul listei).
 */
void ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
    ll_node_t *new_node = create_new_node(new_data, list->data_size);
    ll_node_t *aux = list->head;
    if (!list->size) {
        new_node->next = NULL;
        list->head = new_node;
        list->size++;
    } else if (!n) {
        new_node->next = list->head;
        list->head = new_node;
        list->size++;
    } else if (list->size <= n) {
        for (unsigned int i = 0; i < list->size - 1; i++)
            aux = aux->next;

        new_node->next = NULL;
        aux->next = new_node;
        list->size++;
    } else {
        for (unsigned int i = 0; i < n - 1; i++)
            aux = aux->next;

        new_node->next = aux->next;
        aux->next = new_node;
        list->size++;
    }
}

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Daca n >= nr_noduri - 1, se elimina nodul de
 * la finalul listei. Functia intoarce un pointer spre acest nod proaspat
 * eliminat din lista. Este responsabilitatea apelantului sa elibereze memoria
 * acestui nod.
 */
ll_node_t* ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
    ll_node_t *aux = list->head;
    if (n == 0) {
        ll_node_t *garbage = list->head;
        aux = aux->next;
        garbage->next = NULL;
        list->head = aux;
        list->size--;
        return garbage;
    } else if (list->size <= n) {
        for (unsigned int i = 0; i < list->size - 2; i++)
            aux = aux->next;

        ll_node_t *garbage = aux->next;
        aux->next = NULL;
        garbage->next = NULL;
        list->size--;
        return garbage;
    } else {
        for (unsigned int i = 0; i < n - 1; i++)
            aux = aux->next;

        ll_node_t *garbage = aux->next;
        aux->next = aux->next->next;
        garbage->next = NULL;
        list->size--;
        return garbage;
    }
}

/*
 * Functia intoarce numarul de noduri din lista al carei pointer este trimis ca
 * parametru.
 */
unsigned int ll_get_size(linked_list_t* list)
{
    return list->size;
}

/*
 * Procedura elibereaza memoria folosita de toate nodurile din lista, iar la
 * sfarsit, elibereaza memoria folosita de structura lista si actualizeaza la
 * NULL valoarea pointerului la care pointeaza argumentul (argumentul este un
 * pointer la un pointer).
 */
void ll_free(linked_list_t** pp_list)
{
    ll_node_t *aux = (*pp_list)->head;
    while (aux) {
        ll_node_t *temp = aux;
        aux = aux->next;
        free(temp->data);
        free(temp);
        (*pp_list)->size--;
    }
    free(*pp_list);
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza int-uri. Functia afiseaza toate valorile int stocate in nodurile
 * din lista inlantuita separate printr-un spatiu.
 */
void ll_print_int(linked_list_t* list)
{
    ll_node_t *aux = list->head;
    while (aux) {
        printf("%d ", *((int*)aux->data));
        aux = aux->next;
    }

    printf("\n");
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza string-uri. Functia afiseaza toate string-urile stocate in
 * nodurile din lista inlantuita, separate printr-un spatiu.
 */
void ll_print_string(linked_list_t* list)
{
    ll_node_t *aux = list->head;
    while (aux) {
        printf("%s ", (char*)aux->data);
        aux = aux->next;
    }

    printf("\n");
}

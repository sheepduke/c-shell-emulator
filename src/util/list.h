#ifndef LIST_H
#define LIST_H

#include <stdio.h>

typedef struct List List;

List *list_new();

void list_destroy(List *list);

size_t list_size(const List *list);

void *list_front(const List *list);

void *list_back(const List *list);

void list_foreach(const List *list, void (*func)(void *data));

List *list_push_back(List *list, void *element);

List *list_push_front(List *list, void *element);

void list_pop_front(List *list);

void list_pop_back(List *list);

void list_clear(List *list);

#endif

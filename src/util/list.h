#ifndef LIST_H
#define LIST_H

#include <stdio.h>

typedef struct list list;

list *list_new();

void list_destroy(list **list);

size_t list_size(list *list);

void *list_front(list *list);

void *list_back(list *list);

void list_foreach(list *list, void (*func)(void *data));

list *list_push_back(list *list, void *element);

list *list_push_front(list *list, void *element);

void list_pop_front(list *list);

void list_pop_back(list *list);

void list_clear(list *list);

#endif

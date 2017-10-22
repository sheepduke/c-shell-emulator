#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// ----------------------------------------------------------------------
// node definition
// ----------------------------------------------------------------------

typedef struct node node;

struct node {
  void *data;
  node *prev;
  node *next;
};

static node *node_new(void *data) {
  node *node = malloc(sizeof(node));
  assert(node);

  node->data = data;
  node->prev = NULL;
  node->next = NULL;

  return node;
}

static void node_destroy(node *node) {
  free(node);
}

struct list {
  size_t size;
  node *head;
  node *tail;
};

// ----------------------------------------------------------------------
// internal function
// ----------------------------------------------------------------------

void link_node(node *left, node *right) {
  if (left) {
	left->next = right;
  }
  if (right) {
	right->prev = left;
  }
}

// ----------------------------------------------------------------------
// API
// ----------------------------------------------------------------------

list *list_new() {
  list *list = malloc(sizeof(list));
  assert(list);
  
  list->size = 0;
  list->head = list->tail = NULL;
  return list;
}

void list_destroy(list **list) {
  for (node *node = (*list)->head; node; node = node->next) {
	node_destroy(node);
  }
  free(*list);
  list = NULL;
}

size_t list_size(list *list) {
  assert(list);
  return list->size;
}

void *list_front(list *list) {
  assert(list);
  return (list->head ? list->head->data : NULL);
}

void *list_back(list *list) {
  assert(list);
  return (list->tail ? list->tail->data : NULL);
}

void list_foreach(list *list, void (*func)(void *data)) {
  assert(list);
  for (node *node = list->head; node; node = node->next) {
	func(node->data);
  }
}

list *list_push_back(list *list, void *data) {
  assert(list && data);
  node *new_node = node_new(data);
  if (list->tail) {
	link_node(list->tail, new_node);
	list->tail = list->tail->next;
  }
  else {
	list->tail = list->head = new_node;
  }
  list->size++;
  return list;
}

list *list_push_front(list *list, void *data) {
  assert(list && data);
  
  node *new_node = node_new(data);
  
  if (list->head) {
	link_node(new_node, list->head);
	list->head = new_node;
  }
  else {
	list->head = list->tail = new_node;
  }
  list->size++;
  return list;
}

void list_pop_front(list *list) {
  assert(list);
  if (list->head) {
	node *old_head = list->head;
	list->head = list->head->next;
	node_destroy(old_head);
  }
  list->size--;
}

void list_pop_back(list *list) {
  assert(list);
  if (list->tail) {
	node *old_tail = list->tail;
	list->tail = list->tail->prev;
	node_destroy(old_tail);
  }
  list->size--;
}

void list_clear(list *list) {
  assert(list);
  list_foreach(list, free);
  list->head = list->tail = NULL;
  list->size = 0;
}

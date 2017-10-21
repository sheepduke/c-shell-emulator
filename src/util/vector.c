#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define VECTOR_DEFAULT_SIZE 100

// ----------------------------------------------------------------------
// struct
// ----------------------------------------------------------------------

struct vector {
  void **data;
  size_t size;
  size_t capacity;
};


// ----------------------------------------------------------------------
// API
// ----------------------------------------------------------------------

vector *vector_new(size_t element_size) {
  vector *v = malloc(sizeof(vector));

  if (v) {
	v->capacity = VECTOR_DEFAULT_SIZE;
	v->size = 0;
	v->data = malloc(sizeof(void *) * v->capacity);
  }
	
  return v;
}

void vector_destroy(vector *vector) {
  free(vector);
}

size_t vector_size(vector *vector) {
  assert(vector);
  return vector->size;
}

void *vector_at(vector *v, size_t index) {
  assert(index < v->size);

  return v->data[index];
}

void vector_push(vector *v, void *element) {
  assert(v);
  if (v->size == v->capacity) {
	v->capacity += VECTOR_DEFAULT_SIZE;
	v->data = realloc(v->data, sizeof(void *) * v->capacity);
  }
  v->data[v->size] = element;
  v->size++;
}

void vector_insert(vector *vector, size_t index, void *data) {
  assert(index < vector->size);
  
}

/* TODO */
void *find(void *value, int *compare(void *, void *));

/* TODO */
void vector_remove(void *value, int *compare(void *, void *));



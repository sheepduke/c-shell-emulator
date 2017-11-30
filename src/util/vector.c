#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define VECTOR_DEFAULT_SIZE 100

// ----------------------------------------------------------------------
// Internal
// ----------------------------------------------------------------------

struct Vector {
  void **data;
  size_t size;
  size_t capacity;
  void (*destructor)(void *);
};


// ----------------------------------------------------------------------
// API
// ----------------------------------------------------------------------

Vector *vector_new(void (*destructor)(void *)) {
  Vector *v = malloc(sizeof(Vector));

  assert(v);
  
  v->capacity = VECTOR_DEFAULT_SIZE;
  v->size = 0;
  v->data = malloc(sizeof(void *) * v->capacity);
  v->destructor = destructor;
	
  return v;
}

void vector_destroy(void *obj) {
  Vector *vector = obj;
  for (int i = 0; i < vector->size; i++) {
	vector->destructor(vector_at(vector, i));
  }
  free(vector);
}

size_t vector_size(const Vector *vector) {
  assert(vector);
  return vector->size;
}

void *vector_at(const Vector *v, size_t index) {
  assert(index < v->size);

  return v->data[index];
}

void vector_push(Vector *v, void *element) {
  assert(v);
  if (v->size == v->capacity) {
	v->capacity += VECTOR_DEFAULT_SIZE;
	v->data = realloc(v->data, sizeof(void *) * v->capacity);
  }
  v->data[v->size] = element;
  v->size++;
}

size_t vector_find(Vector *vector, void *value,
                   bool equal(const void *, const void *)) {
  for (int i = 0; i < vector->size; i++) {
    void *element = vector_at(vector, i);
    if (equal(element, value)) {
      return i;
    }
  }

  return -1;
}

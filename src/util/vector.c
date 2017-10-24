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

void vector_destroy(void *vector) {
  free(vector);
}

void vector_destroy_all(void *vec) {
  Vector *vector_ = vec;
  for (int i = 0; i < vector_->size; i++) {
	vector_->destructor(vector_at(vector_, i));
  }
  free(vector_);
}

size_t vector_size(Vector *vector) {
  assert(vector);
  return vector->size;
}

void *vector_at(Vector *v, size_t index) {
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


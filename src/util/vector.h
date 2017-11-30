#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <stdbool.h>

// Struct for vector itself.
// 
typedef struct Vector Vector;

// Create a vector and return its pointer.
// 
Vector *vector_new(void (*destructor)(void *));

// Destroy the vector and its elements.
// 
void vector_destroy(void *);

// Return size of vector.
// Size here means how many elements are there in the vector.
// 
size_t vector_size(const Vector *v);

// Return element at position `index'.
// If `index' is not valid, return NULL.
// 
void *vector_at(const Vector *v, size_t index);

// Push an element to the last position of vector.
// 
void vector_push(Vector *v, void *element);

// // Insert an element `data' into position indicated by `index'.
// // 
// void vector_insert(Vector *vector, size_t index, void *data);

// Return first index of element equals to given `value'.
// If nothing has been found, return -1;
// 
size_t vector_find(Vector *vector, void *value,
                   bool equal(const void *, const void *));

// // Remove element equals to `value'.
// // Elements are compared with `compare', it should return 1 if two elements
// // are equal, 0 otherwise.
// // 
// void vector_remove(void *value, int *compare(void *, void *));

#endif

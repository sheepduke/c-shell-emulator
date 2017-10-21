#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

// Struct for vector itself.
// 
typedef struct vector vector;

// Create a vector and return its pointer.
// 
vector *vector_new();

// Destroy a vector, without touching its elements.
// 
void vector_destroy(vector *);

// Return size of vector.
// Size here means how many elements are there in the vector.
// 
size_t vector_size(vector *v);

// Return element at position `index'.
// If `index' is not valid, return NULL.
// 
void *vector_at(vector *v, size_t index);

// Push an element to the last position of vector.
// 
void vector_push(vector *v, void *element);

// Insert an element `data' into position indicated by `index'.
// 
void vector_insert(vector *vector, size_t index, void *data);

// Return element equals to given `value'.
// 
void *find(void *value, int *compare(void *, void *));

// Remove element equals to `value'.
// Elements are compared with `compare', it should return 1 if two elements
// are equal, 0 otherwise.
// 
void vector_remove(void *value, int *compare(void *, void *));

#endif

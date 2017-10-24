#ifndef STR_H
#define STR_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct String String;

// Create empty string.
// 
String *string_new();

// Create string from given `data'.
// 
String *string_from(const char *data);

// Copy constructor
// 
void string_copy(String *str, String *source);

// Clear the content of string.
// 
void string_clear(String *string);

// Destructor.
// 
void string_destroy(void *string);

// Return true if string is empty.
// 
bool string_empty(String *str);

// Return true if `str1' and `str2' are equal.
// 
bool string_equal(String *str1, String *str2);

// Return the length of characters, excluding '\0'.
// 
size_t string_length(String *string);

// Return the raw data.
// 
char *string_raw(String *string);

// Return char at given index of string.
// 
char string_at(String *str, size_t index);

// Return the first index of `ch' found in string.
// 
size_t string_find(String *str, char ch);

// Set the content of string to given content.
// 
void string_set(String *str, const char *content);

// Append characters `data' into `string'.
// 
void string_append(String *string, const char *data);

// Concatenate the content of `string2' into `string1'.
// 
void string_concat(String *string1, String *source);

// For given `str', remove `length' of characters starting at `start'.
// 
void string_sub(String *str, size_t start, size_t end);

// Trim any space, newline etc from str.
// 
void string_trim(String *str);


#endif

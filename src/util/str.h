#include <stdlib.h>
#include <stdbool.h>

typedef struct string string;

// Create empty string.
// 
string *string_new();

// Create string from given `data'.
// 
string *string_from(const char *data);

// Copy constructor
// 
void string_copy(string *str, string *source);

// Clear the content of string.
// 
void string_clear(string *string);

// Destructor.
// 
void string_destroy(string **string);

// Return true if string is empty.
// 
bool string_empty(string *str);

// Return true if `str1' and `str2' are equal.
// 
bool string_equal(string *str1, string *str2);

// Return the length of characters, excluding '\0'.
// 
size_t string_length(string *string);

// Return the raw data.
// 
char *string_raw(string *string);

// Return char at given index of string.
// 
char string_at(string *str, size_t index);

// Return the first index of `ch' found in string.
// 
size_t string_find(string *str, char ch);

// Set the content of string to given content.
// 
void string_set(string *str, const char *content);

// Append characters `data' into `string'.
// 
void string_append(string *string, const char *data);

// Concatenate the content of `string2' into `string1'.
// 
void string_concat(string *string1, string *source);

// For given `str', remove `length' of characters starting at `start'.
// 
void string_sub(string *str, size_t start, size_t end);

// Trim any space, newline etc from str.
// 
void string_trim(string *str);

#include <stdlib.h>

typedef struct string string;

// Create empty string.
// 
string *string_new();

// Create string from given `data'.
// 
string *string_from(const char *data);

// Copy constructor
// 
string *string_copy(string *source);

// Clear the content of string.
// 
void string_clear(string *string);

// Destructor.
// 
void string_destroy(string *string);

// Return the length of characters, excluding '\0'.
// 
size_t string_length(string *string);

// Return the raw data.
// 
const char *string_raw(string *string);

// Append characters `data' into `string'.
// 
void string_append(string *string, const char *data);

// Concatenate the content of `string2' into `string1'.
// 
void string_concat(string *string1, string *source);

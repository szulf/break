#pragma once

#include <stddef.h>
#include <stdbool.h>

#define STRING_EMPTY (string_type){0, NULL}

typedef struct string_type
{
    size_t length;
    char* str;
} string_type;

int string_new(string_type* str, const char* s);
void string_free(string_type* str);
int string_append(string_type* str, const char* s);
int string_reassign(string_type* str, const char* s);
void string_clear(string_type* str);
bool string_empty(string_type* str);

#include "string_type.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int string_new(string_type* str, const char* s)
{
    size_t s_len = strlen(s);

    str->str = malloc(s_len + 1);
    if (str->str == NULL)
    {
        return 1;
    }

    str->length = s_len;
    strcpy(str->str, s);

    return 0;
}

void string_free(string_type* str)
{
    free(str->str);
    str->str = NULL;
}

int string_append(string_type* str, const char* s)
{
    size_t s_len = strlen(s);

    char* temp = malloc(s_len + str->length + 1);
    if (temp == NULL)
    {
        return 1;
    }

    strcpy(temp, str->str);
    strcpy(temp + str->length, s);

    free(str->str);
    str->str = temp;
    str->length += s_len;

    return 0;
}

int string_reassign(string_type* str, const char* s)
{
    free(str->str);

    size_t s_len = strlen(s);

    str->str = malloc(s_len + 1);
    if (str->str == NULL)
    {
        return 1;
    }

    str->length = s_len;
    strcpy(str->str, s);

    return 0;
}

void string_clear(string_type* str)
{
    free(str->str);
    str->length = 0;
    str->str = NULL;
}

bool string_empty(string_type* str)
{
    return str->str == NULL && str->length == 0;
}

// Usage example
// int main()
// {
//     string_type s;
//     string_new(&s, "foo");
//
//     printf("%s\n", s.str);
//
//     string_append(&s, "bar");
//     printf("%s\n", s.str);
//
//     string_reassign(&s, "test");
//     printf("%s\n", s.str);
//
//     string_clear(&s);
//     printf("%d\n", string_empty(&s));
//
//     string_free(&s);
//     return 0;
// }

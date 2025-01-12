#pragma once

#include <stddef.h>

typedef struct list_type
{
    size_t length;
    size_t max_count;
    size_t element_size;
    void* list;
} list_type;

int list_new(list_type* lt, size_t element_size, size_t max_count);
void list_free(list_type* lt);
int list_append(list_type* lt, void* val);
void* list_get(list_type* lt, size_t idx);

#include "list_type.h"

#include <stdlib.h>
#include <string.h>

int list_new(list_type* lt, size_t element_size, size_t max_count)
{
    lt->list = malloc(element_size * max_count);
    if (lt->list == NULL)
    {
        return 1;
    }

    lt->element_size = element_size;
    lt->max_count = max_count;
    lt->length = 0;

    return 0;
}

void list_free(list_type* lt)
{
    free(lt->list);

    lt->list = NULL;
    lt->length = 0;
    lt->max_count = 0;
    lt->element_size = 0;
}

int list_append(list_type* lt, void* val)
{
    if (lt->length >= lt->max_count)
    {
        void* temp = realloc(lt->list, lt->max_count * 2 * lt->element_size);
        if (temp == NULL)
        {
            return 1;
        }

        lt->list = temp;
        lt->max_count *= 2;
    }

    memcpy(lt->list + (lt->length * lt->element_size), val, lt->element_size);
    lt->length++;

    return 0;
}

void* list_get(list_type* lt, size_t idx)
{
    return lt->list + (idx * lt->element_size);
}

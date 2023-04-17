/******************************************************************************
*** Copyright *****************************************************************
** 
** Copyright 2022 Daniel Alvarez <shogundevel@gmail.com>
** 
** Permission is hereby granted, free of charge, to any person
** obtaining a copy of this software and associated documentation files
** (the "Software"), to deal in the Software without restriction,
** including without limitation the rights to use, copy, modify, merge,
** publish, distribute, sublicense, and/or sell copies of the Software,
** and to permit persons to whom the Software is furnished to do so,
** subject to the following conditions:
** 
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
** 
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
** 
******************************************************************************/

#include "shark.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void shark_memory_error()
{
    fprintf(stderr, "memory allocation failure.");
    exit(-1);
}

#ifndef SHARK_NO_MALLOC
void *shark_allocate(size_t object_size)
{
    void *object = calloc(object_size, 1);
    if (object == NULL) shark_memory_error();
    return object;
}

void *shark_realloc(void *object, size_t new_size)
{
    object = realloc(object, new_size);
    if (object == NULL) shark_memory_error();
    return object;
}

void shark_dealloc(void *object)
{
    free(object);
}
#endif

void *shark_object_new(void *type)
{
    shark_object *object = shark_allocate(((shark_object_class *) type)->object_size);
    object->type = type;
    return object;
}

shark_bool shark_object_instanceof(void *object, void *type)
{
    if (object == NULL) return false;
    for (shark_object_class *object_type = ((shark_object *) object)->type;
        object_type != NULL;
        object_type = object_type->parent)
        if (object_type == type) return true;
    return false;
}

shark_str shark_str_new(char *data)
{
    shark_str object = (shark_str) shark_allocate(sizeof(struct _shark_str));
    object->super.type = NULL;
    object->size = strlen(data);
    object->hash = 0;
    object->data = shark_allocate(object->size + 1);
    if (object->data == NULL) shark_memory_error();
    memcpy(object->data, data, object->size);
    object->data[object->size] = '\0';
    return object;
}

static size_t shark_hash_byte_str(size_t length, uint8_t *data)
{
    size_t hash = 5381;
    for (size_t i = 0; i < length; i++) {
        hash = ((hash << 5) + hash + *(data++)) & 4294967295;
    }
    return hash ? hash : 1;
}

size_t shark_str_hash(shark_str x)
{
    if (x == NULL) return 1;
    if (x->hash != 0) return x->hash;
    return x->hash = shark_hash_byte_str(x->size, (uint8_t *) x->data);
}

shark_bool shark_str_equals(shark_str x, shark_str y)
{
    if (x == y) return true;
    if (x == NULL || y == NULL) return false;
    if (x->size != y->size) return false;
    int cmp = memcmp(x->data, y->data, x->size);
    if (cmp < 0 || cmp > 0) return false;
    return true;
}

#define shark_array_type(value_t) \
    value_t ## _array *value_t ## _array_new(size_t size) { \
        value_t ## _array *object = shark_allocate(sizeof(value_t ## _array) + sizeof(value_t) * size); \
        object->super.type = NULL; \
        object->size = size; \
        return object; \
    }

shark_array_type(shark_bool)
shark_array_type(shark_int)
shark_array_type(shark_float)
shark_array_type(shark_char)
shark_array_type(shark_any)

#undef shark_array_type

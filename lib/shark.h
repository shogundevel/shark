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

#ifndef __SHARK
#define __SHARK

#include <stdbool.h>
#include <stdint.h>

typedef bool shark_bool;
typedef int64_t shark_int;
typedef double shark_float;
typedef uint32_t shark_char;
typedef struct _shark_str *shark_str;
typedef void *shark_any;

typedef struct _shark_object shark_object;
typedef struct _shark_object_class shark_object_class;

struct _shark_object
{
    shark_object_class *type;
};

struct _shark_object_class
{
    size_t object_size;
    shark_object_class *parent;
};

void shark_memory_error();
void *shark_allocate(size_t object_size);
void *shark_realloc(void *object, size_t new_size); // todo
void shark_dealloc(void *object);

void *shark_object_new(void *type);
shark_bool shark_object_instanceof(void *object, void *type);

shark_str shark_str_new(char *data);
size_t shark_str_hash(shark_str x);
shark_bool shark_str_equals(shark_str x, shark_str y);

struct _shark_str
{
    shark_object super;
    size_t size;
    size_t hash;
    char *data;
};

#define SHARK_STR_LIT(size, data) {{ NULL }, size, 0, data}

#define shark_array_type(type) \
    typedef struct { \
        shark_object super; \
        size_t size; \
        type data[]; \
    } type ## _array; \
    type ## _array *type ## _array_new(size_t size);

shark_array_type(shark_bool)
shark_array_type(shark_int)
shark_array_type(shark_float)
shark_array_type(shark_char)
shark_array_type(shark_any)

#undef shark_array_type

typedef shark_any_array shark_object_array;

#define shark_object_array_new  shark_any_array_new

#endif  // __KO

/******************************************************************************
*** Copyright *****************************************************************
** 
** Copyright 2022 Daniel Alvarez <shogundevel@gmail.com>
** 
** Permission is hereby granted, free of charge, to any person
** obtaining a copy of self software and associated documentation files
** (the "Software"), to deal in the Software without restriction,
** including without limitation the rights to use, copy, modify, merge,
** publish, distribute, sublicense, and/or sell copies of the Software,
** and to permit persons to whom the Software is furnished to do so,
** subject to the following conditions:
** 
** The above copyright notice and self permission notice shall be
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

shark_int shark_lib_EXIT_SUCCESS = 0;
shark_int shark_lib_EXIT_FAILURE = -1;

void shark_lib_exit(shark_int code)
{
    exit(code);
}

shark_int shark_lib_ERR_NONE = 0;
shark_int shark_lib_ERR_UNKNOWN = 1;

static shark_int err_code = 0;

shark_int shark_lib_get_err()
{
    return err_code;
}

void shark_lib_set_err(shark_int value)
{
    err_code = value;
}

void shark_lib_clear_err()
{
    err_code = 0;
}

shark_int shark_lib_ERR_INVALID_LITERAL = 3;

shark_int shark_lib_stoi(shark_str x)
{
    return (shark_int) atol(x->data);
}

shark_float shark_lib_stof(shark_str x)
{
    return (shark_float) atof(x->data);
}

static void shark_buffer_overflow()
{
    fprintf(stderr, "buffer overflow");
    exit(-1);
}

shark_str shark_lib_itos(shark_int x)
{
    char data[256];
    size_t size;
    if ((size = sprintf(data, "%d", x)) > 256) shark_buffer_overflow();
    return shark_str_new(data);
}

shark_str shark_lib_ftos(shark_float x)
{
    char data[256];
    size_t size;
    if ((size = sprintf(data, "%f", x)) > 256) shark_buffer_overflow();
    return shark_str_new(data);
}

shark_int shark_lib_ctoi(shark_char x)
{
    return (shark_int) x;
}

shark_char shark_lib_itoc(shark_int x)
{
    return (shark_char) x;
}

shark_bool shark_lib_islower(shark_char x)
{
    return x >= 'a' && x <= 'z';
}

shark_bool shark_lib_isupper(shark_char x)
{
    return x >= 'A' && x <= 'Z';
}

shark_char shark_lib_tolower(shark_char x)
{
    if (x >= 'A' && x <= 'Z')
        return x - 'A' + 'a';
    else
        return x;
}

shark_char shark_lib_toupper(shark_char x)
{
    if (x >= 'a' && x <= 'z')
        return x - 'a' + 'A';
    else
        return x;
}

shark_bool shark_lib_isalpha(shark_char x)
{
    return (x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z');
}

shark_bool shark_lib_isdigit(shark_char x)
{
    return x >= '0' && x <= '9';
}

shark_bool shark_lib_isalnum(shark_char x)
{
    return (x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z') || (x >= '0' && x <= '9');
}

shark_bool shark_lib_isident(shark_char x)
{
    return (x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z') || (x >= '0' && x <= '9') || x == '_';
}

typedef struct _c_type_strbuf *shark_type_strbuf;
typedef struct _c_class_strbuf shark_class_strbuf;

struct _c_type_strbuf
{
  shark_object __super;
  size_t length;
  size_t size;
  char *data;
};

struct _c_class_strbuf
{
  shark_object_class __super;
  void (*strbuf)(void *);
  void (*put)(void *, shark_char);
  void (*write)(void *, shark_str);
  shark_str (*get)(void *);
};

shark_class_strbuf shark_impl_strbuf;

#define shark_STRBUF_INIT_SIZE      16
#define shark_STRBUF_GROW_SIZE(x)   (x << 1)

void shark_lib_strbuf_strbuf(void *self)
{
    shark_type_strbuf object = self;
    object->length = 0;
    object->size = shark_STRBUF_INIT_SIZE;
    object->data = shark_allocate(shark_STRBUF_INIT_SIZE);
    if (object->data == NULL) shark_memory_error();
}

shark_type_strbuf shark_type_strbuf_new()
{
    shark_type_strbuf object = shark_object_new(&shark_impl_strbuf);
    shark_lib_strbuf_strbuf(object);
    return object;
}

void shark_lib_strbuf_put(void *self, shark_char value)
{
    shark_type_strbuf object = self;
    object->data[object->length++] = (char) value;

    if (object->length == object->size)
    {
        object->size = shark_STRBUF_GROW_SIZE(object->size);
        object->data = shark_realloc(object->data, object->size);
        if (object->data == NULL) shark_memory_error();
    }
}

void shark_lib_strbuf_write(void *self, shark_str data)
{
    shark_type_strbuf object = self;

    size_t length = object->length + data->size;
    size_t new_size = object->size;

    while (new_size < length) new_size = shark_STRBUF_GROW_SIZE(new_size);

    object->data = shark_realloc(object->data, new_size);
    if (object->data == NULL) shark_memory_error();
    memcpy(object->data + object->length, data->data, data->size);
    object->length = length;
    object->size = new_size;
}

shark_str shark_lib_strbuf_get(void *self)
{
    shark_lib_strbuf_put(self, '\0');
    shark_str data = shark_str_new(((shark_type_strbuf) self)->data);
    shark_dealloc(((shark_type_strbuf) self)->data);
    shark_lib_strbuf_strbuf(self);
    return data;
}

shark_class_strbuf shark_impl_strbuf = {
    { sizeof(struct _c_type_strbuf), NULL },
    shark_lib_strbuf_strbuf,
    shark_lib_strbuf_put,
    shark_lib_strbuf_write,
    shark_lib_strbuf_get
};

typedef struct _c_type_striter *shark_type_striter;
typedef struct _c_class_striter shark_class_striter;

struct _c_type_striter
{
  shark_object __super;
  size_t pos;
  size_t size;
  char *data;
};

struct _c_class_striter
{
  shark_object_class __super;
  void (*striter)(void *, shark_str);
  shark_int (*tell)(void *);
  void (*seek)(void *, shark_int);
  shark_char (*next)(void *);
  shark_bool (*empty)(void *);
};

shark_class_striter shark_impl_striter;

void shark_lib_striter_striter(void *self, shark_str data)
{
    shark_type_striter object = self;
    object->pos = 0;
    object->size = data->size;
    object->data = data->data;
}

shark_type_striter shark_type_striter_new(shark_str data)
{
    shark_type_striter object = shark_object_new(&shark_impl_striter);
    shark_lib_striter_striter(object, data);
    return object;
}

shark_int shark_lib_striter_tell(void *self)
{
    return ((shark_type_striter) self)->pos;
}

void shark_lib_striter_seek(void *self, shark_int pos)
{
    ((shark_type_striter) self)->pos = (size_t) pos;
}

shark_char shark_lib_striter_next(void *self)
{
    shark_type_striter object = self;
    return object->data[object->pos++];
}

shark_bool shark_lib_striter_empty(void *self)
{
    shark_type_striter object = self;
    return object->pos == object->size;
}

shark_class_striter shark_impl_striter = {
    { sizeof(struct _c_type_striter), NULL },
    shark_lib_striter_striter,
    shark_lib_striter_tell,
    shark_lib_striter_seek,
    shark_lib_striter_next,
    shark_lib_striter_empty
};

typedef struct _c_type_strmap *shark_type_strmap;
typedef struct _c_class_strmap shark_class_strmap;

typedef struct {
    size_t hash;
    shark_str key;
    shark_int value;
} shark_strmap_slot;

struct _c_type_strmap
{
  shark_object __super;
  size_t count;
  size_t size;
  size_t mask;
  shark_strmap_slot *data;
};

struct _c_class_strmap
{
  shark_object_class __super;
  void (*strmap)(void *);
  shark_int (*get)(void *, shark_str);
  void (*set)(void *, shark_str, shark_int);
  shark_bool (*has)(void *, shark_str);
  shark_int (*pop)(void *, shark_str);
};

shark_class_strmap shark_impl_strmap;

#define shark_STRMAP_INIT_SIZE      8

void shark_lib_strmap_strmap(void *self)
{
    shark_type_strmap object = self;
    object->count = 0;
    object->size = shark_STRMAP_INIT_SIZE;
    object->mask = shark_STRMAP_INIT_SIZE - 1;
    object->data = shark_allocate(sizeof(shark_strmap_slot) * object->size);
}

shark_type_strmap shark_type_strmap_new()
{
    shark_type_strmap object = shark_object_new(&shark_impl_strmap);
    shark_lib_strmap_strmap(object);
    return object;
}

static size_t shark_find_closest_shift(size_t n)
{
    size_t i;
    for (i = 0; n; i++) n >>= 1;
    return i;
}

static void shark_strmap_set_closest_size(shark_type_strmap object, size_t size)
{
    if (size < shark_STRMAP_INIT_SIZE) size = shark_STRMAP_INIT_SIZE;
    object->size = 1 << shark_find_closest_shift(size);
    object->mask = object->size - 1;
}

static size_t shark_strmap_lookup_slot(shark_type_strmap object, shark_str key, size_t *target_hash)
{
    size_t step = 0;
    size_t hash = shark_str_hash(key);
    size_t mask = object->mask;

    size_t slot_index = hash & mask;
    size_t slot_hash = object->data[slot_index].hash;

    if (target_hash != NULL) *target_hash = hash;

    while (slot_hash != 0)
    {
        if (slot_hash == hash && shark_str_equals(object->data[slot_index].key, key))
            return slot_index;
        step++;
        slot_index += step;
        slot_index &= mask;
        slot_hash = object->data[slot_index].hash;
    }

    return slot_index;
}

shark_int shark_lib_strmap_get(void *self, shark_str index)
{
    shark_type_strmap object = self;
    size_t slot_index = shark_strmap_lookup_slot(object, index, NULL);
    return object->data[slot_index].value;
}

static void shark_strmap_resize(shark_type_strmap object)
{
    size_t old_size = object->size;
    shark_strmap_slot *old_data = object->data;

    shark_strmap_set_closest_size(object, (size_t) (object->count * 1.333));
    object->data = shark_allocate(sizeof(shark_strmap_slot) * object->size);

    for (size_t i = 0; i < old_size; i++)
    {
        size_t slot_hash = old_data[i].hash;
        if (slot_hash == 0) continue;

        size_t mask = object->mask;
        size_t slot_index = slot_hash & mask;
        size_t step = 0;

        while (object->data[slot_index].hash != 0)
        {
            step++;
            slot_index += step;
            slot_index &= mask;
        }

        object->data[slot_index].hash = slot_hash;
        object->data[slot_index].key = old_data[i].key;
        object->data[slot_index].value = old_data[i].value;
    }

    shark_dealloc(old_data);
}

static void shark_strmap_maybe_resize(shark_type_strmap object)
{
    if ((object->size > object->count * 4 && object->size > shark_STRMAP_INIT_SIZE)
    || (object->size <= object->count + (object->count / 16)))
        shark_strmap_resize(object);
}

static void shark_strmap_insert(shark_type_strmap object, size_t slot,
    size_t hash, shark_str key, shark_int value)
{
    size_t old_hash = object->data[slot].hash;
    
    object->data[slot].hash = hash;
    object->data[slot].key = key;
    object->data[slot].value = value;

    if (old_hash == 0)
    {
        object->count++;
        shark_strmap_maybe_resize(object);
    }
}

void shark_lib_strmap_set(void *self, shark_str index, shark_int value)
{
    size_t hash;
    size_t slot = shark_strmap_lookup_slot(self, index, &hash);
    shark_strmap_insert(self, slot, hash, index, value);
}

shark_bool shark_lib_strmap_has(void *self, shark_str index)
{
    size_t slot = shark_strmap_lookup_slot(self, index, NULL);
    return ((shark_type_strmap) self)->data[slot].hash != 0;
}

static void shark_strmap_remove_slot(shark_type_strmap object, size_t slot)
{
    memset(&object->data[slot], 0, sizeof(shark_strmap_slot));
    object->count--;
}

shark_int shark_lib_strmap_pop(void *self, shark_str index)
{
    shark_type_strmap object = self;
    size_t slot = shark_strmap_lookup_slot(object, index, NULL);
    shark_int value = 0;
    if (object->data[slot].hash != 0) value = object->data[slot].value;
    shark_strmap_remove_slot(object, slot);
    shark_strmap_maybe_resize(object);
    return value;
}

shark_class_strmap shark_impl_strmap = {
    { sizeof(struct _c_type_strmap), NULL },
    shark_lib_strmap_strmap,
    shark_lib_strmap_get,
    shark_lib_strmap_set,
    shark_lib_strmap_has,
    shark_lib_strmap_pop,
};

typedef struct _c_type_list *shark_type_list;
typedef struct _c_class_list shark_class_list;

struct _c_type_list
{
  shark_object __super;
  size_t length;
  size_t size;
  shark_any *data;
};

struct _c_class_list
{
  shark_object_class __super;
  void (*list)(void *);
  shark_int (*size)(void *);
  void (*push)(void *, shark_any);
  shark_any (*pop)(void *);
  shark_any (*get)(void *, shark_int);
  void (*set)(void *, shark_int, shark_any);
};

shark_class_list shark_impl_list;

#define shark_LIST_INIT_SIZE        8
#define shark_LIST_GROW_SIZE(x)     (x << 1)
#define shark_LIST_SHRINK_SIZE(x)   (x >> 1)

void shark_lib_list_list(void *self)
{
    shark_type_list object = self;
    object->length = 0;
    object->size = shark_LIST_INIT_SIZE;
    object->data = shark_allocate(sizeof(shark_any) * shark_LIST_INIT_SIZE);
    if (object->data == NULL) shark_memory_error();
}

shark_type_list shark_type_list_new()
{
    shark_type_list object = shark_object_new(&shark_impl_list);
    shark_lib_list_list(object);
    return object;
}

shark_int shark_lib_list_size(void *self)
{
    return (shark_int) ((shark_type_list) self)->length;
}

void shark_lib_list_push(void *self, shark_any value)
{
    shark_type_list object = self;
    object->data[object->length++] = value;

    if (object->length == object->size)
    {
        size_t new_size = shark_LIST_GROW_SIZE(object->size);
        shark_any *new_data = shark_allocate(sizeof(shark_any) * new_size);
        if (new_data == NULL) shark_memory_error();
        memcpy(new_data, object->data, object->size * sizeof(shark_any));
        shark_dealloc(object->data);
        object->size = new_size;
        object->data = new_data;
    }
}

shark_any shark_lib_list_pop(void *self)
{
    shark_type_list object = self;

    if (object->length == 0)
    {
        fprintf(stderr, "pop from empty list.");
        exit(-1);
    }

    shark_any value = object->data[--object->length];

    if (object->length < (object->size / 4) && object->size > 8)
    {
        size_t new_size = shark_LIST_SHRINK_SIZE(object->size);
        shark_any *new_data = shark_allocate(sizeof(shark_any) * new_size);
        if (new_data == NULL) shark_memory_error();
        memcpy(new_data, object->data, new_size * sizeof(shark_any));
        shark_dealloc(object->data);
        object->size = new_size;
        object->data = new_data;
    }

    return value;
}

shark_any shark_lib_list_get(void *self, shark_int index)
{
    return ((shark_type_list) self)->data[(size_t) index];
}

void shark_lib_list_set(void *self, shark_int index, shark_any value)
{
    ((shark_type_list) self)->data[(size_t) index] = value;
}

shark_class_list shark_impl_list = {
    { sizeof(struct _c_type_list), NULL },
    shark_lib_list_list,
    shark_lib_list_size,
    shark_lib_list_push,
    shark_lib_list_pop,
    shark_lib_list_get,
    shark_lib_list_set,
};

shark_int shark_lib_ERR_FILE_NOT_FOUND = 4;

typedef struct _c_type_FILE *shark_type_FILE;
typedef struct _c_class_FILE shark_class_FILE;

struct _c_type_FILE
{
  shark_object __super;
  FILE *stream;
};

struct _c_class_FILE
{
  shark_object_class __super;
  void (*close)(void *);
  shark_bool (*at_end)(void *);
  void (*put)(void *, shark_char);
  void (*write)(void *, shark_str);
  shark_char (*fetch)(void *);
};

shark_class_FILE shark_impl_FILE;

void shark_lib_FILE_close(void *self)
{
    fclose(((shark_type_FILE) self)->stream);
    shark_dealloc(self);
}

shark_bool shark_lib_FILE_at_end(void *self)
{
    FILE *stream = ((shark_type_FILE) self)->stream;
    int c = fgetc(stream);
    ungetc(c, stream);
    return c == EOF;
}

void shark_lib_FILE_put(void *self, shark_char value)
{
    fputc(value, ((shark_type_FILE) self)->stream);
}

void shark_lib_FILE_write(void *self, shark_str data)
{
    fputs(data->data, ((shark_type_FILE) self)->stream);
}

shark_char shark_lib_FILE_fetch(void *self)
{
    return fgetc(((shark_type_FILE) self)->stream);
}

shark_class_FILE shark_impl_FILE = {
    { sizeof(struct _c_type_FILE), NULL },
    shark_lib_FILE_close,
    shark_lib_FILE_at_end,
    shark_lib_FILE_put,
    shark_lib_FILE_write,
    shark_lib_FILE_fetch,
};

shark_type_FILE shark_lib_open(shark_str filename, shark_char mode)
{
    if (mode != 'w' && mode != 'r')
    {
        fprintf(stderr, "invalid open mode in call to 'open'");
        exit(-1);
    }

    char open_mode[] = { mode, '\0' };
    FILE *stream = fopen(filename->data, open_mode);

    if (stream == NULL)
    {
        shark_lib_set_err(shark_lib_ERR_FILE_NOT_FOUND);
        return NULL;
    }

    shark_type_FILE file = shark_object_new(&shark_impl_FILE);
    file->stream = stream;
    return file;
}

void shark_lib_write(shark_str data)
{
    printf(data->data);
}

shark_str shark_lib_read()
{
    char *line = (char *) shark_allocate(sizeof(char) * (256 + 1));
    size_t step = 0;
    size_t index = 0;
    size_t size;
    
    while (true)
    {
        size_t base = step * 256;
        
        for (index = 0; index < 256; index++)
        {
            int c = fgetc(stdin);
            if (c == EOF || c == '\n')
                goto end;
            line[base + index] = (char) c;
        }
        
        line = (char *) shark_realloc(line, base + 256 + 1);
        step++;
    }
end:
    size = step * 256 + index;
    line[size] = '\0';
    shark_str read = shark_str_new(line);
    shark_dealloc(line);
    return read;
}

#include <math.h>

shark_float pi = M_PI;
shark_float e = M_E;

shark_float shark_lib_real(shark_int x)
{
    return (shark_float) x;
}

shark_int shark_lib_idiv(shark_int x, shark_int y)
{
    return x / y;
}

shark_float shark_lib_abs(shark_float x)
{
    return fabs(x);
}

shark_float shark_lib_acos(shark_float x)
{
    return acos(x);
}

shark_float shark_lib_asin(shark_float x)
{
    return asin(x);
}

shark_float shark_lib_atan(shark_float x)
{
    return atan(x);
}

shark_float shark_lib_atan2(shark_float y, shark_float x)
{
    return atan2(y, x);
}

shark_float shark_lib_cos(shark_float x)
{
    return cos(x);
}

shark_float shark_lib_cosh(shark_float x)
{
    return cosh(x);
}

shark_float shark_lib_sin(shark_float x)
{
    return sin(x);
}

shark_float shark_lib_sinh(shark_float x)
{
    return sinh(x);
}

shark_float shark_lib_tan(shark_float x)
{
    return tan(x);
}

shark_float shark_lib_tanh(shark_float x)
{
    return tanh(x);
}

shark_float shark_lib_exp(shark_float x)
{
    return exp(x);
}

shark_float shark_lib_log(shark_float x)
{
    return log(x);
}

shark_float shark_lib_log10(shark_float x)
{
    return log10(x);
}

shark_float shark_lib_pow(shark_float x, shark_float y)
{
    return pow(x, y);
}

shark_float shark_lib_sqrt(shark_float x)
{
    return sqrt(x);
}

shark_int shark_lib_ceil(shark_float x)
{
    return (shark_int) ceil(x);
}

shark_int shark_lib_floor(shark_float x)
{
    return (shark_int) floor(x);
}

shark_float shark_lib_min(shark_float x, shark_float y)
{
    return x > y ? y : x;
}

shark_float shark_lib_max(shark_float x, shark_float y)
{
    return x > y ? x : y;
}

shark_int shark_lib_random(shark_int x)
{
    shark_int random = (shark_int) (x * (rand() / (double) RAND_MAX));
    if (random && random == x) random--;
    return random;
}

#ifndef SHARK_NO_FS
#ifdef _WIN32
    #include <windows.h>
    #include <sys/stat.h>
    #include <direct.h>
    #include <io.h>
#else
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <dirent.h>
#endif

shark_type_list shark_lib_listdir(shark_str path)
{
    shark_type_list list = shark_type_list_new();
#ifdef _WIN32
    if (path->size > 250) return NULL;
    
    char pattern[256];
    
    memcpy(pattern, path->data, path->size);
    memcpy(pattern + path->size, "\\*", 3);
    
    WIN32_FIND_DATA find_data;
    HANDLE find_handle;
    
    if ((find_handle = FindFirstFileA(pattern, &find_data)) == INVALID_HANDLE_VALUE)
        return NULL;
    do {
        shark_str filename = shark_str_new(find_data.cFileName);
        shark_lib_list_push(list, filename);
    } while (FindNextFile(find_handle, &find_data) != 0);
    FindClose(find_handle);
#else
    struct dirent *entry;
    DIR *dir = opendir(path->data);
    
    if (dir == NULL) return NULL;
    
    while ((entry = readdir(dir)) != NULL)
    {
        shark_str filename = shark_str_new(entry->d_name);
        shark_lib_list_push(list, filename);
    }
    
    closedir(dir);
#endif
    
    return list;
}

shark_bool shark_lib_mkdir(shark_str path)
{
#ifdef _WIN32
    return mkdir(path->data) ? false : true;
#else
    return mkdir(path->data, S_IRWXU) ? false : true;
#endif
}

shark_bool shark_lib_rmdir(shark_str path)
{
    return rmdir(path->data) ? false : true;
}

shark_bool shark_lib_unlink(shark_str filename)
{
    return unlink(filename->data) ? false : true;
}
#endif

#include <time.h>

shark_float shark_lib_clock()
{
    return clock() / (double) CLOCKS_PER_SEC;
}

void shark_lib_free(shark_any value)
{
    shark_dealloc(value);
}

void shark_lib_free_strbuf(shark_type_strbuf value)
{
    shark_dealloc(value);
}

void shark_lib_free_striter(shark_type_striter value)
{
    shark_dealloc(value);
}

void shark_lib_free_strmap(shark_type_strmap value)
{
    shark_dealloc(value);
}

void shark_lib_free_list(shark_type_list value)
{
    shark_dealloc(value);
}

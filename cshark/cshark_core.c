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

#define CSHARK_BYTECODE
#include "cshark.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SHARK_API void shark_fatal_error(void *vm, char *message)
{
    fprintf(stderr, "%s\n", message);
    if (vm != NULL) shark_print_stack_trace(vm);
    exit(-1);
}

SHARK_API void shark_memory_error()
{
    shark_fatal_error(NULL, "memory allocation failure.");
}

SHARK_API void *shark_malloc(size_t object_size)
{
	void *object = malloc(object_size);
    if (object == NULL) shark_memory_error();
    return object;
}

SHARK_API void *shark_realloc(void *object, size_t new_size)
{
	void *new_object = realloc(object, new_size);
    if (new_object == NULL) shark_memory_error();
    return new_object;
}

SHARK_API void *shark_calloc(size_t count, size_t object_size)
{
    void *object = calloc(count, object_size);
    if (object == NULL) shark_memory_error();
    return object;
}

SHARK_API void *shark_object_new(shark_class *type)
{
    shark_object *self = shark_zalloc(type->object_size);
    self->type = type;
    self->ref_count = 1;
    return self;
}

SHARK_API void shark_object_delete(void *object)
{
    shark_object *self = object;
    // printf("deleting %s object at %p\n", shark_class_get_name(self->type), self);
    self->type->destroy(self);
    shark_free(self);
}

SHARK_API void shark_default_destroy(shark_object *self) {
    // do nothing
}

SHARK_API void *shark_object_inc_ref(void *self)
{
    if (self != NULL) ((shark_object *) self)->ref_count++;
    return self;
}

SHARK_API void shark_object_dec_ref(void *self)
{
    shark_object *object = self;
    if (self != NULL && !(--object->ref_count)) shark_object_delete(object);
}

SHARK_API bool shark_object_instanceof(shark_object *self, shark_class *type)
{
    for (shark_class *candidate = type; candidate != NULL; candidate = candidate->parent) {
        if (self->type == candidate) return true;
    }
    return false;
}

SHARK_API shark_value shark_value_inc_ref(shark_value self)
{
    if (SHARK_IS_OBJECT(self))
        shark_object_inc_ref(SHARK_AS_OBJECT(self));
    return self;
}

SHARK_API void shark_value_dec_ref(shark_value self)
{
    if (SHARK_IS_OBJECT(self))
        shark_object_dec_ref(SHARK_AS_OBJECT(self));
}

static shark_class shark_class_class;

static shark_class shark_object_class = {
    { &shark_class_class, 1 },
    NULL,
    "object",
    NULL,
    sizeof(shark_object),
    shark_default_destroy,
    false,
    NULL
};

static shark_class shark_class_class = {
    { &shark_class_class, 1 },
    NULL,
    "class",
    &shark_object_class,
    sizeof(shark_class),
    shark_default_destroy,
    false,
    NULL
};

SHARK_API char *shark_class_get_name(shark_class *type)
{
    if (type->cname != NULL)
        return type->cname;
    else
        return type->name->data;
}

SHARK_API shark_bool_t shark_object_is_class(shark_object *object)
{
    return object->type == &shark_class_class;
}

SHARK_API size_t shark_hash_byte_str(size_t length, uint8_t *data)
{
    size_t hash = 5381;
    for (size_t i = 0; i < length; i++) {
        hash = ((hash << 5) + hash + *(data++)) & 4294967295;
    }
    return hash;
}

SHARK_API void shark_string_init(shark_string *self)
{
    self->hash = shark_hash_byte_str(self->size, self->data);
}

static void shark_string_destroy(shark_object *self)
{
    shark_free(((shark_string *) self)->data);
}

static shark_class shark_string_class = {
    { &shark_class_class, 1 },
    NULL,
    "str",
    &shark_object_class,
    sizeof(shark_string),
    shark_string_destroy,
    false,
    NULL
};

SHARK_API void *shark_string_new_with_size(size_t size)
{
    shark_string *self = shark_object_new(&shark_string_class);
    self->size = size;
    self->data = shark_malloc(size + 1);
    self->data[size] = '\0';
    return self;
}

SHARK_API void *shark_string_new_from_byte_str(size_t size, uint8_t *data)
{
    shark_string *self = shark_object_new(&shark_string_class);
    self->size = size;
    self->data = shark_malloc(size + 1);
    memcpy(self->data, data, size);
    self->data[size] = '\0';
    shark_string_init(self);
	return self;
}

SHARK_API void *shark_string_new_from_cstr(char *data)
{
    shark_string *self = shark_object_new(&shark_string_class);
    size_t size = strlen(data);
    self->size = size;
    self->data = shark_malloc(size + 1);
    memcpy(self->data, data, size);
    self->data[size] = '\0';
    shark_string_init(self);
    return self;
}

SHARK_API bool shark_string_equals(shark_string *self, shark_string *other)
{
    if (self == other) return true;
    if (self->hash != other->hash) return false;
    if (self->size != other->size) return false;
    return memcmp(self->data, other->data, self->size) == 0;
}

static void shark_array_destroy(shark_object *object)
{
    shark_array *self = (shark_array *) object;
    for (size_t i = 0; i < self->length; i++)
        shark_value_dec_ref(self->data[i]);
    shark_free(self->data);
}

static shark_class shark_array_class = {
    { &shark_class_class, 1 },
    NULL,
    "array",
    &shark_object_class,
    sizeof(shark_array),
    shark_array_destroy,
    false,
    NULL
};

SHARK_API void *shark_array_new()
{
    shark_array *self = shark_object_new(&shark_array_class);
    self->length = 0;
    self->size = SHARK_ARRAY_INIT_SIZE;
    self->data = shark_malloc(sizeof(shark_value) * SHARK_ARRAY_INIT_SIZE);
    return self;
}

SHARK_API void shark_array_preallocate(shark_array *self, size_t size)
{
    size_t new_size = SHARK_ARRAY_INIT_SIZE;
    while (new_size <= size) new_size = SHARK_ARRAY_GROW_SIZE(new_size);

    if (new_size <= self->size) return;

    self->size = new_size;
    self->data = shark_realloc(self->data, sizeof(shark_value) * new_size);
}

SHARK_API void shark_array_grow(shark_array *self)
{
    if (self->length >= self->size)
    {
        self->size = SHARK_ARRAY_GROW_SIZE(self->size);
        self->data = shark_realloc(self->data, sizeof(shark_value) * self->size);
    }
}

SHARK_API void shark_array_put(shark_array *self, shark_value value)
{
    self->data[self->length++] = shark_value_inc_ref(value);
    shark_array_grow(self);
}

SHARK_API void shark_array_shrink(shark_array *self)
{
    if (self->length <= self->size >> 2 && self->size >= 16)
    {
        self->size = SHARK_ARRAY_SHRINK_SIZE(self->size);
        self->data = shark_realloc(self->data, sizeof(shark_value) * self->size);
    }
}

SHARK_API shark_value shark_array_pop(shark_array *self)
{
    if (self->length == 0)
        shark_fatal_error(NULL, "pop from empty array.");
    shark_value value = self->data[--self->length];
    shark_array_shrink(self);
    return value;
}

static void shark_table_destroy(shark_object *object)
{
    shark_table *self = (shark_table *) object;
    for (size_t i = 0; i < self->size; i++) {
        if (self->data[i].hash != SHARK_TABLE_HASH_NULL) {
            shark_value_dec_ref(self->data[i].key);
            shark_value_dec_ref(self->data[i].value);
        }
    }
    shark_free(self->data);
}

static shark_class shark_table_class = {
    { &shark_class_class, 1 },
    NULL,
    "table",
    &shark_object_class,
    sizeof(shark_table),
    shark_table_destroy,
    false,
    NULL
};

static void shark_table_init(shark_table *self)
{
    self->count = 0;
    self->size = SHARK_TABLE_INIT_SIZE;
    self->mask = SHARK_TABLE_INIT_SIZE - 1;
    self->data = shark_zalloc(sizeof(shark_table_slot) * SHARK_TABLE_INIT_SIZE);
}

SHARK_API shark_table *shark_table_new()
{
    shark_table *self = shark_object_new(&shark_table_class);
    shark_table_init(self);
    return self;
}

SHARK_API shark_table *shark_table_copy(shark_table *self)
{
    shark_table *copy = shark_object_new(&shark_table_class);
    copy->count = self->count;
    copy->size = self->size;
    copy->mask = self->mask;
    size_t data_size = sizeof(shark_table_slot) * self->size;
    copy->data = shark_malloc(data_size);
    memcpy(copy->data, self->data, data_size);
    for (size_t i = 0; i < copy->size; i++) {
        if (copy->data[i].hash != SHARK_TABLE_HASH_NULL) {
            shark_value_inc_ref(copy->data[i].key);
            shark_value_inc_ref(copy->data[i].value);
        }
    }
    return copy;
}

static size_t find_closest_shift(size_t n)
{
    size_t i;
    for (i = 0; n; i++) n >>= 1;
    return i;
}

static void shark_table_set_closest_size(shark_table *self, size_t size)
{
    if (size < SHARK_TABLE_INIT_SIZE) size = SHARK_TABLE_INIT_SIZE;
    self->size = 1 << find_closest_shift(size);
    self->mask = self->size - 1;
}

SHARK_API size_t shark_value_hash(shark_value x)
{
#ifdef SHARK_UNBOX
    switch (x.type)
    {
        case SHARK_TYPE_VOID:
            return 1;
        case SHARK_TYPE_BOOL:
            return SHARK_AS_BOOL(x) == true ? 2 : 1;
        case SHARK_TYPE_NUM:
            return (size_t) SHARK_AS_NUM(x);
        case SHARK_TYPE_CHAR:
            return (size_t) SHARK_AS_CHAR(x);
        case SHARK_TYPE_OBJECT:
            if (SHARK_AS_OBJECT(x)->type == &shark_string_class)
                return SHARK_AS_STR(x)->hash;
            return (size_t) SHARK_AS_OBJECT(x);
        default:
            shark_fatal_error(NULL, "unknown value type.");
    }
#else
    if (SHARK_IS_OBJECT(x) && SHARK_AS_OBJECT(x)->type == &shark_string_class)
        return SHARK_AS_STR(x)->hash;
    else
        return (size_t) x.INT;
#endif
}

SHARK_API bool shark_value_equals(shark_value x, shark_value y)
{
#ifdef SHARK_UNBOX
    if (x.type != y.type) return false;
    switch (x.type)
    {
        case SHARK_TYPE_VOID:
            return true;
        case SHARK_TYPE_BOOL:
            return SHARK_AS_BOOL(x) == SHARK_AS_BOOL(y);
        case SHARK_TYPE_NUM:
            return SHARK_AS_NUM(x) == SHARK_AS_NUM(y);
        case SHARK_TYPE_CHAR:
            return SHARK_AS_CHAR(x) == SHARK_AS_CHAR(y);
        case SHARK_TYPE_OBJECT:
            if (SHARK_AS_OBJECT(x) == SHARK_AS_OBJECT(y)) return true;
            if (SHARK_AS_OBJECT(x)->type == &shark_string_class
            && SHARK_AS_OBJECT(y)->type == &shark_string_class)
                return shark_string_equals(SHARK_AS_STR(x), SHARK_AS_STR(y));
            return false;
        default:
            shark_fatal_error(NULL, "unknown value type.");
    }
#else
    if (SHARK_IS_OBJECT(x) && SHARK_IS_OBJECT(y)
    && SHARK_AS_OBJECT(x)->type == SHARK_AS_OBJECT(y)->type
    && SHARK_AS_OBJECT(x)->type == &shark_string_class)
        return shark_string_equals(SHARK_AS_STR(x), SHARK_AS_STR(y));
    else
        return x.INT == y.INT;
#endif
}

static size_t shark_table_lookup_slot(shark_table *self, shark_value key, size_t *target_hash)
{
    size_t step = 0;
    size_t hash = shark_value_hash(key);
    size_t mask = self->mask;

    if (hash == SHARK_TABLE_HASH_NULL)
        hash = SHARK_TABLE_HASH_NOT_NULL;

    size_t slot_index = hash & mask;
    size_t slot_hash = self->data[slot_index].hash;

    if (target_hash != NULL) *target_hash = hash;

    while (slot_hash != SHARK_TABLE_HASH_NULL)
    {
        if (slot_hash == hash && shark_value_equals(self->data[slot_index].key, key))
            return slot_index;
        step++;
        slot_index += step;
        slot_index &= mask;
        slot_hash = self->data[slot_index].hash;
    }

    return slot_index;
}

SHARK_API shark_value shark_table_get_index(shark_table *self, shark_value key)
{
    size_t slot_index = shark_table_lookup_slot(self, key, NULL);
    return self->data[slot_index].value;
}

SHARK_API shark_value shark_table_get_str(shark_table *self, char *value)
{
    size_t size = strlen(value);
    shark_string key = (shark_string) {{ &shark_string_class, 1 },
        size, shark_hash_byte_str(size, (uint8_t *) value), value };
    return shark_table_get_index(self, SHARK_FROM_PTR(&key));
}

static void shark_table_resize(shark_table *self)
{
    size_t old_size = self->size;
    shark_table_slot *old_data = self->data;

    shark_table_set_closest_size(self, (size_t) (self->count * 1.333));
    self->data = shark_zalloc(sizeof(shark_table_slot) * self->size);

    for (size_t i = 0; i < old_size; i++)
    {
        size_t slot_hash = old_data[i].hash;
        if (slot_hash == SHARK_TABLE_HASH_NULL) continue;

        size_t mask = self->mask;
        size_t slot_index = slot_hash & mask;
        size_t step = 0;

        while (self->data[slot_index].hash != SHARK_TABLE_HASH_NULL)
        {
            step++;
            slot_index += step;
            slot_index &= mask;
        }

        self->data[slot_index].hash = slot_hash;
        self->data[slot_index].key = old_data[i].key;
        self->data[slot_index].value = old_data[i].value;
    }
}

static void shark_table_maybe_resize(shark_table *self)
{
    if ((self->size > self->count * 4 && self->size > SHARK_TABLE_INIT_SIZE)
    || (self->size <= self->count + (self->count / 16)))
        shark_table_resize(self);
}

static void shark_table_insert(shark_table *self, size_t slot, size_t hash,
                        shark_value key, shark_value value)
{
    size_t old_hash = self->data[slot].hash;

    if (old_hash != SHARK_TABLE_HASH_NULL)
    {
        shark_value_dec_ref(self->data[slot].key);
        shark_value_dec_ref(self->data[slot].value);
    }

    self->data[slot].hash = hash;
    self->data[slot].key = shark_value_inc_ref(key);
    self->data[slot].value = shark_value_inc_ref(value);

    if (old_hash == SHARK_TABLE_HASH_NULL)
    {
        self->count++;
        shark_table_maybe_resize(self);
    }
}

SHARK_API void shark_table_set_index(shark_table *self, shark_value key, shark_value value)
{
    size_t hash;
    size_t slot = shark_table_lookup_slot(self, key, &hash);
    shark_table_insert(self, slot, hash, key, value);
}

SHARK_API shark_bool_t shark_table_contains(shark_table *self, shark_value key)
{
    size_t slot = shark_table_lookup_slot(self, key, NULL);
    return self->data[slot].hash != SHARK_TABLE_HASH_NULL;
}

static void shark_table_remove_slot(shark_table *self, size_t slot)
{
    memset(&self->data[slot], 0, sizeof(shark_table_slot));
    self->count--;
}

SHARK_API void shark_table_delete_index(shark_table *self, shark_value key)
{
    size_t slot = shark_table_lookup_slot(self, key, NULL);

    shark_value_dec_ref(self->data[slot].key);
    shark_value_dec_ref(self->data[slot].value);

    shark_table_remove_slot(self, slot);
    shark_table_maybe_resize(self);
}

SHARK_API shark_value shark_table_pop_index(shark_table *self, shark_value key)
{
    size_t slot = shark_table_lookup_slot(self, key, NULL);
    shark_value value = SHARK_NULL;
    if (self->data[slot].hash != SHARK_TABLE_HASH_NULL)
        value = self->data[slot].value;
    shark_value_dec_ref(self->data[slot].key);
    shark_table_remove_slot(self, slot);
    shark_table_maybe_resize(self);
    return value;
}

SHARK_API void shark_table_update(shark_table *self, shark_table *other)
{
    for (size_t i = 0; i < other->size; i++)
    {
        if (other->data[i].hash != SHARK_TABLE_HASH_NULL)
            shark_table_set_index(self, other->data[i].key, other->data[i].value);
    }
}

static void shark_module_destroy(shark_object *object)
{
    shark_module *self = (shark_module *) object;
    shark_object_delete(self->names);
    for (size_t i = 0; i < self->const_table_size; i++)
        shark_value_dec_ref(self->const_table[i]);
    shark_free(self->const_table);
    shark_free(self->code);
}

static shark_class shark_module_class = {
    { &shark_class_class, 1 },
    NULL,
    "module",
    &shark_object_class,
    sizeof(shark_module),
    shark_module_destroy,
    false,
    NULL
};

#define fetch       ((uint8_t) fgetc(source))

#define fetch_short (((uint16_t) fetch) \
                    | (((uint16_t) fetch) << 8))

#define fetch_int   (((uint32_t) fetch) \
                    | (((uint32_t) fetch) << 8) \
                    | (((uint32_t) fetch) << 16) \
                    | (((uint32_t) fetch) << 24))

#define fetch_str(dest) { \
    size_t size = (size_t) fetch_int; \
    dest = shark_object_new(&shark_string_class); \
    dest->size = size; \
    uint8_t *data = shark_malloc(size * sizeof(uint8_t) + 1); \
    data[size] = '\0'; \
    dest->data = data; \
    for (size_t i = 0; i < size; i++) \
        *(data++) = fetch; \
    shark_string_init(dest); \
}

static shark_module *shark_read_module(shark_string *name, FILE *source)
{
    shark_module *module = shark_object_new(&shark_module_class);

    module->name = name;
    module->names = shark_table_new();

    size_t import_section_size = (size_t) fetch_short;
    shark_array *import_table = shark_array_new();
    module->import_table = import_table;

    for (size_t i = 0; i < import_section_size; i++)
    {
        shark_string *import_path;
        fetch_str(import_path);
        shark_array_put(import_table, SHARK_FROM_PTR(import_path));

        if (fetch == 0)
        {
            shark_string *alias;
            fetch_str(alias);
            shark_array_put(import_table, SHARK_FROM_PTR(alias));
        }
        else
        {
            size_t target_count = (size_t) fetch_short;
            shark_array *target_list = shark_array_new();
            shark_array_preallocate(target_list, target_count);

            for (size_t i = 0; i < target_count; i++)
            {
                shark_string *target;
                fetch_str(target);
                shark_array_put(target_list, SHARK_FROM_PTR(target));
            }

            shark_array_put(import_table, SHARK_FROM_PTR(target_list));
        }
    }

    size_t const_table_size = (size_t) fetch_int;
    shark_value *const_table = shark_malloc(const_table_size * sizeof(shark_value));

    module->const_table_size = const_table_size;
    module->const_table = const_table;

    for (size_t i = 0; i < const_table_size; i++)
    {
        shark_value value;
        uint8_t const_type = fetch;

        switch (const_type)
        {
            case CONST_INT:
            {
                value = SHARK_FROM_INT(fetch_int);
                break;
            }
            case CONST_FLOAT:
            {
                size_t size = (size_t) fetch_int;
                char data[size + 1];
                data[size] = '\0';
                for (size_t i = 0; i < size; i++)
                    data[i] = fetch;
                value = SHARK_FROM_NUM(atof(&data[0]));
                break;
            }
            case CONST_CHAR:
            {
                size_t char_size = (size_t) fetch_int;
                if (char_size == 1)
                    value = SHARK_FROM_CHAR(fetch);
                else
                    shark_fatal_error(NULL, "can't decode non ascii character.");
                break;
            }
            case CONST_STR:
            case CONST_SYMBOL:
            {
                shark_string *str;
                fetch_str(str);
                value = SHARK_FROM_PTR(str);
                break;
            }
            default:
            {
                shark_fatal_error(NULL, "unknown const type.");
                break;
            }
        }

        *(const_table++) = value;
    }

    size_t code_size = (size_t) fetch_int;
    uint8_t *code = shark_malloc(code_size * sizeof(uint8_t));
    module->code = code;

    for (size_t i = 0; i < code_size; i++)
        *(code++) = fetch;

    return module;
}

SHARK_API shark_module *shark_read_archive(shark_vm *vm, shark_string *name, void *source_file)
{
    FILE *source = source_file;
    
    if (shark_table_contains(vm->archive_record, SHARK_FROM_PTR(name)))
        return NULL;
    
    shark_table_set_index(vm->archive_record, SHARK_FROM_PTR(name), SHARK_TRUE);
    
    size_t main_name_size = fetch;
    shark_string *main_name = shark_string_new_with_size(main_name_size);
    for (size_t i = 0; i < main_name_size; i++) main_name->data[i] = fetch;
    shark_string_init(main_name);
    
    size_t import_table_size = (size_t) fetch;
    
    for (size_t i = 0; i < import_table_size; i++)
    {
        size_t archive_name_size = fetch;
        shark_string *archive_name = shark_string_new_with_size(archive_name_size);
        for (size_t k = 0; k < archive_name_size; k++)
            archive_name->data[k] = fetch;
        shark_string_init(archive_name);
        
        size_t path_size = vm->max_import_path + 1 + archive_name->size;
        char *path = shark_malloc(path_size + 1);
        FILE *source = NULL;
        
        for (size_t i = 0; i < vm->import_path->length; i++)
        {
            shark_string *import_path = SHARK_AS_STR(vm->import_path->data[i]);
            memcpy(path, import_path->data, import_path->size);
            path[import_path->size] = '/';
            memcpy(path + import_path->size + 1, archive_name->data, archive_name->size);
            path[import_path->size + 1 + archive_name->size] = '\0';
            
            source = fopen(path, "rb");
            if (source != NULL) break;
        }
        
        shark_free(path);
        
        if (source == NULL)
        {
            fprintf(stderr, "can't locate archive '%s', execution aborted.", name->data);
            exit(EXIT_FAILURE);
        }
        
        shark_read_archive(vm, archive_name, source);
        fclose(source);
    }
    
    while (true)
    {
        int c = fgetc(source);
        ungetc(c, source);
        if (c == EOF) break;
        shark_string *module_name;
        fetch_str(module_name);
        shark_module *module = shark_read_module(module_name, source);
        shark_table_set_index(vm->module_record, SHARK_FROM_PTR(module_name), SHARK_FROM_PTR(module));
    }
    
    return SHARK_AS_MODULE(shark_table_get_index(vm->module_record, SHARK_FROM_PTR(main_name)));
}

#undef fetch
#undef fetch_short
#undef fetch_int
#undef fetch_str

SHARK_API shark_string *shark_path_get_base(shark_string *path)
{
    int64_t index;
    for (index = path->size - 1; index >= 0; index--) {
        if (path->data[index] == '\\' || path->data[index] == '/')
            break;
    }
    if (index < 0) index = 0;
    shark_string *base = shark_string_new_with_size(index);
    memcpy(base->data, path->data, index);
    return base;
}

SHARK_API shark_string *shark_path_get_tail(shark_string *path)
{
    int64_t index;
    for (index = path->size - 1; index >= 0; index--) {
        if (path->data[index] == '\\' || path->data[index] == '/')
            break;
    }
    size_t size = path->size - index + 1;
    shark_string *tail = shark_string_new_with_size(size);
    memcpy(tail->data, path->data + index + 1, size);
    return tail;
}

SHARK_API shark_string *shark_path_get_ext(shark_string *path)
{
    int64_t index;
    bool found = false;
    for (index = path->size - 1; index >= 0; index--)
    {
        if (path->data[index] == '.') {
            found = true;
            break;
        } else if (path->data[index] == '/' || path->data[index] == '\\') {
            break;
        }
    }
    if (found) {
        return shark_string_new_from_byte_str(path->size - index, path->data + index);
    } else {
        return shark_string_new_from_cstr("");
    }
}

SHARK_API shark_string *shark_path_remove_ext(shark_string *path)
{
    int64_t index;
    bool found = false;
    for (index = path->size - 1; index >= 0; index--)
    {
        if (path->data[index] == '.') {
            found = true;
            break;
        } else if (path->data[index] == '/' || path->data[index] == '\\') {
            break;
        }
    }
    if (found) {
        return shark_string_new_from_byte_str(index, path->data);
    } else {
        return path;
    }
}

SHARK_API shark_string *shark_path_join(shark_string *x, shark_string *y)
{
    int insert_slash = 0;
    if (x->size > 0 && x->data[x->size-1] != '\\' && x->data[x->size-1] != '/')
        insert_slash = 1;
    size_t size = x->size + insert_slash + y->size;
    shark_string *join = shark_string_new_with_size(size);
    memcpy(join->data, x->data, x->size);
    if (insert_slash) join->data[x->size] = '/';
    memcpy(join->data + x->size + insert_slash, y->data, y->size);
    shark_string_init(join);
    return join;
}

SHARK_API void shark_function_destroy(shark_object *object)
{
    shark_function *self = (shark_function *) object;
    shark_object_dec_ref(self->name);
    shark_object_dec_ref(self->owner);
    shark_object_dec_ref(self->owner_class);
    shark_object_dec_ref(self->supermethod);
}

static shark_class shark_function_class = {
    { &shark_class_class, 1 },
    NULL,
    "function",
    &shark_object_class,
    sizeof(shark_function),
    shark_function_destroy,
    false,
    NULL
};

SHARK_API shark_class *shark_class_new(shark_string *name, shark_class *parent)
{
    shark_class *self = shark_object_new(&shark_class_class);
    self->name = shark_object_inc_ref(name);
    self->parent = shark_object_inc_ref(parent);
    if (parent == NULL) self->object_size = sizeof(shark_table);
    else self->object_size = parent->object_size;
    self->destroy = shark_table_destroy;
    self->is_object_class = true;
    if (parent == NULL || parent->methods == NULL) self->methods = shark_table_new();
    else self->methods = shark_table_copy(parent->methods);
    return self;
}

SHARK_API bool shark_object_is_str(shark_object *object)
{
    return object->type == &shark_string_class;
}

SHARK_API bool shark_object_is_array(shark_object *object)
{
    return object->type == &shark_array_class;
}

SHARK_API bool shark_object_is_table(shark_object *object)
{
    return object->type == &shark_table_class;
}

SHARK_API bool shark_object_is_function(shark_object *object)
{
    return object->type == &shark_function_class;
}

SHARK_API bool shark_object_is_module(shark_object *object)
{
    return object->type == &shark_module_class;
}

SHARK_API void shark_print_stack_trace(shark_vm *vm)
{
    if (vm->bottom != NULL)
    {
        fprintf(stderr, "call stack trace (most recent call first):\n");
        for (shark_vm_frame *frame = vm->bottom; frame != NULL; frame = frame->parent)
            if (frame->function != NULL)
                if (frame->function->owner_class != NULL)
                    fprintf(stderr, "\tmodule %s class %s function %s\n", frame->module->name->data, frame->function->owner_class->name->data, frame->function->name->data);
                else
                    fprintf(stderr, "\tmodule %s function %s\n", frame->module->name->data, frame->function->name->data);
            else
                fprintf(stderr, "\tmodule %s\n", frame->module->name->data);
    }
}

SHARK_API void shark_vm_destroy(shark_object *object)
{
    shark_vm *self = (shark_vm *) object;
    shark_object_dec_ref(self->import_path);
    shark_object_dec_ref(self->import_record);
    shark_free(self->stack);
}

static shark_class shark_vm_class = {
    { &shark_class_class, 1 },
    NULL,
    "shark_vm",
    &shark_object_class,
    sizeof(shark_vm),
    shark_vm_destroy,
    false,
    NULL
};

SHARK_API shark_vm *shark_vm_new()
{
    shark_vm *self = shark_object_new(&shark_vm_class);
    self->import_path = shark_array_new();
    self->max_import_path = 0;
    self->archive_record = shark_table_new();
    self->module_record = shark_table_new();
    self->import_record = shark_table_new();
    self->bottom = NULL;
    self->error = NULL;
    self->stack_size = SHARK_VM_STACK_INIT_SIZE;
    self->TOS = 0;
    self->stack = shark_malloc(self->stack_size * sizeof(shark_value));
    return self;
}

SHARK_API void shark_vm_add_import_path(shark_vm *self, shark_string *import_path)
{
    shark_array_put(self->import_path, SHARK_FROM_PTR(import_path));
    if (import_path->size > self->max_import_path)
        self->max_import_path = import_path->size;
}

SHARK_API shark_module *shark_vm_import_module(shark_vm *self, shark_string *name)
{
    if (shark_table_contains(self->import_record, SHARK_FROM_PTR(name)))
        return SHARK_AS_MODULE(shark_table_get_index(self->import_record, SHARK_FROM_PTR(name)));
    
    if (!shark_table_contains(self->module_record, SHARK_FROM_PTR(name)))
    {
        fprintf(stderr, "missing module '%s', execution aborted.", name->data);
        shark_print_stack_trace(self);
        exit(EXIT_FAILURE);
    }
    
    shark_module *module = SHARK_AS_MODULE(shark_table_get_index(self->module_record, SHARK_FROM_PTR(name)));
    shark_table_set_index(self->import_record, SHARK_FROM_PTR(name), SHARK_FROM_PTR(module));
    shark_vm_exec_module(self, module);
    
    return module;
}

static void shark_vm_grow_stack(shark_vm *self)
{
    size_t new_stack_size = SHARK_VM_STACK_GROW_SIZE(self->stack_size);
    shark_value *new_stack = shark_malloc(new_stack_size * sizeof(shark_value));
    memcpy(new_stack, self->stack, self->stack_size * sizeof(shark_value));
    shark_free(self->stack);
    self->stack_size = new_stack_size;
    self->stack = new_stack;
}

SHARK_API shark_value shark_vm_execute(shark_vm *self, shark_vm_frame frame)
{

#define FETCH           (*(frame.code++))

#define FETCH_SHORT     (((uint16_t) FETCH) \
                        | (((uint16_t) FETCH) << 8))

#define FETCH_INT       (((uint32_t) FETCH) \
                        | (((uint32_t) FETCH) << 8) \
                        | (((uint32_t) FETCH) << 16) \
                        | (((uint32_t) FETCH) << 24))

#define PUSH(value) { \
                    shark_value __PUSH_VALUE__ = value; \
                    self->stack[self->TOS++] = __PUSH_VALUE__; \
                    shark_value_inc_ref(__PUSH_VALUE__); \
                    if (self->TOS == self->stack_size) \
                        shark_vm_grow_stack(self); \
                }

#define POP         self->stack[--self->TOS]

#define CONST   (frame.const_table[FETCH_INT])

#define DEC_REF(x)  shark_object_dec_ref(SHARK_AS_OBJECT(x))
    
    self->bottom = &frame;
    
    shark_class *current_class = NULL;
    shark_array *current_array = NULL;
    shark_table *current_table = NULL;
    
    for (;;)
    {
    	uint8_t inst = FETCH;
        // printf("inst %d\n", inst);
        switch (inst)
        {
        case OP_END:
end:
            for (size_t i = self->TOS; i > frame.base; i--)
                shark_value_dec_ref(POP);
            // TODO: shrink stack
            return SHARK_NULL;
        case OP_NULL:
            PUSH(SHARK_NULL);
            break;
        case OP_TRUE:
            PUSH(SHARK_TRUE);
            break;
        case OP_FALSE:
            PUSH(SHARK_FALSE);
            break;
        case OP_LOAD_GLOBAL:
            PUSH(shark_table_get_index(frame.globals, CONST));
            break;
        case OP_LOAD:
            PUSH(self->stack[frame.base + FETCH_SHORT]);
            break;
        case OP_GET_FIELD: {
            shark_value object = POP;
            if (!SHARK_IS_OBJECT(object)
            || !SHARK_AS_OBJECT(object)->type->is_object_class)
                shark_fatal_error(self, "can't get field of a non-object.");
            PUSH(shark_table_get_index(SHARK_AS_TABLE(object), CONST));
            DEC_REF(object);
            break;
        }
        case OP_ENTER_CLASS: {
            shark_value parent = POP;
            if (!SHARK_IS_NULL(parent))
                if (!SHARK_IS_OBJECT(parent)
                || SHARK_AS_OBJECT(parent)->type != &shark_class_class)
                    shark_fatal_error(self, "can't inherit from non-class object.");
                else if (!SHARK_AS_CLASS(parent)->is_object_class)
                    shark_fatal_error(self, "can't extend a native class.");
            current_class = shark_class_new(SHARK_AS_STR(CONST), SHARK_IS_OBJECT(parent) ? SHARK_AS_CLASS(parent) : NULL);
            DEC_REF(parent);
            break;
        }
        case OP_EXIT_CLASS:
            shark_table_set_index(frame.globals, SHARK_FROM_PTR(current_class->name), SHARK_FROM_PTR(current_class));
            shark_object_dec_ref(current_class);
            current_class = NULL;
            break;
        case OP_DEFINE: {
            shark_value value = POP;
            shark_table_set_index(frame.globals, CONST, value);
            shark_value_dec_ref(value);
            break;
        }
        case OP_DEFINE_FIELD:
            CONST;
            break;
        case OP_FUNCTION: {
            shark_function *function = shark_object_new(&shark_function_class);
            function->arity = (size_t) FETCH_SHORT;
            function->name = shark_object_inc_ref(SHARK_AS_STR(CONST));
            if (current_class != NULL) {
                function->is_method = true;
                function->owner_class = shark_object_inc_ref(current_class);
                function->supermethod = shark_object_inc_ref(SHARK_AS_OBJECT(
                    shark_table_get_index(current_class->methods, SHARK_FROM_PTR(function->name))));
                shark_table_set_index(current_class->methods,
                    SHARK_FROM_PTR(function->name), SHARK_FROM_PTR(function));
            } else {
                function->is_method = false;
                function->owner_class = NULL;
                function->supermethod = NULL;
                shark_table_set_index(frame.module->names,
                    SHARK_FROM_PTR(function->name), SHARK_FROM_PTR(function));
            }
            function->owner = shark_object_inc_ref(frame.module);
            function->type = SHARK_BYTECODE_FUNCTION;
            size_t code_size = (size_t) FETCH_INT;
            function->code.bytecode = frame.code;
            frame.code += code_size;
            shark_object_dec_ref(function);
            break;
        }
        case OP_NOT_IMPLEMENTED:
            shark_fatal_error(self, "function is not implemented.");
            break;
        case OP_EXIT: {
            size_t block_size = (size_t) FETCH_SHORT;
            for (size_t i = 0; i < block_size; i++)
                shark_value_dec_ref(POP);
            break;
        }
        case OP_DUP:
            PUSH(self->stack[self->TOS-1]);
            break;
        case OP_DROP:
            shark_value_dec_ref(POP);
            break;
        case OP_SWAP: {
            shark_value top = self->stack[self->TOS-1];
            self->stack[self->TOS-1] = self->stack[self->TOS-2];
            self->stack[self->TOS-2] = top;
            break;
        }
#define NUM_BINOP(CODE, OP)     case CODE: { \
    shark_value y = POP; \
    shark_value x = POP; \
    if (!SHARK_IS_NUM(x) || !SHARK_IS_NUM(y)) \
        shark_fatal_error(self, "unsupported operand types for " #OP " operator."); \
    PUSH(SHARK_FROM_NUM(SHARK_AS_NUM(x) OP SHARK_AS_NUM(y))); \
    break; \
}
        NUM_BINOP(OP_MUL, *)
        NUM_BINOP(OP_DIV, /)
        case OP_MOD: {
            shark_value y = POP;
            shark_value x = POP;
            if (!SHARK_IS_INT(x) || !SHARK_IS_INT(y))
                shark_fatal_error(self, "unsupported operand types for % operator. (expected two integers)");
            PUSH(SHARK_FROM_INT(SHARK_AS_INT(x) % SHARK_AS_INT(y)));
            break;
        }
        NUM_BINOP(OP_ADD, +)
        NUM_BINOP(OP_SUB, -)
#undef NUM_BINOP
#define COMP_BINOP(CODE, OP)    case CODE: { \
    shark_value y = POP; \
    shark_value x = POP; \
    if (!SHARK_IS_NUM(x) || !SHARK_IS_NUM(y)) \
        shark_fatal_error(self, "unsupported operand types for " #OP " operator."); \
    PUSH(SHARK_FROM_BOOL(SHARK_AS_NUM(x) OP SHARK_AS_NUM(y))); \
    break; \
}
        COMP_BINOP(OP_LT, <)
        COMP_BINOP(OP_LE, <=)
        COMP_BINOP(OP_GT, >)
        COMP_BINOP(OP_GE, >=)
#undef COMP_BINOP
        case OP_EQ: {
            shark_value y = POP;
            shark_value x = POP;
            PUSH(SHARK_FROM_BOOL(shark_value_equals(x, y)));
            shark_value_dec_ref(y);
            shark_value_dec_ref(x);
            break;
        }
        case OP_NE: {
            shark_value y = POP;
            shark_value x = POP;
            PUSH(SHARK_FROM_BOOL(!shark_value_equals(x, y)));
            shark_value_dec_ref(y);
            shark_value_dec_ref(x);
            break;
        }
        case OP_IN: {
            shark_value y = POP;
            shark_value x = POP;
            if (!SHARK_IS_OBJECT(y)
            || SHARK_AS_OBJECT(y)->type != &shark_table_class)
                shark_fatal_error(self, "can't test membership in a non-table value.");
            PUSH(SHARK_FROM_BOOL(shark_table_contains(SHARK_AS_TABLE(y), x)));
            shark_value_dec_ref(x);
            DEC_REF(y);
            break;
        }
        case OP_NOT_IN: {
            shark_value y = POP;
            shark_value x = POP;
            if (!SHARK_IS_OBJECT(y)
            || SHARK_AS_OBJECT(y)->type != &shark_table_class)
                shark_fatal_error(self, "can't test membership in a non-table value.");
            PUSH(SHARK_FROM_BOOL(!shark_table_contains(SHARK_AS_TABLE(y), x)));
            shark_value_dec_ref(x);
            DEC_REF(y);
            break;
        }
        case OP_NEG: {
            shark_value x = POP;
            if (!SHARK_IS_NUM(x))
                shark_fatal_error(self, "unsupported operand for - unary op.");
            PUSH(SHARK_FROM_NUM(-SHARK_AS_NUM(x)));
            break;
        }
        case OP_NOT: {
            shark_value x = POP;
            if (!SHARK_IS_BOOL(x))
                shark_fatal_error(self, "expected a bool value in 'not' negation.");
            PUSH(SHARK_FROM_BOOL(!SHARK_AS_BOOL(x)));
            break;
        }

#define FUNCTION_CALL(callee, argc, self_offset) \
    if (argc != callee->arity) \
        shark_fatal_error(self, "arity mismatch in function call."); \
    shark_module *module = callee->owner; \
    shark_value result; \
    if (callee->type == SHARK_BYTECODE_FUNCTION) { \
        result = shark_vm_execute(self, (shark_vm_frame) { &frame, module, callee, \
            module->names, module->const_table, \
            self->TOS - argc - self_offset, \
            callee->code.bytecode }); \
        if (!self_offset) DEC_REF(POP); \
    } else { \
        shark_vm_frame child = { &frame, module, callee, \
            NULL, NULL, 0, NULL }; \
        self->bottom = &child; \
        result = callee->code.native_code(self, self->stack + self->TOS - argc - self_offset, self->error); \
        for (size_t i = 0; i < argc; i++) \
            shark_value_dec_ref(POP); \
        DEC_REF(POP); \
    } \
    PUSH(result); \
    shark_value_dec_ref(result); \
    self->bottom = &frame; \
    if (self->error->message != NULL) goto end;

        case OP_FUNCTION_CALL: {
            size_t argc = (size_t) FETCH;
            shark_value callee = self->stack[self->TOS - argc - 1];
            if (!SHARK_IS_OBJECT(callee)
            || SHARK_AS_OBJECT(callee)->type != &shark_function_class) {
                shark_fatal_error(self, "can't call a non-function value.");
            }
            // printf("function call %s\n", SHARK_AS_FUNCTION(callee)->name->data);
            FUNCTION_CALL(SHARK_AS_FUNCTION(callee), argc, 0);
            break;
        }
        case OP_METHOD_CALL: {
            // shark_print_stack_trace();
            size_t argc = (size_t) FETCH;
            shark_value object = self->stack[self->TOS - argc - 1];
            if (!SHARK_IS_OBJECT(object) || SHARK_AS_OBJECT(object)->type->methods == NULL)
                shark_fatal_error(self, "invalid method call reciever. (expected an object)");
            shark_function *callee = SHARK_AS_FUNCTION(shark_table_get_index(
                SHARK_AS_OBJECT(object)->type->methods, CONST));
            if (callee == NULL)
                shark_fatal_error(self, "object has no method with that name.");
            // printf("method call %s\n", callee->name->data);
            FUNCTION_CALL(callee, argc, 1);
            break;
        }
        case OP_GET_INDEX: {
            shark_value index = POP;
            shark_value source = POP;
            if (!SHARK_IS_OBJECT(source))
                shark_fatal_error(self, "unsupported operand for indexing.");
            if (SHARK_AS_OBJECT(source)->type == &shark_array_class) {
                if (!SHARK_IS_INT(index))
                    shark_fatal_error(self, "expected an integer as array index.");
                else if (SHARK_AS_INT(index) < 0
                || SHARK_AS_INT(index) >= SHARK_AS_ARRAY(source)->length)
                    shark_fatal_error(self, "array index out of range.");
                PUSH(SHARK_AS_ARRAY(source)->data[SHARK_AS_INT(index)]);
            } else if (SHARK_AS_OBJECT(source)->type == &shark_table_class) {
                PUSH(shark_table_get_index(SHARK_AS_TABLE(source), index));
            } else {
                shark_fatal_error(self, "unsupported operand for indexing (expected array or table).");
            }
            shark_value_dec_ref(index);
            DEC_REF(source);
            break;
        }
        case OP_SELF:
            PUSH(self->stack[frame.base]);
            break;
        case OP_SUPER_CALL: {
            size_t argc = (size_t) FETCH;
            shark_string *name = frame.function->name;
            shark_value object = self->stack[self->TOS - argc - 1];
            shark_function *callee = SHARK_AS_FUNCTION(shark_table_get_index(
                SHARK_AS_OBJECT(object)->type->methods, SHARK_FROM_PTR(name)))->supermethod;
            if (callee == NULL)
                shark_fatal_error(self, "method has no supermethod.");
            FUNCTION_CALL(callee, argc, 1);
            break;
        }
        case OP_SIZEOF: {
            shark_value x = POP;
            if (!SHARK_IS_OBJECT(x))
                shark_fatal_error(self, "can't get the sizeof of a scalar type.");
            if (SHARK_AS_OBJECT(x)->type == &shark_array_class) {
                PUSH(SHARK_FROM_INT(SHARK_AS_ARRAY(x)->length));
            } else if (SHARK_AS_OBJECT(x)->type == &shark_string_class) {
                PUSH(SHARK_FROM_INT(SHARK_AS_STR(x)->size));
            } else if (SHARK_AS_OBJECT(x)->type == &shark_table_class) {
                PUSH(SHARK_FROM_INT(SHARK_AS_TABLE(x)->count));
            } else {
                shark_fatal_error(self, "invalid operand type for sizeof operator.");
            }
            DEC_REF(x);
            break;
        }
        case OP_NEW: {
            size_t argc = (size_t) FETCH;
            shark_value type = self->stack[self->TOS - argc - 1];
            if (!SHARK_IS_OBJECT(type)
            || SHARK_AS_OBJECT(type)->type != &shark_class_class)
                shark_fatal_error(self, "invalid operand for new operator (expected a class).");
            shark_value object;
            if (SHARK_AS_CLASS(type)->is_object_class) {
                object = SHARK_FROM_PTR(shark_object_inc_ref(shark_table_new()));
                SHARK_AS_OBJECT(object)->type = SHARK_AS_CLASS(type);
            } else {
                object = SHARK_FROM_PTR(shark_object_inc_ref(shark_object_new(SHARK_AS_CLASS(type))));
            }
            self->stack[self->TOS - argc - 1] = object;
            DEC_REF(type);
            shark_function *callee = SHARK_AS_FUNCTION(shark_table_get_str(
                SHARK_AS_OBJECT(object)->type->methods, "init"));
            if (callee == NULL)
                shark_fatal_error(self, "can't create instance of this class (no constructor defined).");
            FUNCTION_CALL(callee, argc, 1);
            shark_value_dec_ref(POP);
            PUSH(object);
            DEC_REF(object);
            break;
        }
#undef FUNCTION_CALL
        case OP_INSTANCEOF: {
            shark_value type = POP;
            shark_value value = POP;
            if (!SHARK_IS_OBJECT(type)
            || SHARK_AS_OBJECT(type)->type != &shark_class_class)
                shark_fatal_error(self, "invalid type operand in instanceof operator (expected a class).");
            if (!SHARK_IS_OBJECT(value))
                shark_fatal_error(self, "invalid value operand in instanceof operator (expected an object).");
            PUSH(SHARK_FROM_BOOL(shark_object_instanceof(
                SHARK_AS_OBJECT(value), SHARK_AS_CLASS(type))));
            DEC_REF(type);
            DEC_REF(value);
            break;
        }
        case OP_ARRAY_NEW:
            PUSH(SHARK_FROM_PTR(current_array));
            current_array = shark_array_new();
            break;
        case OP_ARRAY_NEW_APPEND: {
            shark_value x = POP;
            shark_array_put(current_array, x);
            shark_value_dec_ref(x);
            break;
        }
        case OP_TABLE_NEW:
            PUSH(SHARK_FROM_PTR(current_table));
            current_table = shark_table_new();
            break;
        case OP_TABLE_NEW_INSERT: {
            shark_value value = POP;
            shark_value key = POP;
            shark_table_set_index(current_table, key, value);
            shark_value_dec_ref(key);
            shark_value_dec_ref(value);
            break;
        }
        case OP_CONST:
            PUSH(CONST);
            break;
        case OP_RETURN: {
            shark_value result = POP;
			for (size_t i = self->TOS; i > frame.base; i--) {
                shark_value_dec_ref(POP);
            }
            return result;
        }
        case OP_INSERT: {
            shark_value value = POP;
            shark_value index = POP;
            shark_value target = POP;
            if (!SHARK_IS_OBJECT(target)
            || SHARK_AS_OBJECT(target)->type != &shark_array_class)
                shark_fatal_error(self, "invalid insert target (expected an array).");
            if (!SHARK_IS_INT(index))
                shark_fatal_error(self, "invalid array index (expected an integer).");
            size_t int_index = (size_t) SHARK_AS_INT(index);
            shark_array *array_target = SHARK_AS_ARRAY(target);
            if (int_index < 0 || int_index > array_target->length)
                shark_fatal_error(self, "insert index out of range.");
            for (size_t i = array_target->length; i > int_index; i--)
                array_target->data[i] = array_target->data[i - 1];
            array_target->data[int_index] = value;
            array_target->length++;
            shark_array_grow(array_target);
            shark_object_dec_ref(array_target);
            break;
        }
        case OP_APPEND: {
            shark_value value = POP;
            shark_value target = POP;
            if (!SHARK_IS_OBJECT(target)
            || SHARK_AS_OBJECT(target)->type != &shark_array_class)
                shark_fatal_error(self, "invalid append target (expected an array).");
            shark_array_put(SHARK_AS_ARRAY(target), value);
            shark_value_dec_ref(value);
            DEC_REF(target);
            break;
        }
        case OP_STORE_GLOBAL: {
            shark_value value = POP;
            shark_table_set_index(frame.globals, CONST, value);
            shark_value_dec_ref(value);
            break;
        }
        case OP_STORE:
            self->stack[frame.base + FETCH_SHORT] = POP;
            break;
        case OP_SET_STATIC: {
            shark_value value = POP;
            shark_value object = POP;
            if (!SHARK_IS_OBJECT(object)
            || SHARK_AS_OBJECT(object)->type != &shark_module_class)
                shark_fatal_error(self, "can't set static field of non-module object.");
            shark_table_set_index(SHARK_AS_MODULE(object)->names, CONST, value);
            shark_value_dec_ref(value);
            DEC_REF(object);
            break;
        }
        case OP_SET_FIELD: {
            shark_value value = POP;
            shark_value object = POP;
            if (!SHARK_IS_OBJECT(object)
            || !SHARK_AS_OBJECT(object)->type->is_object_class)
                shark_fatal_error(self, "can't set field of non object.");
            shark_table_set_index(SHARK_AS_TABLE(object), CONST, value);
            shark_value_dec_ref(value);
            DEC_REF(object);
            break;
        }
        case OP_SET_INDEX: {
            shark_value value = POP;
            shark_value index = POP;
            shark_value source = POP;
            if (!SHARK_IS_OBJECT(source))
                shark_fatal_error(self, "invalid target for index assignment (expected table or array).");
            if (SHARK_AS_OBJECT(source)->type == &shark_table_class) {
                shark_table_set_index(SHARK_AS_TABLE(source), index, value);
            } else if (SHARK_AS_OBJECT(source)->type == &shark_array_class) {
                if (!SHARK_IS_INT(index))
                    shark_fatal_error(self, "expected an integer as array index.");
                shark_int_t index_int = SHARK_AS_INT(index);
                if (index_int < 0 || index_int >= SHARK_AS_ARRAY(source)->length)
                    shark_fatal_error(self, "array index out of range.");
                SHARK_AS_ARRAY(source)->data[index_int] = shark_value_inc_ref(value);
            } else {
                shark_fatal_error(self, "unsupported target for index assignment (expected array or table).");
            }
            shark_value_dec_ref(value);
            shark_value_dec_ref(index);
            DEC_REF(source);
            break;
        }
        case OP_GET_FIELD_TOP: {
            shark_value object = self->stack[self->TOS-1];
            if (!SHARK_IS_OBJECT(object)
            || !SHARK_AS_OBJECT(object)->type->is_object_class)
                shark_fatal_error(self, "can't get field of a non-object.");
            PUSH(shark_table_get_index(SHARK_AS_TABLE(object), CONST));
            break;
        }
        case OP_GET_INDEX_TOP: {
            shark_value index = self->stack[self->TOS-1];
            shark_value source = self->stack[self->TOS-2];
            if (!SHARK_IS_OBJECT(source))
                shark_fatal_error(self, "unsupported operand for indexing.");
            if (SHARK_AS_OBJECT(source)->type == &shark_array_class) {
                if (!SHARK_IS_INT(index))
                    shark_fatal_error(self, "expected an integer as array index.");
                else if (SHARK_AS_INT(index) < 0
                || SHARK_AS_INT(index) >= SHARK_AS_ARRAY(source)->length)
                    shark_fatal_error(self, "array index out of range.");
                PUSH(SHARK_AS_ARRAY(source)->data[SHARK_AS_INT(index)]);
            } else if (SHARK_AS_OBJECT(source)->type == &shark_table_class) {
                PUSH(shark_table_get_index(SHARK_AS_TABLE(source), index));
            } else {
                shark_fatal_error(self, "unsupported target for indexing (expected array or table).");
            }
            break;
        }
        case OP_GET_STATIC: {
            shark_value object = POP;
            if (!SHARK_IS_OBJECT(object)
            || SHARK_AS_OBJECT(object)->type != &shark_module_class)
                shark_fatal_error(self, "can't get static field of a non-module object.");
            PUSH(shark_table_get_index(SHARK_AS_MODULE(object)->names, CONST));
            DEC_REF(object);
            break;
        }
        case OP_GET_STATIC_TOP: {
            shark_value object = self->stack[self->TOS-1];
            if (!SHARK_IS_OBJECT(object)
            || SHARK_AS_OBJECT(object)->type != &shark_module_class)
                shark_fatal_error(self, "can't get static field of a non-module object.");
            PUSH(shark_table_get_index(SHARK_AS_MODULE(object)->names, CONST));
            break;
        }
#define GET_OFFSET      (((uint16_t) frame.code[0]) + (((uint16_t) frame.code[1]) << 8))
        case OP_IF: {
            shark_value value = POP;
            if (SHARK_IS_BOOL(value) && SHARK_AS_BOOL(value) == 1) {
                FETCH_SHORT;
            } else {
                frame.code += GET_OFFSET;
            }
            break;
        }
        case OP_JUMP:
            frame.code += GET_OFFSET;
            break;
        case OP_LOOP:
            frame.code -= GET_OFFSET;
            break;
        case OP_ZERO:
            PUSH(SHARK_FROM_NUM(0));
            break;
        case OP_INC: {
            uint16_t local = FETCH_SHORT;
            shark_value value = self->stack[frame.base + local];
            if (!SHARK_IS_NUM(value))
                shark_fatal_error(self, "can't increment a non numeric value.");
            self->stack[frame.base + local] = SHARK_FROM_NUM(SHARK_AS_NUM(value) + 1);
            break;
        }
        case OP_OR: {
            shark_value value = self->stack[self->TOS-1];
            if (SHARK_IS_NULL(value)
            || (SHARK_IS_BOOL(value) && SHARK_AS_BOOL(value) == 0)) {
                POP;
                FETCH_SHORT;
            } else {
                frame.code += GET_OFFSET;
            }
            break;
        }
        case OP_AND: {
            shark_value value = self->stack[self->TOS-1];
            if ((SHARK_IS_BOOL(value) && SHARK_AS_BOOL(value) == 1)
            || (!SHARK_IS_NULL(value) && !SHARK_IS_BOOL(value))) {
                shark_value_dec_ref(POP);
                FETCH_SHORT;
            } else {
                frame.code += GET_OFFSET;
            }
            break;
        }
#undef GET_OFFSET
#define AU_BINOP(X, Y, OP, RESULT)  { \
    if (!SHARK_IS_NUM(X) || !SHARK_IS_NUM(Y)) \
        shark_fatal_error(self, "unsupported operand types for numeric operator."); \
    switch (OP) \
    { \
        case OP_ADD: RESULT = SHARK_FROM_NUM(SHARK_AS_NUM(X) + SHARK_AS_NUM(Y)); break; \
        case OP_SUB: RESULT = SHARK_FROM_NUM(SHARK_AS_NUM(X) - SHARK_AS_NUM(Y)); break; \
        case OP_MUL: RESULT = SHARK_FROM_NUM(SHARK_AS_NUM(X) * SHARK_AS_NUM(Y)); break; \
        case OP_DIV: RESULT = SHARK_FROM_NUM(SHARK_AS_NUM(X) / SHARK_AS_NUM(Y)); break; \
        case OP_MOD: RESULT = SHARK_FROM_INT(SHARK_AS_INT(X) % SHARK_AS_INT(Y)); break; \
        default: RESULT = SHARK_NULL; break; \
    } \
}
        case OP_SET_INDEX_AU: {
            shark_value z = POP;
            shark_value y = POP;
            shark_value x = POP;
            uint8_t op = FETCH;
            shark_value result;
            if (!SHARK_IS_OBJECT(x))
                shark_fatal_error(self, "invalid target for index assignment (expected table or array).");
            if (SHARK_AS_OBJECT(x)->type == &shark_table_class) {
                AU_BINOP(shark_table_get_index(SHARK_AS_TABLE(x), y), z, op, result);
                shark_table_set_index(SHARK_AS_TABLE(x), y, result);
                shark_value_dec_ref(y);
            } else if (SHARK_AS_OBJECT(x)->type == &shark_array_class) {
                if (!SHARK_IS_INT(y))
                    shark_fatal_error(self, "expected an integer as array index.");
                shark_int_t index_int = SHARK_AS_INT(y);
                if (index_int < 0 || index_int >= SHARK_AS_ARRAY(x)->length)
                    shark_fatal_error(self, "array index out of range.");
                AU_BINOP(SHARK_AS_ARRAY(x)->data[index_int], z, op, result);
                SHARK_AS_ARRAY(x)->data[index_int] = result;
            } else {
                shark_fatal_error(self, "unsupported target for index assignment (expected array or table).");
            }
            DEC_REF(x);
            break;
        }
        case OP_SET_FIELD_AU: {
            shark_value y = POP;
            shark_value x = POP;
            if (!SHARK_IS_OBJECT(x)
            || !SHARK_AS_OBJECT(x)->type->is_object_class)
                shark_fatal_error(self, "can't set field of non object.");
            uint8_t op = FETCH;
            shark_value field = CONST;
            shark_value result;
            AU_BINOP(shark_table_get_index(SHARK_AS_TABLE(x), field), y, op, result);
            shark_table_set_index(SHARK_AS_TABLE(x), field, result);
            DEC_REF(x);
            break;
        }
        case OP_SET_STATIC_AU: {
            shark_value y = POP;
            shark_value x = POP;
            if (!SHARK_IS_OBJECT(x)
            || SHARK_AS_OBJECT(x)->type != &shark_module_class)
                shark_fatal_error(self, "can't set static field of non-module object.");
            uint8_t op = FETCH;
            shark_value field = CONST;
            shark_value result;
            AU_BINOP(shark_table_get_index(SHARK_AS_MODULE(x)->names, field), y, op, result);
            shark_table_set_index(SHARK_AS_MODULE(x)->names, field, result);
            DEC_REF(x);
            break;
        }
#undef AU_BINOP
        case OP_ARRAY_CLOSE: {
            shark_array *current = SHARK_AS_ARRAY(POP);
            shark_object_dec_ref(current);
            PUSH(SHARK_FROM_PTR(current_array));
            shark_object_dec_ref(current_array);
            current_array = current;
            break;
        }
        case OP_TABLE_CLOSE: {
            shark_table *current = SHARK_AS_TABLE(POP);
            shark_object_dec_ref(current);
            PUSH(SHARK_FROM_PTR(current_table));
            shark_object_dec_ref(current_table);
            current_table = current;
            break;
        }
        default:
            fprintf(stderr, "usuported operation: %d\n", *(--frame.code));
            shark_fatal_error(self, "");
            break;
        }
    }
#undef FETCH
#undef FETCH_SHORT
#undef FETCH_INT
#undef PUSH
#undef POP
#undef EXIT
#undef CONST
#undef DEC_REF
}

SHARK_API void shark_vm_exec_module(shark_vm *self, shark_module *module)
{
    for (size_t i = 0; i < module->import_table->length; i += 2)
    {
        shark_string *name = SHARK_AS_STR(module->import_table->data[i]);
        shark_module *imported = shark_vm_import_module(self, name);
        shark_object *mode = SHARK_AS_OBJECT(module->import_table->data[i + 1]);
        if (mode->type == &shark_string_class) {
            shark_table_set_index(module->names, SHARK_FROM_PTR(mode), SHARK_FROM_PTR(imported));
        } else {
            shark_array *target = (shark_array *) mode;
            for (size_t j = 0; j < target->length; j++) {
                shark_value imported_value = shark_table_get_index(imported->names, target->data[j]);
                shark_table_set_index(module->names, target->data[j], imported_value);
            }
        }
    }
    
    shark_error error;
    error.protect = false;
    error.message = NULL;
    self->error = &error;
    
    shark_vm_frame frame;
    frame.parent = NULL;
    frame.module = module;
    frame.function = NULL;
    frame.globals = module->names;
    frame.const_table = module->const_table;
    frame.base = self->TOS;
    frame.code = module->code;
    
    shark_vm_execute(self, frame);
}

SHARK_API shark_value shark_vm_exec_main(shark_vm *self, shark_module *module, shark_array *args)
{
	shark_string *main_name = shark_string_new_from_cstr("main");
    shark_value main_value = shark_table_get_index(module->names, SHARK_FROM_PTR(main_name));
    shark_object_dec_ref(main_name);
    if (SHARK_IS_NULL(main_value))
        return SHARK_NULL;
    else if (!SHARK_IS_OBJECT(main_value)
    || SHARK_AS_OBJECT(main_value)->type != &shark_function_class)
        shark_fatal_error(self, "expected symbol main to be a function.");
    else if (SHARK_AS_FUNCTION(main_value)->arity != 1)
        shark_fatal_error(self, "expected main function to take exactly one argument.");
    
    shark_error error;
    error.protect = false;
    error.message = NULL;
    self->error = &error;
    
    shark_function *main = SHARK_AS_FUNCTION(main_value);
    shark_vm_frame frame;
    frame.parent = NULL;
    frame.module = main->owner;
    frame.function = main;
    frame.globals = main->owner->names;
    frame.const_table = main->owner->const_table;
    frame.base = self->TOS;
    frame.code = main->code.bytecode;

    self->stack[self->TOS++] = SHARK_FROM_PTR(args);
    if (self->TOS == self->stack_size)
        shark_vm_grow_stack(self);

    shark_vm_execute(self, frame);
}

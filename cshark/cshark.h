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

#if defined(CSHARK_BYTECODE) && !defined(__CSHARK_BYTECODE)
#define __CSHARK_BYTECODE
typedef enum {
    OP_END = 0,
    OP_NULL = 1,
    OP_TRUE = 2,
    OP_FALSE = 3,
    OP_LOAD_GLOBAL = 4,
    OP_LOAD = 5,
    OP_GET_FIELD = 6,
    OP_ENTER_CLASS = 7,
    OP_EXIT_CLASS = 8,
    OP_DEFINE = 9,
    OP_DEFINE_FIELD = 10,
    OP_FUNCTION = 11,
    OP_NOT_IMPLEMENTED = 12,
    OP_EXIT = 13,
    OP_DUP = 14,
    OP_DROP = 15,
    OP_SWAP = 16,
    OP_MUL = 17,
    OP_DIV = 18,
    OP_MOD = 19,
    OP_ADD = 20,
    OP_SUB = 21,
    OP_LT = 22,
    OP_LE = 23,
    OP_GT = 24,
    OP_GE = 25,
    OP_EQ = 26,
    OP_NE = 27,
    OP_IN = 28,
    OP_NOT_IN = 29,
    OP_NEG = 30,
    OP_NOT = 31,
    OP_FUNCTION_CALL = 32,
    OP_METHOD_CALL = 33,
    OP_GET_SLICE = 34,
    OP_GET_INDEX = 35,
    OP_SELF = 36,
    OP_SUPER_CALL = 37,
    OP_SIZEOF = 38,
    OP_NEW = 39,
    OP_INSTANCEOF = 40,
    OP_ARRAY_NEW = 41,
    OP_ARRAY_NEW_APPEND = 42,
    OP_TABLE_NEW = 43,
    OP_TABLE_NEW_INSERT = 44,
    OP_CONST = 45,
    OP_RETURN = 46,
    OP_INSERT = 47,
    OP_APPEND = 48,
    OP_STORE_GLOBAL = 49,
    OP_STORE = 50,
    OP_SET_STATIC = 51,
    OP_SET_FIELD = 52,
    OP_SET_SLICE = 53,
    OP_SET_INDEX = 54,
    OP_GET_FIELD_TOP = 55,
    OP_GET_INDEX_TOP = 56,
    OP_GET_STATIC = 57,
    OP_GET_STATIC_TOP = 58,
    OP_IF = 59,
    OP_JUMP = 60,
    OP_LOOP = 61,
    OP_ZERO = 62,
    OP_INC = 63,
	OP_OR = 64,
	OP_AND = 65,
	OP_SET_INDEX_AU = 66,
	OP_SET_FIELD_AU = 67,
	OP_SET_STATIC_AU = 68,
    OP_ARRAY_CLOSE = 69,
    OP_TABLE_CLOSE = 70
} shark_opcode;

typedef enum {
    CONST_INT = 0,
    CONST_FLOAT,
    CONST_CHAR,
    CONST_STR,
    CONST_SYMBOL
} shark_const_type;
#endif // __CSHARK_BYTECODE

#ifndef __CSHARK_INCLUDE__
#define __CSHARK_INCLUDE__

#ifndef SHARK_API
    #define SHARK_API
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef bool        shark_bool_t;
typedef int64_t     shark_int_t;
typedef double      shark_num_t;
typedef uint8_t     shark_char_t;

#ifdef SHARK_UNBOX
typedef enum {
    SHARK_TYPE_VOID = 0,
    SHARK_TYPE_BOOL,
    SHARK_TYPE_NUM,
    SHARK_TYPE_CHAR,
    SHARK_TYPE_OBJECT
} shark_type;

typedef struct {
    shark_type type;
    union {
        shark_bool_t    BOOL;
        shark_int_t     INT;
        shark_num_t     NUM;
        shark_char_t    CHAR;
        void           *OBJECT;
    } as;
} shark_value;

#define SHARK_NULL  ((shark_value) { SHARK_TYPE_VOID, { .INT = 0 }})
#define SHARK_TRUE  ((shark_value) { SHARK_TYPE_BOOL, { .BOOL = true }})
#define SHARK_FALSE ((shark_value) { SHARK_TYPE_BOOL, { .BOOL = false }})

#define SHARK_AS_BOOL(x)        (x.as.BOOL)
#define SHARK_AS_INT(x)         ((shark_int_t) x.as.NUM)
#define SHARK_AS_NUM(x)         (x.as.NUM)
#define SHARK_AS_CHAR(x)        (x.as.CHAR)
#define SHARK_AS_PTR(x)         (x.as.OBJECT)

#define SHARK_IS_NULL(x)        (x.type == SHARK_TYPE_VOID)
#define SHARK_IS_BOOL(x)        (x.type == SHARK_TYPE_BOOL)
#define SHARK_IS_INT(x)         (x.type == SHARK_TYPE_NUM \
                                && ((shark_num_t) (shark_int_t) x.as.NUM) == x.as.NUM)
#define SHARK_IS_NUM(x)         (x.type == SHARK_TYPE_NUM)
#define SHARK_IS_CHAR(x)        (x.type == SHARK_TYPE_CHAR)
#define SHARK_IS_OBJECT(x)      (x.type == SHARK_TYPE_OBJECT)

#define SHARK_FROM_BOOL(x)  ((shark_value) { SHARK_TYPE_BOOL, { .BOOL = x }})
#define SHARK_FROM_INT(x)   ((shark_value) { SHARK_TYPE_NUM, { .NUM = (shark_num_t) (x) }})
#define SHARK_FROM_NUM(x)   ((shark_value) { SHARK_TYPE_NUM, { .NUM = x }})
#define SHARK_FROM_CHAR(x)  ((shark_value) { SHARK_TYPE_CHAR, { .CHAR = x }})
#define SHARK_FROM_PTR(x)   ((shark_value) { SHARK_TYPE_OBJECT, { .OBJECT = x }})
#else
typedef union {
    uint64_t INT;
    double NUM;
} shark_value;

#define SHARK_OBJECT_MASK   ((uint64_t) 0x7FFC000000000000)

#define SHARK_NULL  ((shark_value) { .NUM = 0 })
#define SHARK_TRUE  ((shark_value) { .NUM = 1 })
#define SHARK_FALSE ((shark_value) { .NUM = 0 })

#define SHARK_AS_BOOL(x)        ((x).NUM)
#define SHARK_AS_INT(x)         ((shark_int_t) ((x).NUM))
#define SHARK_AS_NUM(x)         (x.NUM)
#define SHARK_AS_CHAR(x)        ((shark_char_t) x.NUM)
#define SHARK_AS_PTR(x)         ((void *) (x.INT & ~SHARK_OBJECT_MASK))

#define SHARK_IS_NULL(x)        ((x).NUM == 0)
#define SHARK_IS_BOOL(x)        (SHARK_IS_NUM(x))
#define SHARK_IS_INT(x)         (SHARK_IS_NUM(x) \
                                && ((shark_num_t) (shark_int_t) x.NUM) == x.NUM)
#define SHARK_IS_NUM(x)         (((x).INT & SHARK_OBJECT_MASK) != SHARK_OBJECT_MASK)
#define SHARK_IS_CHAR(x)        (SHARK_IS_NUM(x))
#define SHARK_IS_OBJECT(x)      (((x).INT & SHARK_OBJECT_MASK) == SHARK_OBJECT_MASK)

#define SHARK_FROM_BOOL(x)  ((shark_value) { .NUM = (double) (x) })
#define SHARK_FROM_INT(x)   ((shark_value) { .NUM = (double) (x) })
#define SHARK_FROM_NUM(x)   ((shark_value) { .NUM = (x) })
#define SHARK_FROM_CHAR(x)  ((shark_value) { .NUM = (double) (x) })
#define SHARK_FROM_PTR(x)   ((shark_value) { .INT = ((uint64_t) (x)) | SHARK_OBJECT_MASK })
#endif

#define SHARK_AS_OBJECT(x)      ((shark_object *) SHARK_AS_PTR(x))
#define SHARK_AS_STR(x)         ((shark_string *) SHARK_AS_PTR(x))
#define SHARK_AS_ARRAY(x)       ((shark_array *) SHARK_AS_PTR(x))
#define SHARK_AS_TABLE(x)       ((shark_table *) SHARK_AS_PTR(x))
#define SHARK_AS_FUNCTION(x)    ((shark_function *) SHARK_AS_PTR(x))
#define SHARK_AS_CLASS(x)       ((shark_class *) SHARK_AS_PTR(x))
#define SHARK_AS_MODULE(x)      ((shark_module *) SHARK_AS_PTR(x))

typedef struct _shark_object shark_object;
typedef struct shark_string shark_string;

typedef struct {
    shark_bool_t protect;
    shark_string *message;
} shark_error;

SHARK_API void shark_fatal_error(void *vm, char *message);

SHARK_API void shark_memory_error();
SHARK_API void *shark_malloc(size_t object_size);
SHARK_API void *shark_realloc(void *object, size_t new_size);
SHARK_API void *shark_calloc(size_t count, size_t object_size);

#define shark_zalloc(object_size)   shark_calloc(1, object_size)
#define shark_free(object)          free(object)

typedef struct _shark_class shark_class;

struct _shark_object
{
    shark_class *type;
    size_t ref_count;
};

typedef struct shark_table shark_table;

struct _shark_class
{
    shark_object super;
    shark_string *name;
    char *cname;
    shark_class *parent;
    size_t object_size;
    void (*destroy)(shark_object *);
    bool is_object_class;
    shark_table *methods;
};

SHARK_API void *shark_object_new(shark_class *type);
SHARK_API void shark_object_delete(void *object);
SHARK_API void shark_default_destroy(shark_object *self);

SHARK_API void *shark_object_inc_ref(void *self);
SHARK_API void shark_object_dec_ref(void *self);

SHARK_API bool shark_object_instanceof(shark_object *self, shark_class *type);

SHARK_API shark_value shark_value_inc_ref(shark_value self);
SHARK_API void shark_value_dec_ref(shark_value self);

SHARK_API bool shark_object_is_str(shark_object *object);
SHARK_API bool shark_object_is_array(shark_object *object);
SHARK_API bool shark_object_is_table(shark_object *object);
SHARK_API bool shark_object_is_function(shark_object *object);
SHARK_API bool shark_object_is_module(shark_object *object);

#define SHARK_ASSERT_TYPE(vm, comp, type, at) \
    do \
    if (!(comp)) { \
        fprintf(stderr, "type error (%s expected) at %s.", type, at); \
        shark_fatal_error(vm, ""); \
    } \
    while (false)

#define SHARK_ASSERT_NULL(value, vm, at)        SHARK_ASSERT_TYPE(vm, SHARK_IS_NULL(value), "null", at)
#define SHARK_ASSERT_BOOL(value, vm, at)        SHARK_ASSERT_TYPE(vm, SHARK_IS_BOOL(value), "bool", at)
#define SHARK_ASSERT_INT(value, vm, at)         SHARK_ASSERT_TYPE(vm, SHARK_IS_INT(value), "int", at)
#define SHARK_ASSERT_NUM(value, vm, at)         SHARK_ASSERT_TYPE(vm, SHARK_IS_NUM(value), "number", at)
#define SHARK_ASSERT_CHAR(value, vm, at)        SHARK_ASSERT_TYPE(vm, SHARK_IS_CHAR(value), "char", at)
#define SHARK_ASSERT_OBJECT(value, vm, at)      SHARK_ASSERT_TYPE(vm, SHARK_IS_OBJECT(value), "object", at)
#define SHARK_ASSERT_STR(value, vm, at)         SHARK_ASSERT_TYPE(vm, SHARK_IS_OBJECT(value) && shark_object_is_str(SHARK_AS_OBJECT(value)), "str", at)
#define SHARK_ASSERT_ARRAY(value, vm, at)       SHARK_ASSERT_TYPE(vm, SHARK_IS_OBJECT(value) && shark_object_is_array(SHARK_AS_OBJECT(value)), "array", at)
#define SHARK_ASSERT_TABLE(value, vm, at)       SHARK_ASSERT_TYPE(vm, SHARK_IS_OBJECT(value) && shark_object_is_table(SHARK_AS_OBJECT(value)), "table", at)
#define SHARK_ASSERT_FUNCTION(value, vm, at)    SHARK_ASSERT_TYPE(vm, SHARK_IS_OBJECT(value) && shark_object_is_function(SHARK_AS_OBJECT(value)), "function", at)
#define SHARK_ASSERT_CLASS(value, vm, at)       SHARK_ASSERT_TYPE(vm, SHARK_IS_OBJECT(value) && shark_object_is_class(SHARK_AS_OBJECT(value)), "class", at)
#define SHARK_ASSERT_MODULE(value, vm, at)      SHARK_ASSERT_TYPE(vm, SHARK_IS_OBJECT(value) && shark_object_is_module(SHARK_AS_OBJECT(value)), "module", at)

SHARK_API char *shark_class_get_name(shark_class *type);
SHARK_API shark_class *shark_class_new(shark_string *name, shark_class *parent);
SHARK_API shark_bool_t shark_object_is_class(shark_object *object);

struct shark_string
{
    shark_object super;
    size_t size;
    size_t hash;
    uint8_t *data;
};

SHARK_API size_t shark_hash_byte_str(size_t length, uint8_t *data);

SHARK_API void shark_string_init(shark_string *self);

SHARK_API void *shark_string_new_with_size(size_t size);
SHARK_API void *shark_string_new_from_byte_str(size_t size, uint8_t *data);
SHARK_API void *shark_string_new_from_cstr(char *data);

SHARK_API bool shark_string_equals(shark_string *self, shark_string *other);

typedef struct {
    shark_object super;
    size_t length;
    size_t size;
    shark_value *data;
} shark_array;

#define SHARK_ARRAY_INIT_SIZE       2
#define SHARK_ARRAY_GROW_SIZE(x)    (x << 1)
#define SHARK_ARRAY_SHRINK_SIZE(x)  (x >> 1)

SHARK_API void *shark_array_new();
SHARK_API void shark_array_preallocate(shark_array *self, size_t size);
SHARK_API void shark_array_grow(shark_array *self);
SHARK_API void shark_array_put(shark_array *self, shark_value value);
SHARK_API void shark_array_shrink(shark_array *self);
SHARK_API shark_value shark_array_pop(shark_array *self);

typedef struct {
    size_t hash;
    shark_value key;
    shark_value value;
} shark_table_slot;

struct shark_table
{
    shark_object super;
    size_t count;
    size_t size;
    size_t mask;
    shark_table_slot *data;
};

#define SHARK_TABLE_INIT_SIZE       4

#define SHARK_TABLE_HASH_NULL       0
#define SHARK_TABLE_HASH_NOT_NULL   1

SHARK_API size_t shark_value_hash(shark_value x);
SHARK_API bool shark_value_equals(shark_value x, shark_value y);

SHARK_API shark_table *shark_table_new();
SHARK_API shark_table *shark_table_copy(shark_table *self);

SHARK_API shark_value shark_table_get_index(shark_table *self, shark_value key);
SHARK_API shark_value shark_table_get_str(shark_table *self, char *value);

SHARK_API void shark_table_set_index(shark_table *self, shark_value key, shark_value value);

SHARK_API shark_bool_t shark_table_contains(shark_table *self, shark_value key);

SHARK_API void shark_table_delete_index(shark_table *self, shark_value key);
SHARK_API shark_value shark_table_pop_index(shark_table *self, shark_value key);

SHARK_API void shark_table_update(shark_table *self, shark_table *other);

typedef struct {
    shark_object super;
    shark_string *name;
    shark_array *import_table;
    shark_table *names;
    size_t const_table_size;
    shark_value *const_table;
    uint8_t *code;
} shark_module;

SHARK_API shark_string *shark_path_get_base(shark_string *path);
SHARK_API shark_string *shark_path_get_tail(shark_string *path);
SHARK_API shark_string *shark_path_get_ext(shark_string *path);
SHARK_API shark_string *shark_path_remove_ext(shark_string *path);
SHARK_API shark_string *shark_path_join(shark_string *x, shark_string *y);

typedef enum {
    SHARK_NATIVE_FUNCTION,
    SHARK_BYTECODE_FUNCTION
} shark_function_type;

typedef struct shark_vm shark_vm;

typedef shark_value (*shark_native_function)(shark_vm *, shark_value *, shark_error *);

typedef struct shark_function shark_function;

struct shark_function
{
    shark_object super;
    shark_string *name;
    size_t arity;
    shark_module *owner;
    bool is_method;
    shark_class *owner_class;
    shark_function *supermethod;
    shark_function_type type;
    union {
        shark_native_function native_code;
        uint8_t *bytecode;
    } code;
};

typedef struct shark_vm_frame shark_vm_frame;

struct shark_vm_frame
{
    shark_vm_frame *parent;
    shark_module *module;
    shark_function *function;
    shark_table *globals;
    shark_value *const_table;
    size_t base;
    uint8_t *code;
};

struct shark_vm
{
    shark_object super;
    shark_array *import_path;
    size_t max_import_path;
    shark_table *archive_record;
    shark_table *module_record;
    shark_table *import_record;
    shark_vm_frame *bottom;
    shark_error *error;
    size_t stack_size;
    size_t TOS;
    shark_value *stack;
};

SHARK_API void shark_print_stack_trace(shark_vm *vm);
SHARK_API shark_module *shark_read_archive(shark_vm *vm, shark_string *name, void *source_file);

#define SHARK_VM_STACK_INIT_SIZE    32
#define SHARK_VM_STACK_GROW_SIZE(x) (x << 1)

SHARK_API shark_vm *shark_vm_new();
SHARK_API void shark_vm_add_import_path(shark_vm *self, shark_string *import_path);
SHARK_API void shark_vm_exec_module(shark_vm *self, shark_module *module);
SHARK_API shark_module *shark_vm_import_module(shark_vm *self, shark_string *name);
SHARK_API shark_value shark_vm_execute(shark_vm *self, shark_vm_frame frame);
SHARK_API void shark_vm_exec_module(shark_vm *self, shark_module *module);
SHARK_API shark_value shark_vm_exec_main(shark_vm *self, shark_module *module, shark_array *args);

SHARK_API shark_int_t shark_get_err();
SHARK_API shark_bool_t shark_has_err();
SHARK_API void shark_set_err(shark_int_t value);
SHARK_API void shark_clear_err();

SHARK_API shark_string *shark_string_format(shark_string *format, shark_array *args);

SHARK_API void shark_init_library(shark_vm *vm, shark_string *library_path);

#endif  // __CSHARK_INCLUDE__

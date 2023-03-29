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

#include "cshark.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#ifndef CSHARK_NO_FS
    #ifdef _WIN32
        #include <windows.h>
        #ifdef BOOL
            #undef BOOL
        #endif
        #ifdef CONST
            #undef CONST
        #endif
        #include <sys/stat.h>
        #include <direct.h>
        #include <io.h>
    #else
        #include <sys/types.h>
        #include <sys/stat.h>
        #include <unistd.h>
        #include <dirent.h>
    #endif
#endif

#define SHARK_NATIVE(name)     static shark_value shark_lib_ ## name(shark_vm *vm, shark_value *args, shark_error *error)

SHARK_NATIVE(exit)
{
    SHARK_ASSERT_INT(args[0], vm, "argument 1 of 'exit'");
    exit((int) SHARK_AS_INT(args[0]));
}

SHARK_NATIVE(clock)
{
    return SHARK_FROM_NUM(clock() / (double) CLOCKS_PER_SEC);
}

static shark_int_t shark_errno = 0;

SHARK_API shark_int_t shark_get_err() {
    return shark_errno;
}

SHARK_API shark_bool_t shark_has_err() {
    return shark_errno != 0;
}

SHARK_API void shark_set_err(shark_int_t value) {
    shark_errno = value;
}

SHARK_API void shark_clear_err() {
    shark_errno = 0;
}

SHARK_NATIVE(get_err)
{
    return SHARK_FROM_INT(shark_errno);
}

SHARK_NATIVE(has_err)
{
    return SHARK_FROM_BOOL(shark_errno != 0);
}

SHARK_NATIVE(set_err)
{
    SHARK_ASSERT_INT(args[0], vm, "argument 1 of 'set_err'");
    shark_errno = SHARK_AS_INT(args[0]);
    return SHARK_NULL;
}

SHARK_NATIVE(clear_err)
{
    shark_errno = 0;
    return SHARK_NULL;
}

SHARK_NATIVE(error)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'error'");
    shark_string *message = SHARK_AS_STR(args[0]);
    if (error->protect)
        error->message = message;
    else
        shark_fatal_error(vm, message->data);
    return SHARK_NULL;
}

SHARK_NATIVE(pcall)
{
    SHARK_ASSERT_FUNCTION(args[0], vm, "argument 1 of 'pcall'");
    SHARK_ASSERT_ARRAY(args[1], vm, "argument 2 of 'pcall'");
    
    shark_function *callee = SHARK_AS_FUNCTION(args[0]);
    shark_array *argv = SHARK_AS_ARRAY(args[1]);
    
    if (argv->length != callee->arity)
        shark_fatal_error(vm, "arity mismatch in function call.");
    
    shark_module *module = callee->owner;
    shark_value result;
    
    shark_error protect_error;
    protect_error.protect = true;
    protect_error.message = NULL;
    
    shark_error *prev_error = vm->error;
    vm->error = &protect_error;
    
    for (size_t i = 0; i < argv->length; i++)
    {
        vm->stack[vm->TOS++] = argv->data[i];
        shark_value_inc_ref(argv->data[i]);
        if (vm->TOS == vm->stack_size) shark_vm_grow_stack(vm);
    }
    
    if (callee->type == SHARK_BYTECODE_FUNCTION) {
        result = shark_vm_execute(vm, (shark_vm_frame) {
            vm->bottom,
            module, callee,
            module->names, module->const_table,
            vm->TOS - argv->length,
            callee->code.bytecode });
    } else {
        shark_vm_frame child = { vm->bottom, module, callee, NULL, NULL, 0, NULL };
        vm->bottom = &child;
        result = callee->code.native_code(vm, vm->stack + vm->TOS - argv->length, vm->error);
        for (size_t i = 0; i < argv->length; i++)
            shark_value_dec_ref(vm->stack[--vm->TOS]);
    }
    
    vm->bottom = vm->bottom->parent;
    vm->error = prev_error;
    
    if (protect_error.message != NULL)
    {
        shark_set_err(1);
        return SHARK_FROM_PTR(protect_error.message);
    }
    
    return result;
}

SHARK_NATIVE(path_get_base)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'get_base'");
    return SHARK_FROM_PTR(shark_path_get_base(SHARK_AS_STR(args[0])));
}

SHARK_NATIVE(path_get_tail)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'get_tail'");
    return SHARK_FROM_PTR(shark_path_get_tail(SHARK_AS_STR(args[0])));
}

SHARK_NATIVE(path_get_ext)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'get_ext'");
    return SHARK_FROM_PTR(shark_path_get_ext(SHARK_AS_STR(args[0])));
}

SHARK_NATIVE(path_remove_ext)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'remove_ext'");
    return SHARK_FROM_PTR(shark_path_remove_ext(SHARK_AS_STR(args[0])));
}

SHARK_NATIVE(path_join)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'join'");
    SHARK_ASSERT_STR(args[1], vm, "argument 2 of 'join'");
    return SHARK_FROM_PTR(shark_path_join(SHARK_AS_STR(args[0]), SHARK_AS_STR(args[1])));
}

#ifndef CSHARK_NO_FS
SHARK_NATIVE(listdir)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'listdir'");
    shark_string *path = SHARK_AS_STR(args[0]);
    shark_array *list = shark_array_new();
#ifdef _WIN32
    shark_string *tail = shark_string_new_from_cstr("*");
    shark_string *pattern = shark_path_join(path, tail);
    shark_object_dec_ref(tail);
    WIN32_FIND_DATA find_data;
    HANDLE find_handle;
    
    if ((find_handle = FindFirstFileA(path->data, &find_data)) == INVALID_HANDLE_VALUE)
        return SHARK_NULL;
    do {
        shark_string *filename = shark_string_new_from_cstr(find_data.cFileName);
        shark_array_put(list, SHARK_FROM_PTR(filename));
        shark_object_dec_ref(filename);
    } while (FindNextFile(find_handle, &find_data) != 0);
    FindClose(find_handle);
    shark_object_dec_ref(pattern);
#else
    struct dirent *entry;
    DIR *dir = opendir(path->data);
    
    if (dir == NULL) return SHARK_NULL;
    
    while ((entry = readdir(dir)) != NULL)
    {
        shark_string *filename = shark_string_new_from_cstr(entry->d_name);
        shark_array_put(list, SHARK_FROM_PTR(filename));
        shark_object_dec_ref(filename);
    }
    
    closedir(dir);
#endif
    
    return SHARK_FROM_PTR(list);
}

SHARK_NATIVE(mkdir)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'mkdir'");
#ifdef _WIN32
    return mkdir(SHARK_AS_STR(args[0])->data) ? SHARK_FALSE : SHARK_TRUE;
#else
    return mkdir(SHARK_AS_STR(args[0])->data, S_IRWXU) ? SHARK_FALSE : SHARK_TRUE;
#endif
}

SHARK_NATIVE(rmdir)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'rmdir'");
    return rmdir(SHARK_AS_STR(args[0])->data) ? SHARK_FALSE : SHARK_TRUE;
}

SHARK_NATIVE(unlink)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'unlink'");
    return unlink(SHARK_AS_STR(args[0])->data) ? SHARK_FALSE : SHARK_TRUE;
}
#endif // CSHARK_NO_FS

SHARK_NATIVE(abs)
{
    SHARK_ASSERT_NUM(args[0], vm, "argument 1 of 'abs'");
    return SHARK_FROM_NUM(fabs(SHARK_AS_NUM(args[0])));
}

SHARK_NATIVE(acos)
{
    SHARK_ASSERT_NUM(args[0], vm, "argument 1 of 'acos'");
    return SHARK_FROM_NUM(acos(SHARK_AS_NUM(args[0])));
}

SHARK_NATIVE(asin)
{
    SHARK_ASSERT_NUM(args[0], vm, "argument 1 of 'asin'");
    return SHARK_FROM_NUM(asin(SHARK_AS_NUM(args[0])));
}

SHARK_NATIVE(atan)
{
    SHARK_ASSERT_NUM(args[0], vm, "argument 1 of 'atan'");
    return SHARK_FROM_NUM(atan(SHARK_AS_NUM(args[0])));
}

SHARK_NATIVE(atan2)
{
    SHARK_ASSERT_NUM(args[0], vm, "argument 1 of 'atan2'");
    SHARK_ASSERT_NUM(args[1], vm, "argument 2 of 'atan2'");
    return SHARK_FROM_NUM(atan2(SHARK_AS_NUM(args[0]), SHARK_AS_NUM(args[1])));
}

SHARK_NATIVE(cos)
{
    SHARK_ASSERT_NUM(args[0], vm, "argument 1 of 'cos'");
    return SHARK_FROM_NUM(cos(SHARK_AS_NUM(args[0])));
}

SHARK_NATIVE(cosh)
{
    SHARK_ASSERT_NUM(args[0], vm, "argument 1 of 'cosh'");
    return SHARK_FROM_NUM(cosh(SHARK_AS_NUM(args[0])));
}

SHARK_NATIVE(sin)
{
    SHARK_ASSERT_NUM(args[0], vm, "argument 1 of 'sin'");
    return SHARK_FROM_NUM(sin(SHARK_AS_NUM(args[0])));
}

SHARK_NATIVE(sinh)
{
    SHARK_ASSERT_NUM(args[0], vm, "argument 1 of 'sinh'");
    return SHARK_FROM_NUM(sinh(SHARK_AS_NUM(args[0])));
}

SHARK_NATIVE(tan)
{
    SHARK_ASSERT_NUM(args[0], vm, "argument 1 of 'tan'");
    return SHARK_FROM_NUM(tan(SHARK_AS_NUM(args[0])));
}

SHARK_NATIVE(tanh)
{
    SHARK_ASSERT_NUM(args[0], vm, "argument 1 of 'tanh'");
    return SHARK_FROM_NUM(tanh(SHARK_AS_NUM(args[0])));
}

SHARK_NATIVE(exp)
{
    SHARK_ASSERT_NUM(args[0], vm, "argument 1 of 'exp'");
    return SHARK_FROM_NUM(exp(SHARK_AS_NUM(args[0])));
}

SHARK_NATIVE(log)
{
    SHARK_ASSERT_NUM(args[0], vm, "argument 1 of 'log'");
    return SHARK_FROM_NUM(log(SHARK_AS_NUM(args[0])));
}

SHARK_NATIVE(log10)
{
    SHARK_ASSERT_NUM(args[0], vm, "argument 1 of 'log10'");
    return SHARK_FROM_NUM(log10(SHARK_AS_NUM(args[0])));
}

SHARK_NATIVE(pow)
{
    SHARK_ASSERT_NUM(args[0], vm, "argument 1 of 'pow'");
    SHARK_ASSERT_NUM(args[1], vm, "argument 2 of 'pow'");
    return SHARK_FROM_NUM(pow(SHARK_AS_NUM(args[0]), SHARK_AS_NUM(args[1])));
}

SHARK_NATIVE(sqrt)
{
    SHARK_ASSERT_NUM(args[0], vm, "argument 1 of 'sqrt'");
    return SHARK_FROM_NUM(sqrt(SHARK_AS_NUM(args[0])));
}

SHARK_NATIVE(ceil)
{
    SHARK_ASSERT_NUM(args[0], vm, "argument 1 of 'ceil'");
    return SHARK_FROM_INT((shark_int_t) ceil(SHARK_AS_NUM(args[0])));
}

SHARK_NATIVE(floor)
{
    SHARK_ASSERT_NUM(args[0], vm, "argument 1 of 'floor'");
    return SHARK_FROM_INT((shark_int_t) floor(SHARK_AS_NUM(args[0])));
}

SHARK_NATIVE(min)
{
    SHARK_ASSERT_NUM(args[0], vm, "argument 1 of 'min'");
    SHARK_ASSERT_NUM(args[1], vm, "argument 2 of 'min'");
    return SHARK_AS_NUM(args[0]) > SHARK_AS_NUM(args[1]) ? args[1] : args[0];
}

SHARK_NATIVE(max)
{
    SHARK_ASSERT_NUM(args[0], vm, "argument 1 of 'max'");
    SHARK_ASSERT_NUM(args[1], vm, "argument 2 of 'max'");
    return SHARK_AS_NUM(args[0]) > SHARK_AS_NUM(args[1]) ? args[0] : args[1];
}

SHARK_NATIVE(random)
{
    SHARK_ASSERT_INT(args[0], vm, "argument 1 of 'random'");
    shark_int_t random = (shark_int_t) (SHARK_AS_INT(args[0]) * (rand() / (double) RAND_MAX));
    if (random && random == SHARK_AS_INT(args[0])) random--;
    return SHARK_FROM_INT(random);
}

#define SHARK_BUFFER_MAX    256

SHARK_NATIVE(itos)
{
    SHARK_ASSERT_INT(args[0], vm, "argument 1 of 'itos'");
    char itos[SHARK_BUFFER_MAX];
    size_t length = sprintf(itos, "%ld", SHARK_AS_INT(args[0]));
    if (length > SHARK_BUFFER_MAX)
        shark_fatal_error(vm, "buffer overflow.");
    return SHARK_FROM_PTR(shark_string_new_from_byte_str(length, itos));
}

SHARK_NATIVE(ftos)
{
    SHARK_ASSERT_NUM(args[0], vm, "argument 1 of 'ftos'");
    char ftos[SHARK_BUFFER_MAX];
    size_t length = sprintf(ftos, "%f", SHARK_AS_NUM(args[0]));
    if (length > SHARK_BUFFER_MAX)
        shark_fatal_error(vm, "buffer overflow.");
    return SHARK_FROM_PTR(shark_string_new_from_byte_str(length, ftos));
}

SHARK_NATIVE(ctos)
{
    SHARK_ASSERT_CHAR(args[0], vm, "argument 1 of 'ctos'");
    char ctos = SHARK_AS_CHAR(args[0]);
    return SHARK_FROM_PTR(shark_string_new_from_byte_str(1, &ctos));
}

SHARK_NATIVE(stoi)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'stoi'");
    return SHARK_FROM_INT(atol(SHARK_AS_STR(args[0])->data));
}

SHARK_NATIVE(stof)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'stof'");
    return SHARK_FROM_NUM(atof(SHARK_AS_STR(args[0])->data));
}

SHARK_NATIVE(islower)
{
    SHARK_ASSERT_CHAR(args[0], vm, "argument 1 of 'islower'");
    uint8_t c = SHARK_AS_CHAR(args[0]);
    return SHARK_FROM_BOOL(c >= 'a' && c <= 'z');
}

SHARK_NATIVE(isupper)
{
    SHARK_ASSERT_CHAR(args[0], vm, "argument 1 of 'isupper'");
    uint8_t c = SHARK_AS_CHAR(args[0]);
    return SHARK_FROM_BOOL(c >= 'A' && c <= 'A');
}

SHARK_NATIVE(isalpha)
{
    SHARK_ASSERT_CHAR(args[0], vm, "argument 1 of 'isalpha'");
    uint8_t c = SHARK_AS_CHAR(args[0]);
    return SHARK_FROM_BOOL((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

SHARK_NATIVE(isdigit)
{
    SHARK_ASSERT_CHAR(args[0], vm, "argument 1 of 'isdigit'");
    uint8_t c = SHARK_AS_CHAR(args[0]);
    return SHARK_FROM_BOOL(c >= '0' && c <= '9');
}

SHARK_NATIVE(isalnum)
{
    SHARK_ASSERT_CHAR(args[0], vm, "argument 1 of 'isalnum'");
    uint8_t c = SHARK_AS_CHAR(args[0]);
    return SHARK_FROM_BOOL((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'));
}

SHARK_NATIVE(isident)
{
    SHARK_ASSERT_CHAR(args[0], vm, "argument 1 of 'isident'");
    uint8_t c = SHARK_AS_CHAR(args[0]);
    return SHARK_FROM_BOOL((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_');
}

SHARK_NATIVE(ishex)
{
    SHARK_ASSERT_CHAR(args[0], vm, "argument 1 of 'ishex'");
    uint8_t c = SHARK_AS_CHAR(args[0]);
    return SHARK_FROM_BOOL((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'));
}

SHARK_NATIVE(isascii)
{
    SHARK_ASSERT_CHAR(args[0], vm, "argument 1 of 'isascii'");
    uint8_t c = SHARK_AS_CHAR(args[0]);
    return SHARK_FROM_BOOL(c >= 0 && c <= 127);
}

SHARK_NATIVE(issurrogate)
{
    SHARK_ASSERT_CHAR(args[0], vm, "argument 1 of 'issurrogate'");
    return SHARK_FROM_BOOL(false);
}

SHARK_NATIVE(tolower)
{
    SHARK_ASSERT_CHAR(args[0], vm, "argument 1 of 'tolower'");
    uint8_t c = SHARK_AS_CHAR(args[0]);
    if (c >= 'A' && c <= 'z') {
        return SHARK_FROM_CHAR(c - 'A' + 'a');
    } else {
        return args[0];
    }
}

SHARK_NATIVE(toupper)
{
    SHARK_ASSERT_CHAR(args[0], vm, "argument 1 of 'toupper'");
    uint8_t c = SHARK_AS_CHAR(args[0]);
    if (c >= 'a' && c <= 'z') {
        return SHARK_FROM_CHAR(c - 'a' + 'A');
    } else {
        return args[0];
    }
}

SHARK_NATIVE(str_len)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'len'");
    return SHARK_FROM_INT(SHARK_AS_STR(args[0])->size);
}

SHARK_NATIVE(str_index)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'index'");
    SHARK_ASSERT_INT(args[1], vm, "argument 2 of 'index'");
    shark_string *data = SHARK_AS_STR(args[0]);
    size_t index = SHARK_AS_INT(args[1]);
    if (index < 0 || index >= data->size)
        shark_fatal_error(vm, "string index out of range.");
    return SHARK_FROM_CHAR(data->data[index]);
}

SHARK_NATIVE(str_slice)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'slice'");
    SHARK_ASSERT_INT(args[1], vm, "argument 2 of 'slice'");
    SHARK_ASSERT_INT(args[2], vm, "argument 3 of 'slice'");
    shark_string *data = SHARK_AS_STR(args[0]);
    size_t start = SHARK_AS_INT(args[1]);
    size_t end = SHARK_AS_INT(args[2]);
    if (start < 0 || start > data->size)
        shark_fatal_error(vm, "string index out of range.");
    if (end < 0 || end > data->size)
        shark_fatal_error(vm, "string index out of range.");
    if (start > end)
        start = end;
    return SHARK_FROM_PTR(shark_string_new_from_byte_str(end - start, data->data + start));
}

SHARK_NATIVE(find)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'find'");
    SHARK_ASSERT_STR(args[1], vm, "argument 2 of 'find'");
    shark_string *x = SHARK_AS_STR(args[0]);
    shark_string *y = SHARK_AS_STR(args[1]);
    for (size_t i = 0; i < x->size - y->size; )
    {
        for (size_t j = 0; i < y->size; i++)
        {
            if (x->data[i + j] != y->data[j])
                goto next;
        }
        return SHARK_FROM_INT(i);
next:
        i++;
    }
    return SHARK_FROM_INT(-1);
}

SHARK_NATIVE(concat)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'concat'");
    SHARK_ASSERT_STR(args[1], vm, "argument 2 of 'concat'");
    shark_string *x = SHARK_AS_STR(args[0]);
    shark_string *y = SHARK_AS_STR(args[1]);
    shark_string *concat = shark_string_new_with_size(x->size + y->size);
    memcpy(concat->data, x->data, x->size);
    memcpy(concat->data + x->size, y->data, y->size);
    shark_string_init(concat);
    return SHARK_FROM_PTR(concat);
}

SHARK_NATIVE(join)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'join'");
    SHARK_ASSERT_ARRAY(args[1], vm, "argument 2 of 'join'");
    shark_string *sep = SHARK_AS_STR(args[0]);
    shark_array *list = SHARK_AS_ARRAY(args[1]);
    if (list->length == 0)
        return SHARK_FROM_PTR(shark_string_new_from_cstr(""));
    size_t size = sep->size * (list->length - 1);
    for (size_t i = 0; i < list->length; i++) {
        SHARK_ASSERT_STR(list->data[i], vm, "value in argument 2 of 'join'");
        size += SHARK_AS_STR(list->data[i])->size;
    }
    shark_string *join = shark_string_new_with_size(size);
    shark_string *unit = SHARK_AS_STR(list->data[0]);
    memcpy(join->data, unit->data, unit->size);
    size_t index = unit->size;
    for (size_t i = 1; i < list->length; i++)
    {
        memcpy(join->data + index, sep->data, sep->size);
        index += sep->size;
        shark_string *part = SHARK_AS_STR(list->data[i]);
        memcpy(join->data + index, part->data, part->size);
        index += part->size;
    }
    shark_string_init(join);
    return SHARK_FROM_PTR(join);
}

SHARK_NATIVE(split)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'split'");
    SHARK_ASSERT_CHAR(args[1], vm, "argument 2 of 'split'");
    shark_string *source = SHARK_AS_STR(args[0]);
    shark_char_t sep = SHARK_AS_CHAR(args[1]);
    shark_array *split = shark_array_new();
    size_t start = 0;
    for (size_t i = 0; i < source->size; i++)
    {
        if (source->data[i] == sep)
        {
            source->data[i] = '\0';
            shark_array_put(split,
                SHARK_FROM_PTR(shark_string_new_from_cstr(source->data + start)));
            source->data[i] = sep;
            start = i + 1;
        }
    }
    shark_array_put(split,
        SHARK_FROM_PTR(shark_string_new_from_cstr(source->data + start)));
    return SHARK_FROM_PTR(split);
}

SHARK_API shark_string *shark_string_format(shark_string *format, shark_array *args)
{
    size_t buf_size = 16;
    size_t buf_len = 0;
    uint8_t *buffer = shark_malloc(buf_size);
    bool want_format = false;
    size_t format_pointer = 0;
    size_t arg_pointer = 0;

    while (format_pointer < format->size || want_format)
    {
        uint8_t temp[256];
        size_t repr_size = 0;
        uint8_t *repr;

        if (want_format) {
            if (arg_pointer == args->size)
                shark_fatal_error(NULL, "incomplete format.");
            shark_value value = args->data[arg_pointer++];
            if (SHARK_IS_INT(value)) {
                repr_size = sprintf(temp, "%ld", SHARK_AS_INT(value));
                if (repr_size > 256)
                    shark_fatal_error(NULL, "buffer overflow.");
                repr = temp;
            } else if (SHARK_IS_NUM(value)) {
                repr_size = sprintf(temp, "%f", SHARK_AS_NUM(value));
                if (repr_size > 256)
                    shark_fatal_error(NULL, "buffer overflow.");
                repr = temp;
            } else if (SHARK_IS_OBJECT(value)
            && SHARK_AS_OBJECT(value)->type == &shark_string_class) {
                repr_size = SHARK_AS_STR(value)->size;
                repr = SHARK_AS_STR(value)->data;
            } else {
                shark_fatal_error(NULL, "can't format argument.");
            }
            want_format = false;
        } else {
            size_t format_start = format_pointer;
            while (format_pointer < format->size)
            {
                if (format->data[format_pointer] == '%') {
                    want_format = true;
                    break;
                }
                format_pointer++;
            }
            repr_size = format_pointer - format_start;
            repr = format->data + format_start;
            format_pointer++;
        }

        buf_len += repr_size;
        if (buf_size < buf_len)
        {
            while (buf_size < buf_len) buf_size <<= 1;
            buffer = shark_realloc(buffer, buf_size);
        }
        memcpy(buffer + buf_len - repr_size, repr, repr_size);
    }
    
    return shark_string_new_from_byte_str(buf_len, buffer);
}

SHARK_NATIVE(format)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'format'");
    SHARK_ASSERT_ARRAY(args[1], vm, "argument 2 of 'format'");
    return SHARK_FROM_PTR(shark_string_format(SHARK_AS_STR(args[0]), SHARK_AS_ARRAY(args[1])));
}

SHARK_NATIVE(normal)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'normal'");
    shark_string *data = SHARK_AS_STR(args[0]);
    shark_string *normal = shark_string_new_with_size(data->size - 2);
    size_t size = data->size - 1;
    uint8_t *view = data->data + 1;
    uint8_t *iter = normal->data;
    for (int i = 1; i < size; i++)
    {
        uint8_t c = *view++;
        if (c == '\\')
        {
            switch (*view++)
            {
            case '0': 	c = '\0'; break;
            case 'r': 	c = '\r'; break;
            case 'n': 	c = '\n'; break;
            case 't': 	c = '\t'; break;
            case '\\': 	c = '\\'; break;
            case '"': 	c = '"'; break;
            case '\'': 	c = '\''; break;
            }
            i++;
        }
        *iter++ = c;
    }
    size_t final_size = (size_t) (iter - normal->data);
    normal->data = shark_realloc(normal->data, final_size + 1);
    normal->data[final_size] = '\0';
    normal->size = final_size;
    shark_string_init(normal);
    return SHARK_FROM_PTR(normal);
}

SHARK_NATIVE(quote)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'quote'");
    shark_string *data = SHARK_AS_STR(args[0]);
    shark_string *quote = shark_string_new_with_size(data->size * 2 + 2);
    size_t size = data->size;
    uint8_t *view = data->data;
    uint8_t *iter = quote->data;
    *iter++ = '"';
    for (int i = 0; i < size; i++)
    {
        uint8_t c = *view++;
        switch (c)
        {
        case '0':
            *iter++ = '\\';
            *iter++ = '\0';
            break;
        case 'r':
            *iter++ = '\\';
            *iter++ = '\r';
            break;
        case 'n':
            *iter++ = '\\';
            *iter++ = '\n';
            break;
        case 't':
            *iter++ = '\\';
            *iter++ = '\t';
            break;
        case '\\':
            *iter++ = '\\';
            *iter++ = '\\';
            break;
        case '"':
            *iter++ = '\\';
            *iter++ = '"';
            break;
        case '\'':
            *iter++ = '\\';
            *iter++ = '\'';
            break;
        default:
            *iter++ = c;
            break;
        }
    }
    *iter++ = '"';
    size_t final_size = (size_t) (iter - quote->data);
    quote->data = shark_realloc(quote->data, final_size + 1);
    quote->data[final_size] = '\0';
    return SHARK_FROM_PTR(quote);
}

typedef struct {
    shark_object super;
    size_t length;
    size_t size;
    uint8_t *data;
} shark_strbuf;

static void shark_strbuf_destroy(shark_object *self)
{
    shark_free(((shark_strbuf *) self)->data);
}

shark_class *shark_strbuf_class = NULL;

#define SHARK_STRBUF_INIT_SIZE      16
#define SHARK_STRBUF_GROW_SIZE(x)   (x << 1)

#define SHARK_AS_STRBUF(x)  ((shark_strbuf *) SHARK_AS_PTR(x))

SHARK_NATIVE(strbuf_init)
{
    shark_strbuf *self = SHARK_AS_STRBUF(args[0]);
    self->length = 0;
    self->size = SHARK_STRBUF_INIT_SIZE;
    self->data = shark_realloc(self->data, SHARK_STRBUF_INIT_SIZE);
    return SHARK_NULL;
}

SHARK_NATIVE(strbuf_put)
{
    SHARK_ASSERT_CHAR(args[1], vm, "argument 1 of 'strbuf.put'");
    shark_strbuf *self = SHARK_AS_STRBUF(args[0]);
    self->data[self->length++] = SHARK_AS_CHAR(args[1]);

    if (self->length == self->size)
    {
        self->size = SHARK_STRBUF_GROW_SIZE(self->size);
        self->data = shark_realloc(self->data, self->size);
    }

    return SHARK_NULL;
}

SHARK_NATIVE(strbuf_puts)
{
    SHARK_ASSERT_STR(args[1], vm, "argument 1 of 'strbuf.puts'");
    shark_strbuf *self = SHARK_AS_STRBUF(args[0]);
    shark_string *data = SHARK_AS_STR(args[1]);

    size_t length = self->length + data->size;
    size_t new_size = self->size;

    while (new_size < length)
        new_size = SHARK_STRBUF_GROW_SIZE(new_size);

    self->data = shark_realloc(self->data, new_size);
    memcpy(self->data + self->length, data->data, data->size);
    self->length = length;
    self->size = new_size;

    return SHARK_NULL;
}

SHARK_NATIVE(strbuf_printf)
{
    SHARK_ASSERT_STR(args[1], vm, "argument 1 of 'strbuf.printf'");
    SHARK_ASSERT_ARRAY(args[2], vm, "argument 2 of 'strbuf.printf'");
    shark_strbuf *self = SHARK_AS_STRBUF(args[0]);
    shark_string *data = shark_string_format(SHARK_AS_STR(args[1]), SHARK_AS_ARRAY(args[2]));

    size_t length = self->length + data->size;
    size_t new_size = self->size;

    while (new_size < length)
        new_size = SHARK_STRBUF_GROW_SIZE(new_size);

    self->data = shark_realloc(self->data, new_size);
    memcpy(self->data + self->length, data->data, data->size);
    self->length = length;
    self->size = new_size;

    shark_object_dec_ref(data);

    return SHARK_NULL;
}

SHARK_NATIVE(strbuf_read_all)
{
    shark_strbuf *self = SHARK_AS_STRBUF(args[0]);
    shark_string *data = shark_string_new_from_byte_str(self->length, self->data);

    self->length = 0;
    self->size = SHARK_STRBUF_INIT_SIZE;
    self->data = shark_realloc(self->data, SHARK_STRBUF_INIT_SIZE);

    return SHARK_FROM_PTR(data);
}

typedef shark_strbuf shark_bytes;

#define shark_bytes_destroy     shark_strbuf_destroy

shark_class *shark_bytes_class = NULL;

#define SHARK_BYTES_INIT_SIZE   SHARK_STRBUF_INIT_SIZE
#define SHARK_BYTES_GROW_SIZE   SHARK_STRBUF_GROW_SIZE

#define SHARK_AS_BYTES(x)   ((shark_bytes *) SHARK_AS_PTR(x))

SHARK_NATIVE(bytes_init)
{
    shark_bytes *self = SHARK_AS_BYTES(args[0]);
    self->length = 0;
    self->size = SHARK_BYTES_INIT_SIZE;
    self->data = shark_realloc(self->data, SHARK_BYTES_INIT_SIZE);
    return SHARK_NULL;
}

SHARK_NATIVE(bytes_put)
{
    SHARK_ASSERT_INT(args[1], vm, "argument 1 of 'bytes.put'");
    shark_bytes *self = SHARK_AS_BYTES(args[0]);
    self->data[self->length++] = (uint8_t) (SHARK_AS_INT(args[1]) & 0xFF);

    if (self->length == self->size)
    {
        self->size = SHARK_BYTES_GROW_SIZE(self->size);
        self->data = shark_realloc(self->data, self->size);
    }

    return SHARK_NULL;
}

SHARK_NATIVE(bytes_put_short)
{
    SHARK_ASSERT_INT(args[1], vm, "argument 1 of 'bytes.put_short'");
    shark_bytes *self = SHARK_AS_BYTES(args[0]);

    if ((self->length + 2) >= self->size)
    {
        self->size = SHARK_BYTES_GROW_SIZE(self->size);
        self->data = shark_realloc(self->data, self->size);
    }

    shark_int_t value = SHARK_AS_INT(args[1]);

    self->data[self->length++] = (uint8_t) (value & 0xFF);
    self->data[self->length++] = (uint8_t) ((value >> 8) & 0xFF);

    return SHARK_NULL;
}

SHARK_NATIVE(bytes_put_int)
{
    SHARK_ASSERT_INT(args[1], vm, "argument 1 of 'bytes.put_int'");
    shark_bytes *self = SHARK_AS_BYTES(args[0]);

    if ((self->length + 4) >= self->size)
    {
        self->size = SHARK_BYTES_GROW_SIZE(self->size);
        self->data = shark_realloc(self->data, self->size);
    }

    shark_int_t value = SHARK_AS_INT(args[1]);

    self->data[self->length++] = (uint8_t) (value & 0xFF);
    self->data[self->length++] = (uint8_t) ((value >> 8) & 0xFF);
    self->data[self->length++] = (uint8_t) ((value >> 16) & 0xFF);
    self->data[self->length++] = (uint8_t) ((value >> 24) & 0xFF);

    return SHARK_NULL;
}

#define SHARK_ASSERT_INSTANCE(value, class, vm, at)  SHARK_ASSERT_TYPE(vm, SHARK_IS_OBJECT(value) && SHARK_AS_OBJECT(value)->type == class, shark_class_get_name(class), at)

SHARK_NATIVE(bytes_puts)
{
    SHARK_ASSERT_INSTANCE(args[1], shark_bytes_class, vm, "argument 1 of 'bytes.puts'");
    shark_bytes *self = SHARK_AS_BYTES(args[0]);
    shark_bytes *data = SHARK_AS_BYTES(args[1]);

    size_t length = self->length + data->length;
    size_t new_size = self->size;

    while (length >= new_size)
        new_size = SHARK_BYTES_GROW_SIZE(new_size);
    
    self->size = new_size;
    self->data = shark_realloc(self->data, self->size);
    memcpy(self->data + self->length, data->data, data->length);
    self->length = length;

    return SHARK_NULL;
}

SHARK_NATIVE(bytes_tell)
{
    return SHARK_FROM_INT((shark_int_t) SHARK_AS_BYTES(args[0])->length);
}

SHARK_NATIVE(bytes_patch)
{
    SHARK_ASSERT_INT(args[1], vm, "argument 1 of 'bytes.patch'");
    SHARK_ASSERT_INT(args[2], vm, "argument 2 of 'bytes.patch'");
    shark_bytes *self = SHARK_AS_BYTES(args[0]);
    size_t pos = (size_t) SHARK_AS_INT(args[1]);
    self->data[pos] = (uint8_t) (SHARK_AS_INT(args[2]) & 0xFF);
    return SHARK_NULL;
}

SHARK_NATIVE(bytes_patch_short)
{
    SHARK_ASSERT_INT(args[1], vm, "argument 1 of 'bytes.patch_short'");
    SHARK_ASSERT_INT(args[2], vm, "argument 2 of 'bytes.patch_short'");
    shark_bytes *self = SHARK_AS_BYTES(args[0]);
    size_t pos = (size_t) SHARK_AS_INT(args[1]);
    shark_int_t value = SHARK_AS_INT(args[2]);
    self->data[pos] = (uint8_t) (value & 0xFF);
    self->data[pos+1] = (uint8_t) ((value >> 8) & 0xFF);
    return SHARK_NULL;
}

SHARK_NATIVE(bytes_patch_int)
{
    SHARK_ASSERT_INT(args[1], vm, "argument 1 of 'bytes.patch_int'");
    SHARK_ASSERT_INT(args[2], vm, "argument 2 of 'bytes.patch_int'");
    shark_bytes *self = SHARK_AS_BYTES(args[0]);
    size_t pos = (size_t) SHARK_AS_INT(args[1]);
    shark_int_t value = SHARK_AS_INT(args[2]);
    self->data[pos] = (uint8_t) (value & 0xFF);
    self->data[pos+1] = (uint8_t) ((value >> 8) & 0xFF);
    self->data[pos+2] = (uint8_t) ((value >> 16) & 0xFF);
    self->data[pos+3] = (uint8_t) ((value >> 24) & 0xFF);
    return SHARK_NULL;
}

SHARK_NATIVE(bytes_get)
{
    SHARK_ASSERT_INT(args[1], vm, "argument 1 of 'bytes.get'");
    return SHARK_FROM_INT((shark_int_t) SHARK_AS_BYTES(args[0])->data[(size_t) SHARK_AS_INT(args[1])]);
}

SHARK_NATIVE(encode)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'encode'");
    shark_string *data = SHARK_AS_STR(args[0]);
    shark_bytes *encode = shark_object_new(shark_bytes_class);
    encode->length = data->size;
    encode->size = SHARK_BYTES_INIT_SIZE;
    while (encode->size <= encode->length)
        encode->size = SHARK_BYTES_GROW_SIZE(encode->size);
    encode->data = shark_realloc(encode->data, encode->size);
    memcpy(encode->data, data->data, data->size);
    return SHARK_FROM_PTR(encode);
}

SHARK_NATIVE(decode)
{
    SHARK_ASSERT_INSTANCE(args[0], shark_bytes_class, vm, "argument 1 of 'decode'");
    shark_bytes *data = SHARK_AS_BYTES(args[0]);
    return SHARK_FROM_PTR(shark_string_new_from_byte_str(data->length, data->data));
}

typedef struct {
    shark_object super;
    FILE *buffer;
} shark_file;

shark_class *shark_text_file_class = NULL;

#define SHARK_AS_FILE(x)    ((shark_file *) SHARK_AS_PTR(x))

SHARK_NATIVE(text_file_put)
{
    SHARK_ASSERT_CHAR(args[1], vm, "argument 1 of 'text_file.put'");
    fputc((char) SHARK_AS_CHAR(args[1]), SHARK_AS_FILE(args[0])->buffer);
    return SHARK_NULL;
}

SHARK_NATIVE(text_file_puts)
{
    SHARK_ASSERT_STR(args[1], vm, "argument 1 of 'text_file.puts'");
    fputs(SHARK_AS_STR(args[1])->data, SHARK_AS_FILE(args[0])->buffer);
    return SHARK_NULL;
}

SHARK_NATIVE(text_file_printf)
{
    SHARK_ASSERT_STR(args[1], vm, "argument 1 of 'text_file.printf'");
    SHARK_ASSERT_ARRAY(args[2], vm, "argument 2 of 'text_file.printf'");
    shark_string *format = shark_string_format(SHARK_AS_STR(args[1]), SHARK_AS_ARRAY(args[2]));
    fputs(format->data, SHARK_AS_FILE(args[0])->buffer);
    shark_object_dec_ref(format);
    return SHARK_NULL;
}

SHARK_NATIVE(text_file_fetch)
{
    return SHARK_FROM_CHAR((uint8_t) fgetc(SHARK_AS_FILE(args[0])->buffer));
}

SHARK_NATIVE(text_file_read)
{
    SHARK_ASSERT_INT(args[1], vm, "argument 1 of 'text_file.read'");
    size_t size = (size_t) SHARK_AS_INT(args[1]);
    shark_string *data = shark_string_new_with_size(size);
    size_t new_size = fread(data->data, 1, size, SHARK_AS_FILE(args[0])->buffer);
    data->data = shark_realloc(data->data, new_size + 1);
    data->data[new_size] = '\0';
    return SHARK_FROM_PTR(data);
}

static shark_string *shark_read_line(FILE *source)
{
    shark_string *line = shark_string_new_with_size(256);
    size_t step = 0;
    size_t index = 0;
    size_t size;
    while (true)
    {
        size_t base = step * 256;
        for (index = 0; index < 256; index++)
        {
            char c = fgetc(source);
            if (c == EOF || c == '\n')
                goto end;
            line->data[base + index] = (uint8_t) c & 0xFF;
        }
        line->data = shark_realloc(line->data, line->size + 256 + 1);
        line->data[line->size + 256] = '\0';
        step++;
    }
end:
    size = step * 256 + index;
    line->data[size] = '\0';
    line->data = shark_realloc(line->data, size + 1);
    line->size = size;
    shark_string_init(line);
    return line;
}

SHARK_NATIVE(file_at_end)
{
    int c = fgetc(SHARK_AS_FILE(args[0])->buffer);
    ungetc(c, SHARK_AS_FILE(args[0])->buffer);
    return SHARK_FROM_BOOL(c == EOF);
}

SHARK_NATIVE(file_close)
{
    fclose(SHARK_AS_FILE(args[0])->buffer);
    return SHARK_NULL;
}

shark_class *shark_binary_file_class = NULL;

SHARK_NATIVE(binary_file_put)
{
    SHARK_ASSERT_INT(args[1], vm, "argument 1 of 'binary_file.put'");
    fputc((int) (SHARK_AS_INT(args[1]) & 0xFF), SHARK_AS_FILE(args[0])->buffer);
    return SHARK_NULL;
}

SHARK_NATIVE(binary_file_puts)
{
    SHARK_ASSERT_INSTANCE(args[1], shark_bytes_class, vm, "argument 1 of 'binary_file.puts'");
    shark_file *file = SHARK_AS_FILE(args[0]);
    shark_bytes *data = SHARK_AS_BYTES(args[1]);
    for (size_t i = 0; i < data->length; i++)
        fputc(data->data[i], file->buffer);
    return SHARK_NULL;
}

SHARK_NATIVE(binary_file_fetch)
{
    return SHARK_FROM_INT((uint8_t) fgetc(SHARK_AS_FILE(args[0])->buffer));
}

SHARK_NATIVE(binary_file_read)
{
    SHARK_ASSERT_INT(args[1], vm, "argument 1 of 'binary_file.read'");
    size_t size = (size_t) SHARK_AS_INT(args[1]);
    shark_bytes *data = shark_object_new(shark_bytes_class);
    data->data = shark_malloc(size);
    size = fread(data->data, 1, size, SHARK_AS_FILE(args[0])->buffer);
    data->size = SHARK_BYTES_INIT_SIZE;
    while (data->size <= size)
        data->size = SHARK_BYTES_GROW_SIZE(data->size);
    data->data = shark_realloc(data->data, data->size);
    data->length = size;
    return SHARK_FROM_PTR(data);
}

SHARK_NATIVE(open)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'open'");
    SHARK_ASSERT_STR(args[1], vm, "argument 2 of 'open'");
    shark_string *real_mode = SHARK_AS_STR(args[1]);
    char mode[3];
    mode[0] = real_mode->data[0];
    mode[1] = 'b';
    mode[2] = '\0';
    shark_file *self;
    if (real_mode->size == 2 && real_mode->data[1] == 'b')
        self = shark_object_new(shark_binary_file_class);
    else
        self = shark_object_new(shark_text_file_class);
    self->buffer = fopen(SHARK_AS_STR(args[0])->data, mode);
    if (self->buffer == NULL)
    {
        shark_errno = 1;
        shark_object_dec_ref(self);
        return SHARK_NULL;
    }
    return SHARK_FROM_PTR(self);
}

SHARK_NATIVE(put)
{
    SHARK_ASSERT_CHAR(args[0], vm, "argument 1 of 'put'");
    fputc((int) SHARK_AS_CHAR(args[0]), stdout);
    return SHARK_NULL;
}

SHARK_NATIVE(puts)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'puts'");
    fputs(SHARK_AS_STR(args[0])->data, stdout);
    return SHARK_NULL;
}

SHARK_NATIVE(printf)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'printf'");
    SHARK_ASSERT_ARRAY(args[1], vm, "argument 2 of 'printf'");
    shark_string *format = shark_string_format(SHARK_AS_STR(args[0]), SHARK_AS_ARRAY(args[1]));
    fputs(format->data, stdout);
    shark_object_dec_ref(format);
    return SHARK_NULL;
}

SHARK_NATIVE(read_line)
{
    return SHARK_FROM_PTR(shark_read_line(stdin));
}

SHARK_NATIVE(copy)
{
    SHARK_ASSERT_OBJECT(args[0], vm, "argument 1 of 'copy'");
    shark_object *object = SHARK_AS_OBJECT(args[0]);
    if (object->type == &shark_array_class) {
        shark_array *source = (shark_array *) object;
        shark_array *copy = shark_array_new();
        shark_array_preallocate(copy, ((shark_array *) object)->length);
        for (size_t i = 0; i < source->length; i++)
            copy->data[i] = shark_value_inc_ref(source->data[i]);
        copy->length = source->length;
        return SHARK_FROM_PTR(copy);
    } else if (object->type == &shark_table_class) {
        return SHARK_FROM_PTR(shark_table_copy((shark_table *) object));
    } else {
        shark_fatal_error(vm, "can't copy object (not an array or table).");
    }
}

SHARK_NATIVE(slice)
{
    SHARK_ASSERT_ARRAY(args[0], vm, "argument 1 of 'slice'");
    SHARK_ASSERT_INT(args[1], vm, "argument 2 of 'slice'");
    SHARK_ASSERT_INT(args[2], vm, "argument 3 of 'slice'");
    shark_array *list = SHARK_AS_ARRAY(args[0]);
    size_t start = (size_t) SHARK_AS_INT(args[1]);
    size_t end = (size_t) SHARK_AS_INT(args[2]);
    if (start < 0 || end > list->length)
        shark_fatal_error(vm, "array index out of range.");
    if (start > end)
        shark_fatal_error(vm, "negative slice range.");
    shark_array *slice = shark_array_new();
    size_t size = end - start;
    shark_array_preallocate(slice, size);
    for (size_t i = 0; i < size; i++)
        slice->data[i] = shark_value_inc_ref(list->data[start+i]);
    slice->length = size;
    return SHARK_FROM_PTR(slice);
}

SHARK_NATIVE(pop)
{
    SHARK_ASSERT_ARRAY(args[0], vm, "argument 1 of 'pop'");
    return shark_array_pop(SHARK_AS_ARRAY(args[0]));
}

SHARK_NATIVE(popindex)
{
    SHARK_ASSERT_ARRAY(args[0], vm, "argument 1 of 'popindex'");
    SHARK_ASSERT_INT(args[1], vm, "argument 2 of 'popindex'");
    shark_array *list = SHARK_AS_ARRAY(args[0]);
    size_t index = (size_t) SHARK_AS_INT(args[1]);
    if (index < 0 || index >= list->length)
        shark_fatal_error(vm, "array index out of range.");
    shark_value value = list->data[index];
    list->length--;
    for (size_t i = index; i < list->length; i++)
        list->data[i] = list->data[i+1];
    shark_array_shrink(list);
    return value;
}

SHARK_NATIVE(array_find)
{
    SHARK_ASSERT_ARRAY(args[0], vm, "argument 1 of 'find'");
    shark_array *list = SHARK_AS_ARRAY(args[0]);
    shark_value value = args[1];
    for (size_t i = 0; i < list->length; i++)
        if (shark_value_equals(value, list->data[i]))
            return SHARK_FROM_INT(i);
    return SHARK_FROM_INT(-1);
}

SHARK_NATIVE(array_concat)
{
    SHARK_ASSERT_ARRAY(args[0], vm, "argument 1 of 'concat'");
    SHARK_ASSERT_ARRAY(args[1], vm, "argument 2 of 'concat'");
    shark_array *x = SHARK_AS_ARRAY(args[0]);
    shark_array *y = SHARK_AS_ARRAY(args[1]);
    shark_array *concat = shark_array_new();
    size_t length = x->length + y->length;
    shark_array_preallocate(concat, length);
    size_t x_size = x->length;
    for (size_t i = 0; i < x_size; i++)
        concat->data[i] = shark_value_inc_ref(x->data[i]);
    size_t y_size = y->length;
    for (size_t i = 0; i < y_size; i++)
        concat->data[x_size + i] = shark_value_inc_ref(y->data[i]);
    return SHARK_FROM_PTR(concat);
}

SHARK_NATIVE(extend)
{
    SHARK_ASSERT_ARRAY(args[0], vm, "argument 1 of 'extend'");
    SHARK_ASSERT_ARRAY(args[1], vm, "argument 2 of 'extend'");
    shark_array *list = SHARK_AS_ARRAY(args[0]);
    shark_array *other = SHARK_AS_ARRAY(args[1]);
    shark_array_preallocate(list, list->length + other->length);
    for (size_t i = 0; i < other->length; i++)
        list->data[list->length + i] = shark_value_inc_ref(other->data[i]);
    return SHARK_NULL;
}

SHARK_NATIVE(remove)
{
    SHARK_ASSERT_TABLE(args[0], vm, "argument 1 of 'remove'");
    return shark_table_pop_index(SHARK_AS_TABLE(args[0]), args[1]);
}

SHARK_NATIVE(update)
{
    SHARK_ASSERT_TABLE(args[0], vm, "argument 1 of 'update'");
    SHARK_ASSERT_TABLE(args[1], vm, "argument 2 of 'update'");
    shark_table_update(SHARK_AS_TABLE(args[0]), SHARK_AS_TABLE(args[1]));
    return SHARK_NULL;
}

#undef SHARK_NATIVE

SHARK_API void shark_init_library(shark_vm *vm, shark_string *library_path)
{
    shark_string *system_archive_tail = shark_string_new_from_cstr("system.shar");
    shark_string *system_archive_name = shark_path_join(library_path, system_archive_tail);

    FILE *system_archive = fopen(system_archive_name->data, "rb");
    shark_object_dec_ref(system_archive_name);

    if (system_archive == NULL)
    {
        fprintf(stderr, "could not find the system archive, aborting execution.");
        exit(EXIT_FAILURE);
    }

    shark_read_archive(vm, system_archive_tail, system_archive);

    shark_string *module_name;
    shark_module *module;
    shark_class *type;
    shark_function *function;

    // system.exit
    module_name = shark_string_new_from_cstr("system.exit");
    module = shark_vm_import_module(vm, module_name);

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "exit"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_exit;

    // system.time
    module_name = shark_string_new_from_cstr("system.time");
    module = shark_vm_import_module(vm, module_name);

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "clock"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_clock;

    // system.error
    module_name = shark_string_new_from_cstr("system.error");
    module = shark_vm_import_module(vm, module_name);

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "get_err"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_get_err;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "has_err"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_has_err;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "set_err"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_set_err;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "clear_err"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_clear_err;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "error"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_error;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "pcall"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_pcall;

    // system.path
    module_name = shark_string_new_from_cstr("system.path");
    module = shark_vm_import_module(vm, module_name);

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "get_base"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_path_get_base;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "get_tail"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_path_get_tail;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "get_ext"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_path_get_ext;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "remove_ext"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_path_remove_ext;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "join"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_path_join;

#ifndef CSHARK_NO_FS
    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "listdir"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_listdir;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "mkdir"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_mkdir;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "rmdir"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_rmdir;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "unlink"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_unlink;
#endif // CSHARK_NO_FS

    // system.math
    module_name = shark_string_new_from_cstr("system.math");
    module = shark_vm_import_module(vm, module_name);

    shark_string *const_name;

    const_name = shark_string_new_from_cstr("pi");
    shark_table_set_index(module->names, SHARK_FROM_PTR(const_name), SHARK_FROM_NUM(M_PI));
    shark_object_dec_ref(const_name);

    const_name = shark_string_new_from_cstr("e");
    shark_table_set_index(module->names, SHARK_FROM_PTR(const_name), SHARK_FROM_NUM(M_E));
    shark_object_dec_ref(const_name);

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "abs"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_abs;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "acos"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_acos;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "asin"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_asin;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "atan"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_atan;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "atan2"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_atan2;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "cos"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_cos;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "cosh"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_cosh;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "sin"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_sin;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "sinh"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_sinh;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "tan"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_tan;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "tanh"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_tanh;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "exp"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_exp;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "log"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_log;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "log10"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_log10;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "pow"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_pow;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "sqrt"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_sqrt;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "ceil"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_ceil;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "floor"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_floor;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "min"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_min;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "max"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_max;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "random"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_random;

    // system.string
    module_name = shark_string_new_from_cstr("system.string");
    module = shark_vm_import_module(vm, module_name);

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "itos"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_itos;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "ftos"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_ftos;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "ctos"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_ctos;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "stoi"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_stoi;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "stof"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_stof;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "islower"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_islower;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "isupper"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_isupper;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "isalpha"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_isalpha;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "isdigit"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_isdigit;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "isalnum"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_isalnum;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "isident"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_isident;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "ishex"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_ishex;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "isascii"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_isascii;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "issurrogate"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_issurrogate;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "tolower"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_tolower;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "toupper"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_toupper;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "len"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_str_len;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "index"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_str_index;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "slice"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_str_slice;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "find"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_find;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "concat"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_concat;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "join"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_join;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "split"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_split;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "format"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_format;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "normal"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_normal;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "quote"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_quote;

    type = SHARK_AS_CLASS(shark_table_get_str(module->names, "strbuf"));
    type->object_size = sizeof(shark_strbuf);
    type->destroy = shark_strbuf_destroy;
    type->is_object_class = false;

    shark_strbuf_class = type;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "init"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_strbuf_init;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "put"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_strbuf_put;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "puts"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_strbuf_puts;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "printf"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_strbuf_printf;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "read_all"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_strbuf_read_all;

    type = SHARK_AS_CLASS(shark_table_get_str(module->names, "bytes"));
    type->object_size = sizeof(shark_bytes);
    type->destroy = shark_bytes_destroy;
    type->is_object_class = false;

    shark_bytes_class = type;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "init"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_bytes_init;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "put"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_bytes_put;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "put_short"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_bytes_put_short;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "put_int"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_bytes_put_int;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "puts"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_bytes_puts;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "tell"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_bytes_tell;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "patch"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_bytes_patch;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "patch_short"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_bytes_patch_short;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "patch_int"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_bytes_patch_int;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "get"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_bytes_get;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "encode"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_encode;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "decode"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_decode;

    // system.io
    module_name = shark_string_new_from_cstr("system.io");
    module = shark_vm_import_module(vm, module_name);

    type = SHARK_AS_CLASS(shark_table_get_str(module->names, "text_file"));
    type->object_size = sizeof(shark_file);
    type->destroy = shark_default_destroy;
    type->is_object_class = false;

    shark_text_file_class = type;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "put"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_text_file_put;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "puts"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_text_file_puts;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "printf"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_text_file_printf;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "fetch"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_text_file_fetch;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "read"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_text_file_read;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "at_end"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_file_at_end;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "close"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_file_close;

    type = SHARK_AS_CLASS(shark_table_get_str(module->names, "binary_file"));
    type->object_size = sizeof(shark_file);
    type->destroy = shark_default_destroy;
    type->is_object_class = false;

    shark_binary_file_class = type;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "put"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_binary_file_put;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "puts"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_binary_file_puts;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "fetch"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_binary_file_fetch;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "read"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_binary_file_read;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "at_end"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_file_at_end;

    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "close"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_file_close;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "open"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_open;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "put"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_put;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "puts"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_puts;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "printf"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_printf;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "read_line"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_read_line;

    // system.util
    module_name = shark_string_new_from_cstr("system.util");
    module = shark_vm_import_module(vm, module_name);

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "copy"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_copy;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "slice"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_slice;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "pop"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_pop;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "popindex"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_popindex;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "find"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_array_find;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "concat"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_array_concat;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "extend"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_extend;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "remove"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_remove;

    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "update"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_update;
}

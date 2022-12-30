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
#include "cshark_core.c"
#include "cshark_system.c"

int main(int argc, char **argv)
{
    if (argc >= 2) {
        shark_vm *vm = shark_vm_new();
        shark_string *exec = shark_string_new_from_cstr(argv[0]);
        shark_string *base = shark_path_get_base(exec);
        
        shark_vm_add_import_path(vm, base);
        shark_init_library(vm, base);
        
        shark_string *filename = shark_string_new_from_cstr(argv[1]);
        FILE *source = fopen(filename->data, "rb");
        
        if (source == NULL)
        {
            shark_string *archive_name = shark_path_join(base, filename);
            source = fopen(archive_name->data, "rb");
            
            if (source == NULL)
            {
                printf("can't open input file '%s', execution aborted.", filename->data);
                return -1;
            }
            
            shark_object_dec_ref(filename);
            filename = archive_name;
        }
        
        shark_string *root = shark_path_get_base(filename);
        shark_vm_add_import_path(vm, root);
        
        shark_module *module = shark_read_archive(vm, shark_path_get_tail(filename), source);
        fclose(source);
        
        shark_vm_exec_module(vm, module);
        shark_array *args = shark_array_new();
        shark_array_put(args, SHARK_FROM_PTR(filename));
        
        for (size_t i = 2; i < argc; i++) {
            shark_array_put(args, SHARK_FROM_PTR(shark_string_new_from_cstr(argv[i])));
            // shark_object_dec_ref(SHARK_AS_OBJECT(args->data[args->length-1]));
        }
        
        shark_vm_exec_main(vm, module, args);
        shark_object_dec_ref(args);
        return 0;
    } else {
        printf("usage: %s <command> <args>", argv[0]);
        return -1;
    }
}

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

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL2_rotozoom.h"

#define SHARK_NATIVE(name) \
    static shark_value shark_lib_ ## name(shark_vm *vm, shark_value *args, shark_error *error)

// shark.texture

typedef struct {
    shark_object super;
    SDL_Surface *surface;
} shark_texture;

static void shark_texture_destroy(shark_object *object)
{
    SDL_FreeSurface(((shark_texture *) object)->surface);
}

static shark_class *shark_texture_class = NULL;

SHARK_NATIVE(texture_get_size_x)
{
    shark_texture *texture = (shark_texture *) SHARK_AS_OBJECT(args[0]);
    return SHARK_FROM_NUM(texture->surface->w);
}

SHARK_NATIVE(texture_get_size_y)
{
    shark_texture *texture = (shark_texture *) SHARK_AS_OBJECT(args[0]);
    return SHARK_FROM_NUM(texture->surface->h);
}

// shark.text

typedef struct {
    shark_object super;
    TTF_Font *font;
    SDL_Color color;
} shark_font;

static void shark_font_destroy(shark_object *object)
{
    TTF_CloseFont(((shark_font *) object)->font);
}

static shark_class *shark_font_class = NULL;

SHARK_NATIVE(font_get_height)
{
    return SHARK_FROM_INT(TTF_FontHeight(((shark_font *) SHARK_AS_OBJECT(args[0]))->font));
}

SHARK_NATIVE(font_get_width)
{
    int height = 0, width = 0;
    TTF_SizeUTF8(((shark_font *) SHARK_AS_OBJECT(args[0]))->font,
        (char *) ((shark_string *) SHARK_AS_OBJECT(args[1]))->data,
        &height, &width);
    return SHARK_FROM_INT(width);
}

// shark.asset

static shark_string *asset_dir = NULL;

SHARK_NATIVE(load_texture)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'load_texture'");
    shark_string *name = SHARK_AS_STR(args[0]);
    shark_string *path = shark_path_join(asset_dir, name);
    SDL_Surface *image = IMG_Load(path->data);
    shark_object_dec_ref(path);
    
    if (image == NULL)
    {
        shark_set_err(1);
        return SHARK_NULL;
    }
    
    shark_texture *result = shark_object_new(shark_texture_class);
    result->surface = image;
    
    return SHARK_FROM_PTR(result);
}

SHARK_NATIVE(load_font)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'load_font'");
    SHARK_ASSERT_INT(args[1], vm, "argument 2 of 'load_font'");
    SHARK_ASSERT_INT(args[2], vm, "argument 3 of 'load_font'");
    shark_string *name = SHARK_AS_STR(args[0]);
    shark_string *path = shark_path_join(asset_dir, name);
    TTF_Font *font = TTF_OpenFont(path->data, (int) SHARK_AS_INT(args[1]));
    shark_object_dec_ref(path);
    
    if (font == NULL)
    {
        shark_set_err(1);
        return SHARK_NULL;
    }
    
    shark_int_t color = SHARK_AS_INT(args[2]);
    shark_font *result = shark_object_new(shark_font_class);
    result->font = font;
    result->color = (SDL_Color) {
        (color >> 24 ) & 0xFF, (color >> 16 ) & 0xFF,
        (color >> 8 ) & 0xFF, color & 0xFF };
    
    return SHARK_FROM_PTR(result);
}

// shark.activity

typedef struct {
    shark_table super;
    SDL_Window *window;
    SDL_Surface *window_display;
    SDL_Surface *display;
} shark_activity;

void shark_activity_destroy(shark_object *self)
{
    // ...
}

shark_class *shark_activity_class;

#define SHARK_SCALE     3

shark_activity *shark_activity_new()
{
    shark_activity *self = shark_object_new(shark_activity_class);
    shark_table_init((shark_table *) self);
    self->window = SDL_CreateWindow("Gameshark", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320 * SHARK_SCALE, 192 * SHARK_SCALE, SDL_WINDOW_SHOWN);
    self->window_display = SDL_GetWindowSurface(self->window);
    self->display = SDL_CreateRGBSurface(0, 320, 192, 32, 0, 0, 0, 0);
    if (self->display == NULL) shark_fatal_error(NULL, "could not initialize display.");
    return self;
}

SHARK_NATIVE(activity_draw)
{
    SHARK_ASSERT_INSTANCE(args[1], shark_texture_class, vm, "argument 1 of 'activity.draw'");
    SHARK_ASSERT_INT(args[2], vm, "argument 2 of 'activity.draw'");
    SHARK_ASSERT_INT(args[3], vm, "argument 3 of 'activity.draw'");
    shark_activity *act = (shark_activity *) SHARK_AS_OBJECT(args[0]);
    shark_texture *tex = (shark_texture *) SHARK_AS_OBJECT(args[1]);
    int x = (int) SHARK_AS_INT(args[2]);
    int y = (int) SHARK_AS_INT(args[3]);
    
    SDL_Rect dest = { x, y, tex->surface->w, tex->surface->h };
    SDL_BlitSurface(tex->surface, NULL, act->display, &dest);
    
    return SHARK_NULL;
}

SHARK_NATIVE(activity_draw_ex)
{
    SHARK_ASSERT_INSTANCE(args[1], shark_texture_class, vm, "argument 1 of 'activity.draw_ex'");
    SHARK_ASSERT_INT(args[2], vm, "argument 2 of 'activity.draw_ex'");
    SHARK_ASSERT_INT(args[3], vm, "argument 3 of 'activity.draw_ex'");
    SHARK_ASSERT_INT(args[4], vm, "argument 4 of 'activity.draw_ex'");
    SHARK_ASSERT_INT(args[5], vm, "argument 5 of 'activity.draw_ex'");
    SHARK_ASSERT_NUM(args[6], vm, "argument 6 of 'activity.draw_ex'");
    SHARK_ASSERT_NUM(args[7], vm, "argument 7 of 'activity.draw_ex'");
    SHARK_ASSERT_NUM(args[8], vm, "argument 8 of 'activity.draw_ex'");
    shark_activity *act = (shark_activity *) SHARK_AS_OBJECT(args[0]);
    shark_texture *tex = (shark_texture *) SHARK_AS_OBJECT(args[1]);
    int x = (int) SHARK_AS_INT(args[2]);
    int y = (int) SHARK_AS_INT(args[3]);
    int origin_x = (int) SHARK_AS_INT(args[4]);
    int origin_y = (int) SHARK_AS_INT(args[5]);
    double rotation = (double) SHARK_AS_NUM(args[6]);
    double scale_x = (double) SHARK_AS_NUM(args[7]);
    double scale_y = (double) SHARK_AS_NUM(args[8]);
    
    SDL_Surface *render = rotozoomSurfaceXY(tex->surface, rotation, scale_x, scale_y, 1);
    
    double r = rotation * M_PI / 180;
    int ox = (int) (cos(r) * origin_x + sin(r) * origin_y) * scale_x;
    int oy = (int) (cos(r) * origin_y + sin(r) * origin_x) * scale_y;
    x = x + ox - render->w / 2;
    y = y + oy - render->h / 2;
    
    SDL_Rect dest = { x, y, render->w, render->h };
    SDL_BlitSurface(render, NULL, act->display, &dest);
    SDL_FreeSurface(render);
    
    return SHARK_NULL;
}

SHARK_NATIVE(activity_draw_text)
{
    SHARK_ASSERT_STR(args[1], vm, "argument 1 of 'activity.draw_text'");
    SHARK_ASSERT_INSTANCE(args[2], shark_font_class, vm, "argument 2 of 'activity.draw_text'");
    SHARK_ASSERT_INT(args[3], vm, "argument 3 of 'activity.draw_text'");
    SHARK_ASSERT_INT(args[4], vm, "argument 4 of 'activity.draw_text'");
    
    shark_activity *act = (shark_activity *) SHARK_AS_OBJECT(args[0]);
    shark_string *data = SHARK_AS_STR(args[1]);
    shark_font *font = (shark_font *) SHARK_AS_OBJECT(args[2]);
    int x = (int) SHARK_AS_INT(args[3]);
    int y = (int) SHARK_AS_INT(args[4]);
    
    SDL_Surface *render = TTF_RenderUTF8_Blended(font->font, data->data, font->color);
    SDL_Rect dest = { x, y, render->w, render->h };
    SDL_BlitSurface(render, NULL, act->display, &dest);
    SDL_FreeSurface(render);
    
    return SHARK_NULL;
}

SHARK_NATIVE(activity_empty)
{
    return SHARK_NULL;
}

static shark_string *save_file_name = NULL;

SHARK_NATIVE(get_save_file)
{
    shark_value open_args[2] = { SHARK_FROM_PTR(save_file_name), args[0] };
    return shark_lib_open(vm, &open_args[0], error);
}

#undef SHARK_NATIVE

typedef enum {
    E_NONE = 0,
    E_PRESS = 1,
    E_MOVE = 2,
    E_RELEASE = 3,
    E_PRESS_UP = 4,
    E_PRESS_DOWN = 5,
    E_PRESS_LEFT = 6,
    E_PRESS_RIGHT = 7,
    E_PRESS_X = 8,
    E_PRESS_Y = 9,
    E_REL_UP = 10,
    E_REL_DOWN = 11,
    E_REL_LEFT = 12,
    E_REL_RIGHT = 13,
    E_REL_X = 14,
    E_REL_Y = 15
} shark_event;

static void shark_init_game_library(shark_vm *vm, shark_string *library_path)
{
    shark_string *shark_archive_tail = shark_string_new_from_cstr("gameshark.shar");
    shark_string *shark_archive_name = shark_path_join(library_path, shark_archive_tail);
    
    FILE *shark_archive = fopen(shark_archive_name->data, "rb");
    shark_object_dec_ref(shark_archive_name);
    
    if (shark_archive == NULL)
    {
        fprintf(stderr, "could not find the gameshark archive, aborting execution.");
        exit(EXIT_FAILURE);
    }
    
    shark_read_archive(vm, shark_archive_tail, shark_archive);
    
    shark_string *module_name;
    shark_module *module;
    shark_class *type;
    shark_function *function;
    
    // shark.activity
    module_name = shark_string_new_from_cstr("shark.activity");
    module = shark_vm_import_module(vm, module_name);
    
    type = SHARK_AS_CLASS(shark_table_get_str(module->names, "activity"));
    type->object_size = sizeof(shark_activity);
    type->destroy = shark_activity_destroy;
    
    shark_activity_class = type;
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "draw"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_activity_draw;
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "draw_ex"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_activity_draw_ex;
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "draw_text"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_activity_draw_text;
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "launch"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_activity_empty;
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "event"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_activity_empty;
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "update"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_activity_empty;
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "render"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_activity_empty;
    
    // shark.asset
    module_name = shark_string_new_from_cstr("shark.asset");
    module = shark_vm_import_module(vm, module_name);
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "load_texture"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_load_texture;
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "load_font"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_load_font;
    
    // shark.persistent
    
    module_name = shark_string_new_from_cstr("shark.persistent");
    module = shark_vm_import_module(vm, module_name);
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "get_save_file"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_get_save_file;
    
    // shark.text
    module_name = shark_string_new_from_cstr("shark.text");
    module = shark_vm_import_module(vm, module_name);
    
    type = SHARK_AS_CLASS(shark_table_get_str(module->names, "font"));
    type->object_size = sizeof(shark_font);
    type->destroy = shark_font_destroy;
    type->is_object_class = false;
    
    shark_font_class = type;
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "get_height"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_font_get_height;
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "get_width"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_font_get_width;
    
    // shark.texture
    module_name = shark_string_new_from_cstr("shark.texture");
    module = shark_vm_import_module(vm, module_name);
    
    type = SHARK_AS_CLASS(shark_table_get_str(module->names, "texture"));
    type->object_size = sizeof(shark_texture);
    type->destroy = shark_texture_destroy;
    type->is_object_class = false;
    
    shark_texture_class = type;
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "get_size_x"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_texture_get_size_x;
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "get_size_y"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_texture_get_size_y;
}

void shark_exec_game(shark_vm *vm)
{
    if (SDL_Init(SDL_INIT_VIDEO)
    || !IMG_Init(IMG_INIT_PNG)
    || TTF_Init())
        shark_fatal_error(NULL, "couldn't initialize libraries.");
    
    shark_string *main_name = shark_string_new_from_cstr("main");
    shark_module *module = shark_vm_import_module(vm, main_name);
    
    shark_class *main_class = SHARK_AS_CLASS(shark_table_get_str(module->names, "main_activity"));
    if (main_class == NULL) shark_fatal_error(NULL, "can't load activity class.");
    
    shark_activity *main = shark_activity_new();
    ((shark_object *) main)->type = main_class;
    
#define PUSH(value) { \
                    shark_value __PUSH_VALUE__ = value; \
                    vm->stack[vm->TOS++] = __PUSH_VALUE__; \
                    shark_value_inc_ref(__PUSH_VALUE__); \
                    if (vm->TOS == vm->stack_size) \
                        shark_vm_grow_stack(vm); \
                }
    
#define SHARK_CALL_METHOD(name) \
    { \
        shark_function *method = SHARK_AS_FUNCTION(shark_table_get_str(main_class->methods, name)); \
        shark_error error; \
        error.protect = false; \
        error.message = NULL; \
        vm->error = &error; \
        if (method->type == SHARK_BYTECODE_FUNCTION) { \
            shark_vm_frame frame; \
            frame.parent = NULL; \
            frame.module = module; \
            frame.function = method; \
            frame.globals = module->names; \
            frame.const_table = module->const_table; \
            frame.base = 0; \
            frame.code = method->code.bytecode; \
            shark_vm_execute(vm, frame); \
        } else { \
            method->code.native_code(vm, vm->stack, &error); \
        } \
        while (vm->TOS > 0) \
            shark_value_dec_ref(vm->stack[--vm->TOS]); \
    }
    
    PUSH(SHARK_FROM_PTR(main));
    SHARK_CALL_METHOD("launch");
    
    bool pressed = false;
    
    while (true)
    {
        SDL_Event event;
        
#define DISPATCH_EVENT(type, x, y) do { \
    PUSH(SHARK_FROM_PTR(main)); \
    PUSH(SHARK_FROM_INT(type)); \
    PUSH(SHARK_FROM_INT(x)); \
    PUSH(SHARK_FROM_INT(y)); \
    SHARK_CALL_METHOD("event"); \
    } while (false)
        
#define DISPATCH_KEY(type) \
    DISPATCH_EVENT(type, 0, 0); \
    break
        
        while (SDL_PollEvent(&event))
        switch (event.type)
        {
        case SDL_QUIT:
            goto end;
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                DISPATCH_EVENT(E_PRESS, event.button.x / SHARK_SCALE, event.button.y / SHARK_SCALE);
                pressed = true;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                DISPATCH_EVENT(E_RELEASE, event.button.x / SHARK_SCALE, event.button.y / SHARK_SCALE);
                pressed = false;
            }
            break;
        case SDL_MOUSEMOTION:
            if (pressed)
                DISPATCH_EVENT(E_MOVE, event.motion.x / SHARK_SCALE, event.motion.y / SHARK_SCALE);
            break;
        case SDL_KEYDOWN: {
            switch (event.key.keysym.sym)
            {
            case SDLK_UP: DISPATCH_KEY(E_PRESS_UP);
            case SDLK_DOWN: DISPATCH_KEY(E_PRESS_DOWN);
            case SDLK_LEFT: DISPATCH_KEY(E_PRESS_LEFT);
            case SDLK_RIGHT: DISPATCH_KEY(E_PRESS_RIGHT);
            case SDLK_z: DISPATCH_KEY(E_PRESS_X);
            case SDLK_x: DISPATCH_KEY(E_PRESS_Y);
            }
            break;
        }
        case SDL_KEYUP:
            switch (event.key.keysym.sym)
            {
            case SDLK_UP: DISPATCH_KEY(E_REL_UP);
            case SDLK_DOWN: DISPATCH_KEY(E_REL_DOWN);
            case SDLK_LEFT: DISPATCH_KEY(E_REL_LEFT);
            case SDLK_RIGHT: DISPATCH_KEY(E_REL_RIGHT);
            case SDLK_z: DISPATCH_KEY(E_REL_X);
            case SDLK_x: DISPATCH_KEY(E_REL_Y);
            }
            break;
        default:
            break;
#undef DISPATCH_EVENT
#undef DISPATCH_KEY
        }
        
        PUSH(SHARK_FROM_PTR(main));
        SHARK_CALL_METHOD("update");
        
        PUSH(SHARK_FROM_PTR(main));
        SHARK_CALL_METHOD("render");
        
        main->window_display = SDL_GetWindowSurface(main->window);
        SDL_BlitScaled(main->display, NULL, main->window_display, NULL);
        
        SDL_UpdateWindowSurface(main->window);
        SDL_Delay(1000 / 24);
    }
#undef PUSH
#undef SHARK_CALL_METHOD
end:
    SDL_Quit();
    IMG_Quit();
    TTF_Quit();
}

#ifdef _WIN32
int SDL_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
    if (argc == 2)
    {
        shark_vm *vm = shark_vm_new();
        shark_string *exec = shark_string_new_from_cstr(argv[0]);
        shark_string *base = shark_path_get_base(exec);
        
        shark_vm_add_import_path(vm, base);
        shark_init_library(vm, base);
        shark_init_game_library(vm, base);
        
        shark_string *gamedir = shark_string_new_from_cstr(argv[1]);
        shark_string *tail = shark_string_new_from_cstr("bin/game.shar");
        shark_string *filename = shark_path_join(gamedir, tail);
        shark_object_dec_ref(tail);
        
        FILE *source = fopen(filename->data, "rb");
        
        if (source == NULL)
        {
            printf("can't open code archive 'bin/game.shar', execution aborted.");
            return -1;
        }
        
        tail = shark_string_new_from_cstr("asset");
        asset_dir = shark_path_join(gamedir, tail);
        shark_object_dec_ref(tail);
        
        tail = shark_string_new_from_cstr("save");
        save_file_name = shark_path_join(gamedir, tail);
        shark_object_dec_ref(tail);
        
        shark_module *module = shark_read_archive(vm, shark_path_get_tail(filename), source);
        fclose(source);
        
        shark_exec_game(vm);
        
        return 0;
    }
    else
    {
        printf("usage: %s <game-dir>", argv[0]);
        return -1;
    }
}

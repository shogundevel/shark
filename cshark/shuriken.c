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
#include "SDL_ttf.h"

#define SHARK_NATIVE(name) \
    static shark_value shark_lib_ ## name(shark_vm *vm, shark_value *args, shark_error *error)

// shuriken.render

typedef struct {
    shark_object super;
    SDL_Surface *surface;
} shark_render;

static void shark_render_destroy(shark_object *object)
{
    SDL_FreeSurface(((shark_render *) object)->surface);
}

static shark_class *shark_render_class = NULL;

SHARK_NATIVE(render_draw)
{
    SHARK_ASSERT_INSTANCE(args[1], shark_render_class, vm, "argument 1 of 'render.draw'");
    SHARK_ASSERT_INT(args[2], vm, "argument 2 of 'render.draw'");
    SHARK_ASSERT_INT(args[3], vm, "argument 3 of 'render.draw'");
    shark_render *render_x = (shark_render *) SHARK_AS_OBJECT(args[0]);
    shark_render *render_y = (shark_render *) SHARK_AS_OBJECT(args[1]);
    int x = (int) SHARK_AS_INT(args[2]);
    int y = (int) SHARK_AS_INT(args[3]);
    
    SDL_Rect dest = { x, y, render_y->surface->w, render_y->surface->h };
    SDL_BlitSurface(render_y->surface, NULL, render_x->surface, &dest);
    
    return SHARK_NULL;
}

SHARK_NATIVE(render_get_size_x)
{
    shark_render *render = (shark_render *) SHARK_AS_OBJECT(args[0]);
    return SHARK_FROM_NUM(render->surface->w);
}

SHARK_NATIVE(render_get_size_y)
{
    shark_render *render = (shark_render *) SHARK_AS_OBJECT(args[0]);
    return SHARK_FROM_NUM(render->surface->h);
}

typedef struct {
    shark_object super;
    TTF_Font *font;
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

SHARK_NATIVE(font_get_size)
{
    int height = 0, width = 0;
    TTF_SizeUTF8(((shark_font *) SHARK_AS_OBJECT(args[0]))->font,
        (char *) ((shark_string *) SHARK_AS_OBJECT(args[1]))->data,
        &height, &width);
    return SHARK_FROM_INT(width);
}

SHARK_NATIVE(render_color)
{
    SHARK_ASSERT_INT(args[0], vm, "argument 1 of 'color'");
    SHARK_ASSERT_INT(args[1], vm, "argument 2 of 'color'");
    SHARK_ASSERT_INT(args[2], vm, "argument 3 of 'color'");
    SHARK_ASSERT_INT(args[3], vm, "argument 4 of 'color'");
    return SHARK_FROM_INT((SHARK_AS_INT(args[0]) << 24) | (SHARK_AS_INT(args[1]) << 16) | (SHARK_AS_INT(args[2]) << 8) | SHARK_AS_INT(args[3]));
}

SHARK_NATIVE(render_solid)
{
    SHARK_ASSERT_INT(args[0], vm, "argument 1 of 'render_solid'");
    SHARK_ASSERT_INT(args[1], vm, "argument 2 of 'render_solid'");
    SHARK_ASSERT_INT(args[2], vm, "argument 3 of 'render_solid'");
    shark_int_t color_code = SHARK_AS_INT(args[0]);
    int size_x = (int) SHARK_AS_INT(args[1]);
    int size_y = (int) SHARK_AS_INT(args[2]);
    SDL_Surface *render_surf = SDL_CreateRGBSurface(0, size_x, size_y, 32, 0, 0, 0, 0);
    
    if (render_surf == NULL)
    {
        fprintf(stderr, "could not render solid: %s", SDL_GetError());
        exit(-1);
    }
    
    SDL_FillRect(render_surf, NULL, (Uint32) (color_code >> 8 | (color_code & 0xFF) << 24));
    shark_render *render = shark_object_new(shark_render_class);
    render->surface = render_surf;
    return SHARK_FROM_PTR(render);
}

SHARK_NATIVE(render_text)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'render_text'");
    SHARK_ASSERT_INSTANCE(args[1], shark_font_class, vm, "argument 2 of 'render_text'");
    SHARK_ASSERT_INT(args[2], vm, "argument 3 of 'render_text'");
    
    shark_string *data = SHARK_AS_STR(args[0]);
    shark_font *font = (shark_font *) SHARK_AS_OBJECT(args[1]);
    shark_int_t color_code = SHARK_AS_INT(args[2]);
    SDL_Color color = (SDL_Color) {
        (color_code >> 24 ) & 0xFF, (color_code >> 16 ) & 0xFF,
        (color_code >> 8 ) & 0xFF, color_code & 0xFF };
    
    SDL_Surface *render_text = TTF_RenderUTF8_Blended(font->font, data->data, color);
    
    if (render_text == NULL)
        render_text = SDL_CreateRGBSurface(0, 0, 0, 32, 0, 0, 0, 0);
    
    if (render_text == NULL)
    {
        fprintf(stderr, "could not render text: %s", SDL_GetError());
        exit(-1);
    }
    
    shark_render *render = (shark_render *) shark_object_new(shark_render_class);
    render->surface = render_text;
    
    return SHARK_FROM_PTR(render);
}

// shuriken.asset

shark_string *asset_path = NULL;

SHARK_NATIVE(load_image)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'load_texture'");
    shark_string *path = SHARK_AS_STR(args[0]);
    shark_string *join = shark_path_join(asset_path, path);
    SDL_Surface *image = SDL_LoadBMP(join->data);
    shark_object_dec_ref(join);
    
    if (image == NULL)
    {
        fprintf(stderr, "could not load image: %s", SDL_GetError());
        exit(-1);
    }
    
    shark_render *result = shark_object_new(shark_render_class);
    result->surface = image;
    
    return SHARK_FROM_PTR(result);
}

SHARK_NATIVE(load_font)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'load_font'");
    SHARK_ASSERT_INT(args[1], vm, "argument 2 of 'load_font'");
    shark_string *path = SHARK_AS_STR(args[0]);
    shark_string *join = shark_path_join(asset_path, path);
    TTF_Font *font = TTF_OpenFont(join->data, (int) SHARK_AS_INT(args[1]));
    shark_object_dec_ref(join);
    
    if (font == NULL)
    {
        shark_set_err(1);
        return SHARK_NULL;
    }
    
    shark_font *result = shark_object_new(shark_font_class);
    result->font = font;
    
    return SHARK_FROM_PTR(result);
}

// shuriken.activity

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

shark_activity *shark_activity_new(int size_x, int size_y, char *name, int scale)
{
    shark_activity *self = shark_object_new(shark_activity_class);
    shark_table_init((shark_table *) self);
    self->window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, size_x * scale, size_y * scale, SDL_WINDOW_SHOWN);
    self->window_display = SDL_GetWindowSurface(self->window);
    self->display = SDL_CreateRGBSurface(0, size_x, size_y, 32, 0, 0, 0, 0);
    
    if (self->display == NULL)
    {
        fprintf(stderr, "could not create display: %s", SDL_GetError());
        exit(-1);
    }
    
    return self;
}

SHARK_NATIVE(activity_draw)
{
    SHARK_ASSERT_INSTANCE(args[1], shark_render_class, vm, "argument 1 of 'activity.draw'");
    SHARK_ASSERT_INT(args[2], vm, "argument 2 of 'activity.draw'");
    SHARK_ASSERT_INT(args[3], vm, "argument 3 of 'activity.draw'");
    shark_activity *act = (shark_activity *) SHARK_AS_OBJECT(args[0]);
    shark_render *tex = (shark_render *) SHARK_AS_OBJECT(args[1]);
    int x = (int) SHARK_AS_INT(args[2]);
    int y = (int) SHARK_AS_INT(args[3]);
    
    SDL_Rect dest = { x, y, tex->surface->w, tex->surface->h };
    SDL_BlitSurface(tex->surface, NULL, act->display, &dest);
    
    return SHARK_NULL;
}

#undef SHARK_NATIVE

typedef enum {
    E_NONE = 0,
    E_PRESS = 1,
    E_RELEASE = 2,
    E_MOVE = 3,
    E_ROLL_UP = 4,
    E_ROLL_DOWN = 5,
    E_PRESS_UP = 6,
    E_PRESS_DOWN = 7,
    E_PRESS_LEFT = 8,
    E_PRESS_RIGHT = 9,
    E_PRESS_ENTER = 10,
    E_PRESS_BACK = 11,
    E_PRESS_CONTROL = 12,
    E_REL_UP = 13,
    E_REL_DOWN = 14,
    E_REL_LEFT = 15,
    E_REL_RIGHT = 16,
    E_REL_ENTER = 17,
    E_REL_BACK = 18,
    E_REL_CONTROL = 19,
    E_TEXT_DOWN = 20,
    E_TEXT_UP = 21
} shark_event;

static void shark_init_fx_library(shark_vm *vm, shark_string *library_path)
{
    shark_string *shark_archive_tail = shark_string_new_from_cstr("shuriken.shar");
    shark_string *shark_archive_name = shark_path_join(library_path, shark_archive_tail);
    
    FILE *shark_archive = fopen(shark_archive_name->data, "rb");
    shark_object_dec_ref(shark_archive_name);
    
    if (shark_archive == NULL)
    {
        fprintf(stderr, "could not find the shuriken archive, aborting execution.");
        exit(EXIT_FAILURE);
    }
    
    shark_read_archive(vm, shark_archive_tail, shark_archive);
    
    shark_string *module_name;
    shark_module *module;
    shark_class *type;
    shark_function *function;
    
    // shuriken.activity
    module_name = shark_string_new_from_cstr("shuriken.activity");
    module = shark_vm_import_module(vm, module_name);
    
    type = SHARK_AS_CLASS(shark_table_get_str(module->names, "activity"));
    type->object_size = sizeof(shark_activity);
    type->destroy = shark_activity_destroy;
    
    shark_activity_class = type;
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "draw"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_activity_draw;
    
    // shuriken.asset
    module_name = shark_string_new_from_cstr("shuriken.asset");
    module = shark_vm_import_module(vm, module_name);
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "load_image"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_load_image;
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "load_font"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_load_font;
    
    // shuriken.render
    module_name = shark_string_new_from_cstr("shuriken.render");
    module = shark_vm_import_module(vm, module_name);
    
    type = SHARK_AS_CLASS(shark_table_get_str(module->names, "render"));
    type->object_size = sizeof(shark_render);
    type->destroy = shark_render_destroy;
    type->is_object_class = false;
    
    shark_render_class = type;
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "draw"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_render_draw;
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "get_size_x"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_render_get_size_x;
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "get_size_y"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_render_get_size_y;
    
    type = SHARK_AS_CLASS(shark_table_get_str(module->names, "font"));
    type->object_size = sizeof(shark_font);
    type->destroy = shark_font_destroy;
    type->is_object_class = false;
    
    shark_font_class = type;
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "get_height"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_font_get_height;
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(type->methods, "get_size"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_font_get_size;
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "color"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_render_color;
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "render_solid"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_render_solid;
    
    function = SHARK_AS_FUNCTION(shark_table_get_str(module->names, "render_text"));
    function->type = SHARK_NATIVE_FUNCTION;
    function->code.native_code = shark_lib_render_text;
}

int SIZE_X = 640;
int SIZE_Y = 480;
int SCALE = 1;
char *NAME = "Shuriken";

void shark_exec_app(shark_vm *vm, shark_module *module)
{
    if (SDL_Init(SDL_INIT_VIDEO)
    || TTF_Init())
        shark_fatal_error(NULL, "couldn't initialize libraries.");
    
    shark_vm_exec_module(vm, module);
    
    shark_class *main_class = SHARK_AS_CLASS(shark_table_get_str(module->names, "main_activity"));
    if (main_class == NULL) shark_fatal_error(NULL, "can't load activity class.");
    
    shark_activity *main = shark_activity_new(SIZE_X, SIZE_Y, NAME, SCALE);
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
    PUSH(SHARK_FROM_INT(SIZE_X));
    PUSH(SHARK_FROM_INT(SIZE_Y));
    SHARK_CALL_METHOD("launch");
    
    bool shift_down = false;
    
    while (true)
    {
        SDL_Event event;
        
#define DISPATCH_EVENT(type, x, y) do { \
    PUSH(SHARK_FROM_PTR(main)); \
    PUSH(SHARK_FROM_INT(type)); \
    PUSH(SHARK_FROM_INT(x)); \
    PUSH(SHARK_FROM_INT(y)); \
    PUSH(SHARK_FROM_CHAR('\0')); \
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
                DISPATCH_EVENT(E_PRESS, event.button.x, event.button.y);
            break;
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT)
                DISPATCH_EVENT(E_RELEASE, event.button.x, event.button.y);
            break;
        case SDL_MOUSEMOTION:
            DISPATCH_EVENT(E_MOVE, event.motion.x, event.motion.y);
            break;
#define DISPATCH_TEXT(type, char) do { \
    PUSH(SHARK_FROM_PTR(main)); \
    PUSH(SHARK_FROM_INT(type)); \
    PUSH(SHARK_FROM_INT(0)); \
    PUSH(SHARK_FROM_INT(0)); \
    PUSH(SHARK_FROM_CHAR(char)); \
    SHARK_CALL_METHOD("event"); \
    } while (false);
        case SDL_KEYDOWN: {
            switch (event.key.keysym.sym)
            {
            case SDLK_UP: DISPATCH_KEY(E_PRESS_UP);
            case SDLK_DOWN: DISPATCH_KEY(E_PRESS_DOWN);
            case SDLK_LEFT: DISPATCH_KEY(E_PRESS_LEFT);
            case SDLK_RIGHT: DISPATCH_KEY(E_PRESS_RIGHT);
            case SDLK_RETURN: DISPATCH_KEY(E_PRESS_ENTER);
            case SDLK_BACKSPACE: DISPATCH_KEY(E_PRESS_BACK);
            case SDLK_PAGEUP: DISPATCH_KEY(E_ROLL_UP);
            case SDLK_PAGEDOWN: DISPATCH_KEY(E_ROLL_DOWN);
            case SDLK_LCTRL: DISPATCH_KEY(E_PRESS_CONTROL);
            case SDLK_LSHIFT:
                shift_down = true;
                break;
#define SELECT_KEY(name, x, y) \
    case SDLK_ ## name: \
        if (shift_down) { \
            DISPATCH_TEXT(E_TEXT_DOWN, y); \
        } else { \
            DISPATCH_TEXT(E_TEXT_DOWN, x); \
        } break
            SELECT_KEY(a, 'a', 'A');
            SELECT_KEY(b, 'b', 'B');
            SELECT_KEY(c, 'c', 'C');
            SELECT_KEY(d, 'd', 'D');
            SELECT_KEY(e, 'e', 'E');
            SELECT_KEY(f, 'f', 'F');
            SELECT_KEY(g, 'g', 'G');
            SELECT_KEY(h, 'h', 'H');
            SELECT_KEY(i, 'i', 'I');
            SELECT_KEY(j, 'j', 'J');
            SELECT_KEY(k, 'k', 'K');
            SELECT_KEY(l, 'l', 'L');
            SELECT_KEY(m, 'm', 'M');
            SELECT_KEY(n, 'n', 'N');
            SELECT_KEY(o, 'o', 'O');
            SELECT_KEY(p, 'p', 'P');
            SELECT_KEY(q, 'q', 'Q');
            SELECT_KEY(r, 'r', 'R');
            SELECT_KEY(s, 's', 'S');
            SELECT_KEY(t, 't', 'T');
            SELECT_KEY(u, 'u', 'U');
            SELECT_KEY(v, 'v', 'V');
            SELECT_KEY(w, 'w', 'W');
            SELECT_KEY(x, 'x', 'X');
            SELECT_KEY(y, 'y', 'Y');
            SELECT_KEY(z, 'z', 'Z');
            
            SELECT_KEY(0, '0', ')');
            SELECT_KEY(1, '1', '!');
            SELECT_KEY(2, '2', '@');
            SELECT_KEY(3, '3', '#');
            SELECT_KEY(4, '4', '$');
            SELECT_KEY(5, '5', '%');
            SELECT_KEY(6, '6', '^');
            SELECT_KEY(7, '7', '&');
            SELECT_KEY(8, '8', '*');
            SELECT_KEY(9, '9', '(');
            
            SELECT_KEY(KP_0, '0', '0');
            SELECT_KEY(KP_1, '1', '1');
            SELECT_KEY(KP_2, '2', '2');
            SELECT_KEY(KP_3, '3', '3');
            SELECT_KEY(KP_4, '4', '4');
            SELECT_KEY(KP_5, '5', '5');
            SELECT_KEY(KP_6, '6', '6');
            SELECT_KEY(KP_7, '7', '7');
            SELECT_KEY(KP_8, '8', '8');
            SELECT_KEY(KP_9, '9', '9');
            
            SELECT_KEY(SPACE, ' ', ' ');
            SELECT_KEY(MINUS, '-', '_');
            SELECT_KEY(EQUALS, '=', '+');
            SELECT_KEY(LEFTBRACKET, '[', '{');
            SELECT_KEY(RIGHTBRACKET, ']', '}');
            SELECT_KEY(SEMICOLON, ';', ':');
            SELECT_KEY(QUOTE, '\'', '"');
            SELECT_KEY(COMMA, ',', '<');
            SELECT_KEY(PERIOD, '.', '>');
            SELECT_KEY(SLASH, '/', '?');
            SELECT_KEY(BACKSLASH, '\\', '|');
            SELECT_KEY(TAB, '\t', '\t');
#undef SELECT_KEY
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
            case SDLK_RETURN: DISPATCH_KEY(E_REL_ENTER);
            case SDLK_BACKSPACE: DISPATCH_KEY(E_REL_BACK);
            case SDLK_LCTRL: DISPATCH_KEY(E_REL_CONTROL);
            case SDLK_LSHIFT:
                shift_down = false;
                break;
#define SELECT_KEY(name, x, y) \
    case SDLK_ ## name: \
        if (shift_down) { \
            DISPATCH_TEXT(E_TEXT_UP, y); \
        } else { \
            DISPATCH_TEXT(E_TEXT_UP, x); \
        } break
            SELECT_KEY(a, 'a', 'A');
            SELECT_KEY(b, 'b', 'B');
            SELECT_KEY(c, 'c', 'C');
            SELECT_KEY(d, 'd', 'D');
            SELECT_KEY(e, 'e', 'E');
            SELECT_KEY(f, 'f', 'F');
            SELECT_KEY(g, 'g', 'G');
            SELECT_KEY(h, 'h', 'H');
            SELECT_KEY(i, 'i', 'I');
            SELECT_KEY(j, 'j', 'J');
            SELECT_KEY(k, 'k', 'K');
            SELECT_KEY(l, 'l', 'L');
            SELECT_KEY(m, 'm', 'M');
            SELECT_KEY(n, 'n', 'N');
            SELECT_KEY(o, 'o', 'O');
            SELECT_KEY(p, 'p', 'P');
            SELECT_KEY(q, 'q', 'Q');
            SELECT_KEY(r, 'r', 'R');
            SELECT_KEY(s, 's', 'S');
            SELECT_KEY(t, 't', 'T');
            SELECT_KEY(u, 'u', 'U');
            SELECT_KEY(v, 'v', 'V');
            SELECT_KEY(w, 'w', 'W');
            SELECT_KEY(x, 'x', 'X');
            SELECT_KEY(y, 'y', 'Y');
            SELECT_KEY(z, 'z', 'Z');
            
            SELECT_KEY(0, '0', ')');
            SELECT_KEY(1, '1', '!');
            SELECT_KEY(2, '2', '@');
            SELECT_KEY(3, '3', '#');
            SELECT_KEY(4, '4', '$');
            SELECT_KEY(5, '5', '%');
            SELECT_KEY(6, '6', '^');
            SELECT_KEY(7, '7', '&');
            SELECT_KEY(8, '8', '*');
            SELECT_KEY(9, '9', '(');
            
            SELECT_KEY(KP_0, '0', '0');
            SELECT_KEY(KP_1, '1', '1');
            SELECT_KEY(KP_2, '2', '2');
            SELECT_KEY(KP_3, '3', '3');
            SELECT_KEY(KP_4, '4', '4');
            SELECT_KEY(KP_5, '5', '5');
            SELECT_KEY(KP_6, '6', '6');
            SELECT_KEY(KP_7, '7', '7');
            SELECT_KEY(KP_8, '8', '8');
            SELECT_KEY(KP_9, '9', '9');
            
            SELECT_KEY(SPACE, ' ', ' ');
            SELECT_KEY(MINUS, '-', '_');
            SELECT_KEY(EQUALS, '=', '+');
            SELECT_KEY(LEFTBRACKET, '[', '{');
            SELECT_KEY(RIGHTBRACKET, ']', '}');
            SELECT_KEY(SEMICOLON, ';', ':');
            SELECT_KEY(QUOTE, '\'', '"');
            SELECT_KEY(COMMA, ',', '<');
            SELECT_KEY(PERIOD, '.', '>');
            SELECT_KEY(SLASH, '/', '?');
            SELECT_KEY(BACKSLASH, '\\', '|');
            SELECT_KEY(TAB, '\t', '\t');
#undef SELECT_KEY
            }
            break;
        default:
            break;
#undef DISPATCH_TEXT
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
    TTF_Quit();
}

int main(int argc, char **argv)
{
    if (argc >= 2)
    {
        if (argc >= 4)
        {
            SIZE_X = atoi(argv[2]);
            SIZE_Y = atoi(argv[3]);
            
            if (argc >= 5) SCALE = atoi(argv[4]);
            if (argc >= 6) NAME = argv[5];
        }
        
        shark_vm *vm = shark_vm_new();
        shark_string *exec = shark_string_new_from_cstr(argv[0]);
        shark_string *base = shark_path_get_base(exec);
        
        shark_vm_add_import_path(vm, base);
        shark_init_library(vm, base);
        shark_init_fx_library(vm, base);
        
        shark_string *filename = shark_string_new_from_cstr(argv[1]);
        
        FILE *source = fopen(filename->data, "rb");
        
        if (source == NULL)
        {
            shark_string *archive_name = shark_path_join(base, filename);
            source = fopen(archive_name->data, "rb");
            
            if (source == NULL)
            {
                printf("can't open code archive '%s', execution aborted.", filename->data);
                return -1;
            }
            
            shark_object_dec_ref(filename);
            filename = archive_name;
        }
        
        shark_string *code_base = shark_path_get_base(filename);
        shark_vm_add_import_path(vm, code_base);
        
        asset_path = code_base;
        
        shark_module *module = shark_read_archive(vm, shark_path_get_tail(filename), source);
        fclose(source);
        
        shark_exec_app(vm, module);
        
        return 0;
    }
    else
    {
        printf("usage: %s <app.shar>", argv[0]);
        return -1;
    }
}

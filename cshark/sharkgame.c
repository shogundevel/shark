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

#ifdef __PSP__
#define SHARK_UNBOX
#define CSHARK_NO_FS
#endif

#include "cshark.h"
#include "cshark_core.c"
#include "cshark_system.c"

#include "SDL_config.h"
#if defined(__TINYC__) && defined(_WIN32) && defined(HAVE_STRINGS_H)
#undef HAVE_STRINGS_H
#define alloca      _ALLOCA
#define SDL_DISABLE_IMMINTRIN_H
#endif

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL2_rotozoom.h"
#include "SDL_main.h"

#define SHARK_DIRECT_RENDER

#ifdef __PSP__
    #define SHARK_DIRECT_DRAW   true
    #ifndef SHARK_DIRECT_RENDER
        #define SHARK_SOFT_RESCALE  true
        #define SHARK_FAKE_SCALE    true
    #endif
    #define SHARK_SOFT_SCALE    1.5
#endif

#ifdef SHARK_SOFT_RESCALE
    #define SHARK_SCALE_UP(x)   ((int) ((x) * SHARK_SOFT_SCALE))
    #define SHARK_SCALE_DOWN(x) ((int) ((x) / SHARK_SOFT_SCALE))
#else
    #define SHARK_SCALE_UP(x)   (x)
    #define SHARK_SCALE_DOWN(x) (x)
#endif

#define SHARK_NATIVE(name) \
    static shark_value shark_lib_ ## name(shark_vm *vm, shark_value *args, shark_error *error)

#ifdef SHARK_DIRECT_RENDER
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
#endif

// shark.texture

typedef struct {
    shark_object super;
#ifndef SHARK_DIRECT_RENDER
    SDL_Surface *surface;
#else
    SDL_Texture *texture;
    int w;
    int h;
#endif
} shark_texture;

static void shark_texture_destroy(shark_object *object)
{
#ifndef SHARK_DIRECT_RENDER
    SDL_FreeSurface(((shark_texture *) object)->surface);
#else
    SDL_DestroyTexture(((shark_texture *) object)->texture);
#endif
}

static shark_class *shark_texture_class = NULL;

SHARK_NATIVE(texture_get_size_x)
{
    shark_texture *texture = (shark_texture *) SHARK_AS_OBJECT(args[0]);
#ifndef SHARK_DIRECT_RENDER
    return SHARK_FROM_NUM(SHARK_SCALE_DOWN(texture->surface->w));
#else
    return SHARK_FROM_NUM(texture->w);
#endif
}

SHARK_NATIVE(texture_get_size_y)
{
    shark_texture *texture = (shark_texture *) SHARK_AS_OBJECT(args[0]);
#ifndef SHARK_DIRECT_RENDER
    return SHARK_FROM_NUM(SHARK_SCALE_DOWN(texture->surface->h));
#else
    return SHARK_FROM_NUM(texture->h);
#endif
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
    return SHARK_FROM_INT(SHARK_SCALE_DOWN(TTF_FontHeight(((shark_font *) SHARK_AS_OBJECT(args[0]))->font)));
}

SHARK_NATIVE(font_get_width)
{
    int height = 0, width = 0;
    TTF_SizeUTF8(((shark_font *) SHARK_AS_OBJECT(args[0]))->font,
        (char *) ((shark_string *) SHARK_AS_OBJECT(args[1]))->data,
        &height, &width);
    return SHARK_FROM_INT(SHARK_SCALE_DOWN(width));
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
#ifdef SHARK_DIRECT_RENDER
    result->texture = SDL_CreateTextureFromSurface(renderer, image);
    result->w = image->w;
    result->h = image->h;
    SDL_FreeSurface(image);
#elif defined(SHARK_FAKE_SCALE)
    SDL_Surface *render = zoomSurface(image, SHARK_SOFT_SCALE, SHARK_SOFT_SCALE, 1);
    SDL_FreeSurface(image);
    result->surface = render;
#else
    result->surface = image;
#endif
    
    return SHARK_FROM_PTR(result);
}

SHARK_NATIVE(load_font)
{
    SHARK_ASSERT_STR(args[0], vm, "argument 1 of 'load_font'");
    SHARK_ASSERT_INT(args[1], vm, "argument 2 of 'load_font'");
    SHARK_ASSERT_INT(args[2], vm, "argument 3 of 'load_font'");
    
    shark_string *name = SHARK_AS_STR(args[0]);
    shark_string *path = shark_path_join(asset_dir, name);
    
    TTF_Font *font = TTF_OpenFont(path->data, (int) SHARK_SCALE_UP(SHARK_AS_INT(args[1])));
    
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
#ifndef SHARK_DIRECT_RENDER
    SDL_Surface *window_display;
    SDL_Surface *display;
#else
    SDL_Renderer *renderer;
#endif
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
    return self;
}

SHARK_NATIVE(activity_draw)
{
    SHARK_ASSERT_INSTANCE(args[1], shark_texture_class, vm, "argument 1 of 'activity.draw'");
    SHARK_ASSERT_INT(args[2], vm, "argument 2 of 'activity.draw'");
    SHARK_ASSERT_INT(args[3], vm, "argument 3 of 'activity.draw'");
    
    shark_activity *act = (shark_activity *) SHARK_AS_OBJECT(args[0]);
    shark_texture *tex = (shark_texture *) SHARK_AS_OBJECT(args[1]);
    
    int x = (int) SHARK_SCALE_UP(SHARK_AS_INT(args[2]));
    int y = (int) SHARK_SCALE_UP(SHARK_AS_INT(args[3]));
    
#ifndef SHARK_DIRECT_RENDER
    SDL_Rect dest = { x, y, tex->surface->w, tex->surface->h };
    SDL_BlitSurface(tex->surface, NULL, act->display, &dest);
#else
    SDL_Rect dest = { x, y, tex->w, tex->h };
    SDL_RenderCopy(renderer, tex->texture, NULL, &dest);
#endif
    
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
    
    int x = (int) SHARK_SCALE_UP(SHARK_AS_INT(args[2]));
    int y = (int) SHARK_SCALE_UP(SHARK_AS_INT(args[3]));
    
    int origin_x = (int) SHARK_SCALE_UP(SHARK_AS_INT(args[4]));
    int origin_y = (int) SHARK_SCALE_UP(SHARK_AS_INT(args[5]));
    
    double rotation = (double) SHARK_AS_NUM(args[6]);
    
    double scale_x = (double) SHARK_AS_NUM(args[7]);
    double scale_y = (double) SHARK_AS_NUM(args[8]);
    
#ifndef SHARK_DIRECT_RENDER
    SDL_Surface *render = rotozoomSurfaceXY(tex->surface, rotation, scale_x, scale_y, 1);
    
    double r = rotation * M_PI / 180;
    
    int ox = (int) (cos(r) * origin_x + sin(r) * origin_y) * scale_x;
    int oy = (int) (cos(r) * origin_y + sin(r) * origin_x) * scale_y;
    
    x = x + ox - render->w / 2;
    y = y + oy - render->h / 2;
    
    SDL_Rect dest = { x, y, render->w, render->h };
    SDL_BlitSurface(render, NULL, act->display, &dest);
    SDL_FreeSurface(render);
#else
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    
    if (scale_x < 0)
    {
        flip |= SDL_FLIP_HORIZONTAL;
        scale_x = -scale_x;
    }
    
    if (scale_y < 0)
    {
        flip |= SDL_FLIP_VERTICAL;
        scale_y = -scale_y;
    }
    
    SDL_Rect dest = (SDL_Rect) {
        x - (int) ((origin_x + (tex->w / 2)) * scale_x),
        y - (int) ((origin_y + (tex->h / 2)) * scale_y),
        (int) (tex->w * scale_x),
        (int) (tex->h * scale_y)
    };
    
    SDL_Point center = (SDL_Point) {
        (int) ((origin_x + (tex->w / 2)) * scale_x),
        (int) ((origin_y + (tex->h / 2)) * scale_y)
    };
    
    SDL_RenderCopyEx(renderer, tex->texture, NULL, &dest, -rotation, &center, flip);
#endif
    
    return SHARK_NULL;
}

#ifdef SHARK_DIRECT_RENDER
#define TEXT_RENDER_MAX     512
size_t TEXT_RENDER_COUNT = 0;
SDL_Texture *TEXT_RENDER_BUFFER[TEXT_RENDER_MAX];
#endif

SHARK_NATIVE(activity_draw_text)
{
    SHARK_ASSERT_STR(args[1], vm, "argument 1 of 'activity.draw_text'");
    SHARK_ASSERT_INSTANCE(args[2], shark_font_class, vm, "argument 2 of 'activity.draw_text'");
    SHARK_ASSERT_INT(args[3], vm, "argument 3 of 'activity.draw_text'");
    SHARK_ASSERT_INT(args[4], vm, "argument 4 of 'activity.draw_text'");
    
    shark_activity *act = (shark_activity *) SHARK_AS_OBJECT(args[0]);
    shark_string *data = SHARK_AS_STR(args[1]);
    shark_font *font = (shark_font *) SHARK_AS_OBJECT(args[2]);
    
    int x = (int) SHARK_SCALE_UP(SHARK_AS_INT(args[3]));
    int y = (int) SHARK_SCALE_UP(SHARK_AS_INT(args[4]));
    
#ifdef SHARK_DIRECT_RENDER
#ifdef __PSP__
    SDL_Surface *render = TTF_RenderUTF8_Solid(font->font, data->data, font->color);
#else
    SDL_Surface *render = TTF_RenderUTF8_Blended(font->font, data->data, font->color);
#endif
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, render);
    SDL_FreeSurface(render);
#else
    SDL_Surface *render = TTF_RenderUTF8_Blended(font->font, data->data, font->color);
#endif
    SDL_Rect dest = { x, y, render->w, render->h };
#ifdef SHARK_DIRECT_RENDER
    SDL_RenderCopy(renderer, texture, NULL, &dest);
    
    TEXT_RENDER_BUFFER[TEXT_RENDER_COUNT++] = texture;
    
    if (TEXT_RENDER_COUNT >= TEXT_RENDER_MAX)
        shark_fatal_error(vm, "Text render buffer overflow.");
#else
    SDL_BlitSurface(render, NULL, act->display, &dest);
    SDL_FreeSurface(render);
#endif
    
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

static void shark_init_game_library(shark_vm *vm)
{
    shark_module *module;
    shark_class *type;
    shark_function *function;
    
    // shark.activity
    module = shark_vm_bind_module(vm, "shark.activity");
    
    type = shark_activity_class = shark_vm_bind_class(vm, module, "activity", sizeof(shark_activity), shark_activity_destroy, true);
    shark_vm_bind_function(vm, module, type, "draw", 3, shark_lib_activity_draw);
    shark_vm_bind_function(vm, module, type, "draw_ex", 8, shark_lib_activity_draw_ex);
    shark_vm_bind_function(vm, module, type, "draw_text", 4, shark_lib_activity_draw_text);
    shark_vm_bind_function(vm, module, type, "launch", 0, shark_lib_activity_empty);
    shark_vm_bind_function(vm, module, type, "event", 3, shark_lib_activity_empty);
    shark_vm_bind_function(vm, module, type, "update", 0, shark_lib_activity_empty);
    shark_vm_bind_function(vm, module, type, "render", 0, shark_lib_activity_empty);
    
    // shark.asset
    module = shark_vm_bind_module(vm, "shark.asset");
    shark_vm_bind_function(vm, module, NULL, "load_texture", 1, shark_lib_load_texture);
    shark_vm_bind_function(vm, module, NULL, "load_font", 3, shark_lib_load_font);
    
    // shark.event
    module = shark_vm_bind_module(vm, "shark.event");
    shark_table_set_index(module->names, SHARK_FROM_PTR(shark_string_new_from_cstr("E_NONE")), SHARK_FROM_NUM(E_NONE));
    shark_table_set_index(module->names, SHARK_FROM_PTR(shark_string_new_from_cstr("E_PRESS")), SHARK_FROM_NUM(E_PRESS));
    shark_table_set_index(module->names, SHARK_FROM_PTR(shark_string_new_from_cstr("E_RELEASE")), SHARK_FROM_NUM(E_RELEASE));
    shark_table_set_index(module->names, SHARK_FROM_PTR(shark_string_new_from_cstr("E_MOVE")), SHARK_FROM_NUM(E_MOVE));
    shark_table_set_index(module->names, SHARK_FROM_PTR(shark_string_new_from_cstr("E_PRESS_UP")), SHARK_FROM_NUM(E_PRESS_UP));
    shark_table_set_index(module->names, SHARK_FROM_PTR(shark_string_new_from_cstr("E_PRESS_DOWN")), SHARK_FROM_NUM(E_PRESS_DOWN));
    shark_table_set_index(module->names, SHARK_FROM_PTR(shark_string_new_from_cstr("E_PRESS_LEFT")), SHARK_FROM_NUM(E_PRESS_LEFT));
    shark_table_set_index(module->names, SHARK_FROM_PTR(shark_string_new_from_cstr("E_PRESS_RIGHT")), SHARK_FROM_NUM(E_PRESS_RIGHT));
    shark_table_set_index(module->names, SHARK_FROM_PTR(shark_string_new_from_cstr("E_PRESS_X")), SHARK_FROM_NUM(E_PRESS_X));
    shark_table_set_index(module->names, SHARK_FROM_PTR(shark_string_new_from_cstr("E_PRESS_Y")), SHARK_FROM_NUM(E_PRESS_Y));
    shark_table_set_index(module->names, SHARK_FROM_PTR(shark_string_new_from_cstr("E_REL_UP")), SHARK_FROM_NUM(E_REL_UP));
    shark_table_set_index(module->names, SHARK_FROM_PTR(shark_string_new_from_cstr("E_REL_DOWN")), SHARK_FROM_NUM(E_REL_DOWN));
    shark_table_set_index(module->names, SHARK_FROM_PTR(shark_string_new_from_cstr("E_REL_LEFT")), SHARK_FROM_NUM(E_REL_LEFT));
    shark_table_set_index(module->names, SHARK_FROM_PTR(shark_string_new_from_cstr("E_REL_RIGHT")), SHARK_FROM_NUM(E_REL_RIGHT));
    shark_table_set_index(module->names, SHARK_FROM_PTR(shark_string_new_from_cstr("E_REL_X")), SHARK_FROM_NUM(E_REL_X));
    shark_table_set_index(module->names, SHARK_FROM_PTR(shark_string_new_from_cstr("E_REL_Y")), SHARK_FROM_NUM(E_REL_Y));
    
    // shark.persistent
    module = shark_vm_bind_module(vm, "shark.persistent");
    shark_vm_bind_function(vm, module, NULL, "get_save_file", 2, shark_lib_get_save_file);
    
    // shark.text
    module = shark_vm_bind_module(vm, "shark.text");
    type = shark_font_class = shark_vm_bind_class(vm, module, "font", sizeof(shark_font), shark_font_destroy, false);
    shark_vm_bind_function(vm, module, type, "get_height", 0, shark_lib_font_get_height);
    shark_vm_bind_function(vm, module, type, "get_width", 1, shark_lib_font_get_width);
    
    // shark.texture
    module = shark_vm_bind_module(vm, "shark.texture");
    type = shark_texture_class = shark_vm_bind_class(vm, module, "texture", sizeof(shark_texture), shark_texture_destroy, false);
    shark_vm_bind_function(vm, module, type, "get_size_x", 0, shark_lib_texture_get_size_x);
    shark_vm_bind_function(vm, module, type, "get_size_y", 0, shark_lib_texture_get_size_y);
}

#ifdef __PSP__
#include <pspkernel.h>
#include <pspctrl.h>
#endif

void shark_exec_game(shark_vm *vm)
{
    if (SDL_Init(SDL_INIT_VIDEO)
    || !IMG_Init(IMG_INIT_PNG)
    || TTF_Init())
        shark_fatal_error(NULL, "couldn't initialize libraries.");
    
#ifdef __PSP__
    sceCtrlSetSamplingCycle(10);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
#endif
    
#ifndef SHARK_DIRECT_RENDER
    SDL_Window *window = NULL;
#endif
    
#ifdef __PSP__
    window = SDL_CreateWindow("SharkGame", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 272, 0);
#else
    window = SDL_CreateWindow("SharkGame", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320 * SHARK_SCALE, 192 * SHARK_SCALE, SDL_WINDOW_SHOWN);
#endif
    
#ifdef SHARK_DIRECT_RENDER
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    #ifdef __PSP__
        SDL_RenderSetScale(renderer, 1.5, 1.42);
    #elif defined(SHARK_SOFT_SCALE)
        SDL_RenderSetScale(renderer, SHARK_SOFT_SCALE, SHARK_SOFT_SCALE);
    #else
        SDL_RenderSetScale(renderer, SHARK_SCALE, SHARK_SCALE);
    #endif
#endif
    
    shark_string *main_name = shark_string_new_from_cstr("main");
    shark_module *module = shark_vm_import_module(vm, main_name);
    
    shark_class *main_class = SHARK_AS_CLASS(shark_table_get_str(module->names, "main_activity"));
    if (main_class == NULL) shark_fatal_error(NULL, "can't load activity class.");
    
    shark_activity *main = shark_activity_new();
    ((shark_object *) main)->type = main_class;
    
#ifndef SHARK_DIRECT_RENDER
    main->window = window;
    main->window_display = SDL_GetWindowSurface(main->window);
    #ifndef SHARK_DIRECT_DRAW
        main->display = SDL_CreateRGBSurface(0, 320, 192, 32, 0, 0, 0, 0);
    #else
        main->display = main->window_display;
    #endif
    if (main->display == NULL) shark_fatal_error(NULL, "could not initialize display.");
#endif
    
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
            shark_vm_execute(vm, NULL, module, method); \
        } else { \
            method->code.native_code(vm, vm->stack, &error); \
        } \
        while (vm->TOS > 0) \
            shark_value_dec_ref(vm->stack[--vm->TOS]); \
    }
    
    PUSH(SHARK_FROM_PTR(main));
    SHARK_CALL_METHOD("launch");
    
    bool pressed = false;
    
    unsigned long max_delta = 1000 / 24;
#ifdef __PSP__
    unsigned long last = 0;
#endif
    
    while (true)
    {
        SDL_Event event;
        
        unsigned long time = SDL_GetTicks();
        
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
        }
#ifdef __PSP__
        SceCtrlData pad;
        
        while (true)
        {
            sceCtrlReadBufferPositive(&pad, 1);
            
            if (pad.Buttons == last)
                break;
            
            if (pad.Buttons & PSP_CTRL_CIRCLE) {
                if (!(last & PSP_CTRL_CIRCLE))
                    DISPATCH_EVENT(E_PRESS_X, 0, 0);
            } else {
                if (last & PSP_CTRL_CIRCLE)
                    DISPATCH_EVENT(E_REL_X, 0, 0);
            }
            
            if (pad.Buttons & PSP_CTRL_CROSS) {
                if (!(last & PSP_CTRL_CROSS))
                    DISPATCH_EVENT(E_PRESS_Y, 0, 0);
            } else {
                if (last & PSP_CTRL_CROSS)
                    DISPATCH_EVENT(E_REL_Y, 0, 0);
            }
            
            if (pad.Buttons & PSP_CTRL_UP) {
                if (!(last & PSP_CTRL_UP))
                    DISPATCH_EVENT(E_PRESS_UP, 0, 0);
            } else {
                if (last & PSP_CTRL_UP)
                    DISPATCH_EVENT(E_REL_UP, 0, 0);
            }
            
            if (pad.Buttons & PSP_CTRL_DOWN) {
                if (!(last & PSP_CTRL_DOWN))
                    DISPATCH_EVENT(E_PRESS_DOWN, 0, 0);
            } else {
                if (last & PSP_CTRL_DOWN)
                    DISPATCH_EVENT(E_REL_DOWN, 0, 0);
            }
            
            if (pad.Buttons & PSP_CTRL_LEFT) {
                if (!(last & PSP_CTRL_LEFT))
                    DISPATCH_EVENT(E_PRESS_LEFT, 0, 0);
            } else {
                if (last & PSP_CTRL_LEFT)
                    DISPATCH_EVENT(E_REL_LEFT, 0, 0);
            }
            
            if (pad.Buttons & PSP_CTRL_RIGHT) {
                if (!(last & PSP_CTRL_RIGHT))
                    DISPATCH_EVENT(E_PRESS_RIGHT, 0, 0);
            } else {
                if (last & PSP_CTRL_RIGHT)
                    DISPATCH_EVENT(E_REL_RIGHT, 0, 0);
            }
            
            last = pad.Buttons;
        }
#endif
#undef DISPATCH_EVENT
#undef DISPATCH_KEY
        
        PUSH(SHARK_FROM_PTR(main));
        SHARK_CALL_METHOD("update");
        
#ifdef SHARK_DIRECT_RENDER
        SDL_RenderClear(renderer);
#endif
        
        PUSH(SHARK_FROM_PTR(main));
        SHARK_CALL_METHOD("render");
        
#ifndef SHARK_DIRECT_RENDER
        main->window_display = SDL_GetWindowSurface(main->window);
#ifdef __PSP__
    #ifndef SHARK_SOFT_RESCALE
        SDL_Rect main_rect = (SDL_Rect) { 80, 40, 320, 192 };
        SDL_BlitSurface(main->display, NULL, main->window_display, &main_rect);
    #endif
#else
        SDL_BlitScaled(main->display, NULL, main->window_display, NULL);
#endif
        SDL_UpdateWindowSurface(main->window);
#else
        SDL_RenderPresent(renderer);
        
        for (size_t i = 0; i < TEXT_RENDER_COUNT; i++)
            SDL_DestroyTexture(TEXT_RENDER_BUFFER[i]);
        
        TEXT_RENDER_COUNT = 0;
#endif
        
        unsigned long delta = SDL_GetTicks() - time;
        
        if (delta >= max_delta)
            continue;
        
        SDL_Delay(max_delta - delta);
    }
#undef PUSH
#undef SHARK_CALL_METHOD
end:
    SDL_Quit();
    IMG_Quit();
    TTF_Quit();
}

#if (defined(_WIN32) || defined(__PSP__)) && !defined(__TINYC__)
int SDL_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
#ifndef __PSP__
    if (argc == 2)
    {
#endif
        shark_vm *vm = shark_vm_new();
#ifdef __PSP__
        shark_string *exec = shark_string_new_from_cstr("game");
        shark_string *base = shark_string_new_from_cstr("./");
#else
        shark_string *exec = shark_string_new_from_cstr(argv[0]);
        shark_string *base = shark_path_get_base(exec);
#endif
        shark_vm_add_import_path(vm, base);
        shark_init_library(vm);
        shark_init_game_library(vm);
#ifdef __PSP__
        shark_string *gamedir = shark_string_new_from_cstr("");
        shark_string *filename = shark_string_new_from_cstr("bin/game.shar");
#else
        shark_string *gamedir = shark_string_new_from_cstr(argv[1]);
        shark_string *tail = shark_string_new_from_cstr("bin/game.shar");
        shark_string *filename = shark_path_join(gamedir, tail);
        shark_object_dec_ref(tail);
#endif
        
        FILE *source = fopen(filename->data, "rb");
        
        if (source == NULL)
        {
            printf("can't open code archive 'bin/game.shar', execution aborted.");
            return -1;
        }
        
#ifdef __PSP__
        asset_dir = shark_string_new_from_cstr("asset");
        save_file_name = shark_string_new_from_cstr("save");
#else
        tail = shark_string_new_from_cstr("asset");
        asset_dir = shark_path_join(gamedir, tail);
        shark_object_dec_ref(tail);
        
        tail = shark_string_new_from_cstr("save");
        save_file_name = shark_path_join(gamedir, tail);
        shark_object_dec_ref(tail);
#endif
        shark_module *module = shark_read_archive(vm, shark_path_get_tail(filename), source);
        fclose(source);
        
        shark_exec_game(vm);
        
        return 0;
#ifndef __PSP__
    }
    else
    {
        printf("usage: %s <game-dir>", argv[0]);
        return -1;
    }
#endif
}

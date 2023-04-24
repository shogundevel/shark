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

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct _shark_type_texture *shark_type_texture;
typedef struct _shark_class_texture shark_class_texture;

struct _shark_type_texture
{
    shark_object __super;
    SDL_Surface *surface;
};

struct _shark_class_texture
{
    shark_object_class __super;
    void (*free)(void *);
    shark_int (*get_size_x)(void *);
    shark_int (*get_size_y)(void *);
};

void shark_lib_texture_free(void *self)
{
    SDL_FreeSurface(((shark_type_texture) self)->surface);
    shark_dealloc(self);
}

shark_int shark_lib_texture_get_size_x(void *self)
{
    return (shark_int) ((shark_type_texture) self)->surface->w;
}

shark_int shark_lib_texture_get_size_y(void *self)
{
    return (shark_int) ((shark_type_texture) self)->surface->h;
}

shark_class_texture shark_impl_texture = {
    { sizeof(struct _shark_type_texture), NULL },
    shark_lib_texture_free,
    shark_lib_texture_get_size_x,
    shark_lib_texture_get_size_y
};

typedef struct _shark_type_font *shark_type_font;
typedef struct _shark_class_font shark_class_font;

struct _shark_type_font
{
    shark_object super;
    TTF_Font *font;
    SDL_Color color;
};

struct _shark_class_font
{
    shark_object_class __super;
    void (*free)(void *);
    void (*get_height)(void *);
};

void shark_lib_font_free(void *self)
{
    TTF_CloseFont(((shark_type_font) self)->font);
    shark_dealloc(self);
}

void shark_lib_font_get_height(void *self)
{
    return (shark_int) TTF_FontHeight(((shark_type_font) self)->font);
}

shark_class_font shark_impl_font = {
    { sizeof(struct _shark_type_font), NULL },
    shark_lib_font_free,
    shark_lib_font_get_height
};

#include <stdio.h>
#include <stdlib.h>

shark_int shark_lib_ERR_ASSET_NOT_FOUND = 1;

static shark_str asset_dir = NULL;

shark_type_texture shark_lib_load_texture(shark_str path)
{
    char *filename = shark_allocate(asset_dir->size + path->size + 1);
    
    memcpy(filename, asset_dir->data, asset_dir->size);
    memcpy(filename + asset_dir->size, path->data, path->size);
    
    filename[asset_dir->size + path->size] = '\0';
    
    SDL_Surface *image = SDL_LoadBMP(filename);
    
    shark_dealloc(filename);
    
    if (image == NULL)
    {
        fprintf(stderr, "fatal error: can't open image '%s'", path->data);
        exit(-1);
    }
    
    shark_type_texture result = shark_object_new(&shark_impl_texture);
    result->surface = image;
    
    return result;
}

shark_type_font shark_lib_load_font(shark_str path, shark_int size, shark_int color)
{
    char *filename = shark_allocate(asset_dir->size + path->size + 1);
    
    memcpy(filename, asset_dir->data, asset_dir->size);
    memcpy(filename + asset_dir->size, path->data, path->size);
    
    filename[asset_dir->size + path->size] = '\0';
    
    TTF_Font *font = TTF_OpenFont(filename, size);
    
    shark_dealloc(filename);
    
    if (font == NULL)
    {
        fprintf(stderr, "fatal error: can't open image '%s'", path->data);
        exit(-1);
    }
    
    shark_type_font result = shark_object_new(&shark_impl_font);
    
    result->font = font;
    result->color = (SDL_Color) {
        (color >> 24 ) & 0xFF, (color >> 16 ) & 0xFF,
        (color >> 8 ) & 0xFF, color & 0xFF };
    
    return result;
}

shark_int shark_lib_E_NONE = 0;
shark_int shark_lib_E_PRESS = 1;
shark_int shark_lib_E_MOVE = 2;
shark_int shark_lib_E_RELEASE = 3;
shark_int shark_lib_E_PRESS_UP = 4;
shark_int shark_lib_E_PRESS_DOWN = 5;
shark_int shark_lib_E_PRESS_LEFT = 6;
shark_int shark_lib_E_PRESS_RIGHT = 7;
shark_int shark_lib_E_PRESS_X = 8;
shark_int shark_lib_E_PRESS_Y = 9;
shark_int shark_lib_E_REL_UP = 10;
shark_int shark_lib_E_REL_DOWN = 11;
shark_int shark_lib_E_REL_LEFT = 12;
shark_int shark_lib_E_REL_RIGHT = 13;
shark_int shark_lib_E_REL_X = 14;
shark_int shark_lib_E_REL_Y = 15;

void shark_lib_launch();
void shark_lib_event(shark_int type, shark_int x, shark_int y);
void shark_lib_update();
void shark_lib_render();

static shark_str save_file_name = NULL;

void *shark_lib_open(shark_str filename, shark_char mode);

void *shark_lib_get_save_file(shark_char mode)
{
    return shark_lib_open(save_file_name, mode);
}

static SDL_Window *window = NULL;
static SDL_Surface *window_display = NULL;
static SDL_Surface *display = NULL;

#define SHARK_SCALE     3

static void shark_game_init()
{
    window = SDL_CreateWindow("SharkGame", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        320 * SHARK_SCALE, 192 * SHARK_SCALE, SDL_WINDOW_SHOWN);
    window_display = SDL_GetWindowSurface(window);
    display = SDL_CreateRGBSurface(0, 320, 192, 32, 0, 0, 0, 0);
    
    if (display == NULL)
    {
        fprintf(stderr, "could not initialize display.");
        exit(EXIT_FAILURE);
    }
}

void shark_lib_draw(shark_type_texture text, shark_int x, shark_int y)
{
    SDL_Rect dest = { x, y, text->surface->w, text->surface->h };
    SDL_BlitSurface(text->surface, NULL, display, &dest);
}

void shark_lib_draw_ex(shark_type_texture text, shark_int x, shark_int y, shark_int origin_x, shark_int origin_y, shark_float rotation, shark_float scale_x, shark_float scale_y)
{
    fprintf(stderr, "shark_game: draw_ex not supported.");
    exit(-1);
/*
    SDL_Surface *render = rotozoomSurfaceXY(text->surface, rotation, scale_x, scale_y, 1);
    
    double r = rotation * M_PI / 180;
    int ox = (int) (cos(r) * origin_x + sin(r) * origin_y) * scale_x;
    int oy = (int) (cos(r) * origin_y + sin(r) * origin_x) * scale_y;
    x = x + ox - render->w / 2;
    y = y + oy - render->h / 2;
    
    SDL_Rect dest = { x, y, render->w, render->h };
    SDL_BlitSurface(render, NULL, display, &dest);
    SDL_FreeSurface(render);
*/
}

void shark_lib_draw_text(shark_str text, shark_type_font font_object, shark_int x, shark_int y)
{
    SDL_Surface *render = TTF_RenderUTF8_Blended(font_object->font, text->data, font_object->color);
    SDL_Rect dest = { x, y, render->w, render->h };
    SDL_BlitSurface(render, NULL, display, &dest);
    SDL_FreeSurface(render);
}

#if defined(_WIN32) && !defined(SDL_MAIN_HANDLED)
int SDL_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
    asset_dir = shark_str_new("asset/");
    save_file_name = shark_str_new("save");
    
    if (SDL_Init(SDL_INIT_VIDEO) || TTF_Init())
    {
        fprintf(stderr, "could not initialize libraries.");
        return -1;
    }
    
    shark_game_init();
    shark_lib_launch();
    
    bool pressed = false;
    
#define event   shark_lib_event
    
    while (true)
    {
        SDL_Event ev;
        
        while (SDL_PollEvent(&ev))
        switch (ev.type)
        {
        case SDL_QUIT:
            goto end;
        case SDL_MOUSEBUTTONDOWN:
            if (ev.button.button == SDL_BUTTON_LEFT)
            {
                event(shark_lib_E_PRESS, ev.button.x / SHARK_SCALE, ev.button.y / SHARK_SCALE);
                pressed = true;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (ev.button.button == SDL_BUTTON_LEFT)
            {
                event(shark_lib_E_RELEASE, ev.button.x / SHARK_SCALE, ev.button.y / SHARK_SCALE);
                pressed = false;
            }
            break;
        case SDL_MOUSEMOTION:
            if (pressed)
                event(shark_lib_E_MOVE, ev.motion.x / SHARK_SCALE, ev.motion.y / SHARK_SCALE);
            break;

#undef event
#define event(x, y, z)  shark_lib_event(x, y, z); break

        case SDL_KEYDOWN: {
            switch (ev.key.keysym.sym)
            {
            case SDLK_UP: event(shark_lib_E_PRESS_UP, 0, 0);
            case SDLK_DOWN: event(shark_lib_E_PRESS_DOWN, 0, 0);
            case SDLK_LEFT: event(shark_lib_E_PRESS_LEFT, 0, 0);
            case SDLK_RIGHT: event(shark_lib_E_PRESS_RIGHT, 0, 0);
            case SDLK_z: event(shark_lib_E_PRESS_X, 0, 0);
            case SDLK_x: event(shark_lib_E_PRESS_Y, 0, 0);
            }
            break;
        }
        case SDL_KEYUP:
            switch (ev.key.keysym.sym)
            {
            case SDLK_UP: event(shark_lib_E_REL_UP, 0, 0);
            case SDLK_DOWN: event(shark_lib_E_REL_DOWN, 0, 0);
            case SDLK_LEFT: event(shark_lib_E_REL_LEFT, 0, 0);
            case SDLK_RIGHT: event(shark_lib_E_REL_RIGHT, 0, 0);
            case SDLK_z: event(shark_lib_E_REL_X, 0, 0);
            case SDLK_x: event(shark_lib_E_REL_Y, 0, 0);
            }
            break;
        default:
            break;
        }
        
#undef event
        
        shark_lib_update();
        shark_lib_render();
        
        window_display = SDL_GetWindowSurface(window);
        SDL_BlitScaled(display, NULL, window_display, NULL);
        
        SDL_UpdateWindowSurface(window);
        SDL_Delay(1000 / 24);
    }
end:
    SDL_Quit();
    TTF_Quit();
    
    return 0;
}

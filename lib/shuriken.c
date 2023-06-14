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

#include "SDL.h"
#include "SDL_ttf.h"

// shuriken.render


typedef struct _shark_type_render_t *shark_type_render_t;
typedef struct _shark_class_render_t shark_class_render_t;

struct _shark_type_render_t
{
    shark_object __super;
    SDL_Surface *surface;
};

struct _shark_class_render_t
{
    shark_object_class __super;
    void (*free)(void *);
    void (*draw)(void *, shark_type_render_t, shark_int, shark_int);
    void (*draw_point)(void *, shark_int, shark_int, shark_int);
    shark_int (*get_size_x)(void *);
    shark_int (*get_size_y)(void *);
    void (*get_point)(void *, shark_int, shark_int);
};

void shark_lib_render_t_free(void *self)
{
    SDL_FreeSurface(((shark_type_render_t) self)->surface);
    shark_dealloc(self);
}

void shark_lib_render_t_draw(void *self, shark_type_render_t render_y, shark_int x, shark_int y)
{
    shark_type_render_t render_x = self;
    SDL_Rect dest = { x, y, render_y->surface->w, render_y->surface->h };
    SDL_BlitSurface(render_y->surface, NULL, render_x->surface, &dest);
}

void shark_lib_render_t_draw_point(void *self, shark_int color, shark_int x, shark_int y)
{
    shark_type_render_t render_x = self;
    SDL_Rect dest = { x, y, 1, 1 };
    SDL_FillRect(render_surf, &dest, (Uint32) (color >> 8 | (color & 0xFF) << 24));
}

shark_int shark_lib_render_t_get_size_x(void *self)
{
    return ((shark_type_render_t self)->surface->w);
}

shark_int shark_lib_render_t_get_size_y(void *self)
{
    return ((shark_type_render_t self)->surface->h);
}

shark_int shark_lib_render_t_get_point(void *self, shark_int x, shark_int y)
{
    SDL_Surface surf = ((shark_type_render_t) self)->surface;
    return ((Uint32) surf->pixels + y * surf->pitch)[x];
}

shark_class_render_t shark_impl_render_t = {
    { sizeof(struct _shark_type_render_t), NULL },
    shark_lib_render_t_free,
    shark_lib_render_t_draw,
    shark_lib_render_t_draw_point,
    shark_lib_render_t_get_size_x,
    shark_lib_render_t_get_size_y,
    shark_lib_render_t_get_point,
};


typedef struct _shark_type_font *shark_type_font;
typedef struct _shark_class_font shark_class_font;

struct _shark_type_font_t
{
    shark_object super;
    TTF_Font *font;
};

struct _shark_class_font_t
{
    shark_object_class __super;
    void (*free)(void *);
    shark_int (*get_height)(void *);
    shark_int (*get_size)(void *, shark_str)
};

void shark_lib_font_t_free(void *self)
{
    TTF_CloseFont(((shark_type_font_t) self)->font);
    shark_dealloc(self);
}

shark_int shark_lib_font_t_get_height(void *self)
{
    return (shark_int) TTF_FontHeight(((shark_type_font) self)->font);
}

shark_int shark_lib_font_t_get_size(void *self, shark_str data)
{
    int height = 0, width = 0;
    TTF_SizeUTF8(((shark_type_font_t) self)->font,
        (char *) data->data, &height, &width);
    return (shark_int) width;
}

shark_class_font_t shark_impl_font_t = {
    { sizeof(struct _shark_type_font_t), NULL },
    shark_lib_font_t_free,
    shark_lib_font_t_get_height,
    shark_lib_font_t_get_size
};

shark_int shark_lib_color(shark_int r, shark_int g, shark_int b, shark_int a)
{
    return (r << 24) | (g << 16) | (b << 8) | a;
}

shark_type_render_t shark_lib_render_solid(shark_int color_code, shark_int size_x, shark_int size_y)
{
    SDL_Surface *render_surf = SDL_CreateRGBSurface(0, size_x, size_y, 32, 0, 0, 0, 0);
    
    if (render_surf == NULL)
    {
        fprintf(stderr, "could not render solid: %s", SDL_GetError());
        exit(-1);
    }
    
    SDL_FillRect(render_surf, NULL, (Uint32) (color_code >> 8 | (color_code & 0xFF) << 24));
    shark_type_render_t *render = shark_object_new(shark_class_render_t);
    render->surface = render_surf;
    return render;
}

shark_type_render_t shark_lib_render_text(shark_str data, shark_type_font_t font, shark_int color_code)
{
    SDL_Color color = (SDL_Color) {
        (color_code >> 24 ) & 0xFF, (color_code >> 16 ) & 0xFF,
        (color_code >> 8 ) & 0xFF, color_code & 0xFF };
    
    SDL_Surface *render_text = TTF_RenderUTF8_Blended(font->font, data->data, color);
    
    if (render_text == NULL)
        render_text = SDL_CreateRGBSurface(0, 0, 0, 32, 0, 0, 0, 0);
    
    shark_type_render_t render = shark_object_new(shark_class_render_t);
    render->surface = render_text;
    
    return render;
}

shark_type_render_t shark_lib_load_image(shark_str path)
{
    SDL_Surface *image = SDL_LoadBMP(path->data);
    
    if (image == NULL) return NULL;
    
    shark_type_render_t result = shark_object_new(shark_class_render_t);
    result->surface = image;
    
    return result;
}

shark_type_font_t shark_lib_load_font(shark_str path, shark_int size)
{
    TTF_Font *font = TTF_OpenFont(path->data, (int) size);
    
    if (font == NULL) return NULL;
    
    shark_type_font_t result = shark_object_new(shark_class_font_t);
    result->font = font;
    
    return result;
}

SDL_Window *window;
SDL_Surface *window_display;
SDL_Surface *display;

void shark_fx_init(int size_x, int size_y, char *name, int scale)
{
    window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, size_x * scale, size_y * scale, SDL_WINDOW_SHOWN);
    window_display = SDL_GetWindowSurface(window);
    display = SDL_CreateRGBSurface(0, size_x, size_y, 32, 0, 0, 0, 0);
    
    if (display == NULL)
    {
        fprintf(stderr, "could not create display: %s", SDL_GetError());
        exit(-1);
    }
}

void shark_lib_draw(shark_type_render_t render, shark_int x, shark_int y)
{
    SDL_Rect dest = { x, y, render->surface->w, render->surface->h };
    SDL_BlitSurface(render->surface, NULL, display, &dest);
}

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

int SIZE_X = 640;
int SIZE_Y = 480;
int SCALE = 1;
char *NAME = "Shuriken";

void shark_lib_launch(shark_int size_x, shark_int size_y);
void shark_lib_event(shark_int type, shark_int x, shark_int y, shark_char c);
void shark_lib_update();
void shark_lib_render();

void shark_fx_exec()
{
    if (SDL_Init(SDL_INIT_VIDEO) || TTF_Init())
    {
        fprintf(stderr, "couldn't initialize libraries.");
        exit(-1);
    }
    
    shark_fx_init(SIZE_X, SIZE_Y, NAME, SCALE);
    shark_lib_launch(SIZE_X, SIZE_Y);
    
    bool shift_down = false;
    
    while (true)
    {
        SDL_Event event;
        
#define DISPATCH_EVENT(type, x, y) \
    shark_lib_event(type, x, y, '\0')
        
#define DISPATCH_KEY(type) \
    DISPATCH_EVENT(type, 0, 0)
        
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
#define DISPATCH_TEXT(type, char) \
    shark_lib_event(type, 0, 0, char)
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
}

int main(int argc, char **argv)
{
    if (argc >= 4)
    {
        SIZE_X = atoi(argv[2]);
        SIZE_Y = atoi(argv[3]);
        
        if (argc >= 5) SCALE = atoi(argv[4]);
        if (argc >= 6) NAME = argv[5];
    }
    
    shark_fx_exec();
    return 0;
}

################################################################################
### Copyright ##################################################################
## 
## Copyright 2022 Daniel Alvarez <shogundevel@gmail.com>
## 
## Permission is hereby granted, free of charge, to any person
## obtaining a copy of this software and associated documentation files
## (the "Software"), to deal in the Software without restriction,
## including without limitation the rights to use, copy, modify, merge,
## publish, distribute, sublicense, and/or sell copies of the Software,
## and to permit persons to whom the Software is furnished to do so,
## subject to the following conditions:
## 
## The above copyright notice and this permission notice shall be
## included in all copies or substantial portions of the Software.
## 
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
## EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
## MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
## IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
## CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
## TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
## SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
## 
################################################################################

init -1500 python:
    EXIT_SUCCESS = 0
    EXIT_FAILURE = -1

    def exit(code):
        import sys
        sys.exit(code)

    ERR_NONE = 0
    ERR_UNKNOWN = 1

    ERR = ERR_NONE

    def get_err():
        return ERR

    def set_err(value):
        global ERR
        ERR = value

    def clear_err():
        set_err(ERR_NONE)

    ERR_INVALID_LITERAL = 2

    def stoi(x):
        try:
            return int(x)
        except ValueError:
            set_err(ERR_INVALID_LITERAL)
            return 0

    def stof(x):
        try:
            return float(x)
        except ValueError:
            set_err(ERR_INVALID_LITERAL)
            return 0.0

    def itos(x):
        return str(x)

    def ftos(x):
        return str(x)

    def ctoi(x):
        raise NotImplementedError()

    def itoc(x):
        raise NotImplementedError()

    def islower(x):
        return x >= 'a' and x <= 'z'

    def isupper(x):
        return x >= 'A' and x <= 'Z'

    def tolower(x):
        return x.tolower()

    def toupper(x):
        return x.toupper()

    def isalpha(x):
        return islower(x) or isupper(x)

    def isdigit(x):
        return x >= '0' and x <= '9'

    def isalnum(x):
        return isalpha(x) or isdigit(x)

    def isident(x):
        return isalnum(x) or x == '_'

    class strbuf:
        def __init__(self):
            self.buffer = ""
        
        def put(self, value):
            self.buffer += value
        
        def write(self, data):
            self.buffer += data
        
        def get(self):
            data = self.buffer
            self.buffer = ""
            return data

    class striter:
        def __init__(self, data):
            self.data = data
            self.pos = 0
        
        def tell(self):
            return self.pos
        
        def seek(self, pos):
            self.pos = pos
        
        def next(self):
            c = self.data[self.pos]
            self.pos += 1
            return c
        
        def empty(self):
            return self.pos >= len(self.data)

    class strmap:
        def __init__(self):
            self.data = { }
        
        def get(self, index):
            return self.data[index]
        
        def set(self, index, value):
            self.data[index] = value
        
        def has(self, index):
            return index in self.data
        
        def pop(self, index):
            return self.data.pop(index)

    ERR_FILE_NOT_FOUND = 3

    class FILE:
        def __init__(self, stream):
            self.stream = stream
        
        def close(self):
            self.stream.close()
        
        def at_end(self):
            current = self.stream.tell()
            end = not self.stream.read(1)
            self.stream.seek(current)
            return end
        
        def put(self, value):
            self.stream.write(value)
        
        def write(self, data):
            self.stream.write(data)
        
        def fetch(self):
            return self.stream.read(1) or '\0'

    fopen = open

    def open(filename, mode):
        try:
            return FILE(fopen(filename, mode))
        except IOError:
            set_err(ERR_FILE_NOT_FOUND)
            return None

    def write(data):
        print(data)

    def read():
        return input()

    class list (list):
        def size(self):
            return len(self)
        
        def push(self, value):
            self.append(value)
        
        def get(self, index):
            return self[index]
        
        def set(self, index, value):
            self[index] = value

    import math

    def idiv(x, y):
        return x // y

    pi = math.pi
    e = math.e

    acos = math.acos
    asin = math.asin
    atan = math.atan
    atan2 = math.atan2

    cos = math.cos
    cosh = math.cosh
    sin = math.sin
    sinh = math.sinh
    tan = math.tan
    tanh = math.tanh

    exp = math.exp
    log = math.log
    log10 = math.log10
    pow = math.pow
    sqrt = math.sqrt

    def ceil(x):
        return int(math.ceil(x))

    def floor(x):
        return int(math.floor(x))

    import random as prandom

    def random(x):
        return prandom.randint(0, x - 1)

    import os

    def listdir(path):
        try:
            return os.listdir(path)
        except:
            pass

    def mkdir(path):
        try:
            os.mkdir(path)
            return True
        except:
            return False

    def rmdir(path):
        try:
            os.rmdir(path)
            return True
        except:
            return False

    def unlink(path):
        try:
            os.unlink(path)
            return True
        except:
            return False

    import time

    clock = time.clock

    def free(value):
        pass

    free_strbuf = free
    free_striter = free
    free_strmap = free
    free_list = free

    def ctext(text, font, size, color):
        return Text(text, font=font, size=size, color=color)
    
    class render_object (renpy.Displayable):
        def __init__(self, surf):
            renpy.Displayable.__init__(self)
            self.surf = surf
        
        def render(self, xs, ys, st, at):
            return self.surf
    
    class texture:
        def __init__(self, image_object):
            self.image_object = image_object
            self.render = None
        
        def get_render(self):
            if self.render is None:
                self.render = self.image_object.render(1024, 1024, 0, 0)
            return self.render
        
        def get_size_x(self):
            return self.render.get_size()[0]
        
        def get_size_y(self):
            return self.render.get_size()[1]
    
    class font:
        def __init__(self, font, size, color):
            self.font = font
            self.size = size
            self.color = Color((
                floor(color / (0xFFFFFF + 1)) % (0xFF + 1),
                floor(color / (0xFFFF + 1)) % (0xFF + 1),
                floor(color / (0xFF + 1)) % (0xFF + 1),
                color % (0xFF + 1)
            ))
        
        def get_height(self):
            return self.size
    
    def get_save_file(mode):
        return None
    
    E_NONE = 0
    E_PRESS = 1
    E_MOVE = 2
    E_RELEASE = 3
    E_PRESS_UP = 4
    E_PRESS_DOWN = 5
    E_PRESS_LEFT = 6
    E_PRESS_RIGHT = 7
    E_PRESS_X = 8
    E_PRESS_Y = 9
    E_REL_UP = 10
    E_REL_DOWN = 11
    E_REL_LEFT = 12
    E_REL_RIGHT = 13
    E_REL_X = 14
    E_REL_Y = 15
    
    ERR_ASSET_NOT_FOUND = 1000
    __ASSET_PATH__ = "asset"
    
    def load_texture(name):
        image_object = Image(__ASSET_PATH__ + '/' + name)
        return texture (image_object)
    
    def load_font(name, size, color):
        return font (__ASSET_PATH__ + '/' + name, size, color)
    
    def cscale(text, size_x, size_y):
        x_size, y_size = text.get_size()
        transform_object = Transform(xzoom=size_x / x_size, yzoom=size_y / y_size)
        object = transform_object(render_object(text))
        return object.render(1024, 1024, 0, 0)
    
    def crotate(text, rot):
        transform_object = Transform(rotate=rot)
        object = transform_object(render_object(text))
        return object.render(1024, 1024, 0, 0)
    
    def crotozoom(text, rot, scale):
        transform_object = Transform(rotate=rot, zoom=scale)
        object = transform_object(render_object(text))
        return object.render(1024, 1024, 0, 0)
    
    def cflip(text, flip_x, flip_y):
        transform_object = Transform(xzoom=1.0 if not flip_x else -1.0, yzoom=1.0 if not flip_y else -1.0)
        object = transform_object(render_object(text))
        return object.render(1024, 1024, 0, 0)
    
    def ctransform(text, rot, scale_x, scale_y):
        transform_object = Transform(rotate=-rot, xzoom=scale_x if scale_x >= 0 else -scale_x, yzoom=scale_y if scale_y >= 0 else -scale_y)
        object = transform_object(render_object(text))
        return object.render(1024, 1024, 0, 0)
    
    def draw(text, x, y):
        display.display.blit(text.get_render(), (x, y))
    
    import math
    
    def draw_ex(text, x, y, origin_x, origin_y, rotation, scale_x, scale_y):
        t = ctransform(text.get_render(), rotation, scale_x, scale_y)
        r = math.radians(rotation)
        ox = (math.cos(r) * origin_x + math.sin(r) * origin_y) * scale_x
        oy = (math.cos(r) * origin_y - math.sin(r) * origin_x) * scale_y
        x = x + ox - t.get_size()[0] / 2
        y = y + oy - t.get_size()[1] / 2
        display.display.blit(t, (x, y))
    
    def draw_text(text, font, x, y):
        display.display.blit(ctext(text, font.font, font.size, font.color).render(320, 192, 0, 0), (x, y))
    
    import pygame
    
    class main_display (renpy.Displayable):
        def __init__(self):
            renpy.Displayable.__init__(self)
            self.pressed = False
            self.x_size = 320
            self.y_size = 192
            self.surface = None
            self.clock = pygame.time.Clock()
            launch()
        
        def render(self, x_size, y_size, time, anim):
            self.x_size = x_size
            self.y_size = y_size
            update()
            self.display = renpy.Render(320, 192)
            render()
            self.surface = cscale(self.display, x_size, y_size)
            self.clock.tick(24)
            renpy.redraw(self, 0)
            return self.surface
        
        def key_event(self, ev):
            event(ev, 0, 0)
    
    def key_event(dis, key):
        def callable():
            dis.key_event(key)
        return callable

init -500 python:
    display = main_display()

screen main_screen ():
    add display
    
    if renpy.variant("pc"):
        key "keydown_K_z" action key_event(display, 8)
        key "keydown_K_x" action key_event(display, 9)
        
        key "keyup_K_z" action key_event(display, 14)
        key "keyup_K_x" action key_event(display, 15)
    else:
        key "keydown_K_x" action key_event(display, 8)
        key "keydown_K_y" action key_event(display, 9)
        
        key "keyup_K_x" action key_event(display, 14)
        key "keyup_K_y" action key_event(display, 15)
    
    key "keydown_K_UP" action key_event(display, 4)
    key "keydown_K_DOWN" action key_event(display, 5)
    key "keydown_K_LEFT" action key_event(display, 6)
    key "keydown_K_RIGHT" action key_event(display, 7)
    
    key "keyup_K_UP" action key_event(display, 10)
    key "keyup_K_DOWN" action key_event(display, 11)
    key "keyup_K_LEFT" action key_event(display, 12)
    key "keyup_K_RIGHT" action key_event(display, 13)
    
    key "K_ESCAPE" action Return(None)

label start:
    call screen main_screen()

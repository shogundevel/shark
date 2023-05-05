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

init -1000 python:
    SIZE_X = 640
    SIZE_Y = 480
    SCALE = 1
    
    import importlib
    
    class shark_module:
        pass
    
    shark_imports = { }
    shark_modules = { }
    
    def shark_import_module(import_path):
        if import_path in shark_imports:
            return shark_imports[import_path]
        elif import_path in shark_modules:
            return shark_modules[import_path]()
        else:
            return importlib.import_module(import_path)
    
    class shark_error_t (Exception):
        def __init__(self, message):
            Exception.__init__(self, message)
            self.message = message
    
    class shark_interop (object):
        pass
    
    def shark_error(message):
        raise shark_error_t (message)
    
    def shark_pcall(callee, args):
        try:
            return callee(*args)
        except shark_error_t as err:
            shark_interop.set_err(shark_interop.ERR_UNKNOWN)
            return err.message
    
    def fopen(filename, mode):
        try:
            return open(filename, mode)
        except Exception:
            return None
    
    def cprint(data):
        print(data)
    
    def ppcall(callee, args):
        try:
            return callee(*args)
        except:
            return None
    
    def pslice(data, start, end):
        return data[start:end]
    
    def pformat(x, y):
        r = ''
        e = False
        i = 0
        for c in x:
            if c == '\\':
                if e:
                    r += '\\'
                else:
                    e = True
            elif c == '%':
                if e:
                    r += '%'
                    e = False
                elif i >= len(y):
                    r += '%'
                else:
                    r += str(y[i])
                    i += 1
            else:
                if e:
                    r += '\\'
                    e = False
                r += c
        if e:
            r += '\\'
        return r
    
    class strbuf:
        def init(self):
            self.data = ""
        
        def __init__(self):
            strbuf.init(self)
        
        def size(self):
            return len(self.data)
        
        def put(self, value):
            self.data += value
        
        def puts(self, data):
            self.put(data)
        
        def printf(self, data, args):
            self.put(format(data, args))
        
        def fetch(self):
            __local_0__ = self.data[0:1]
            self.data = self.data[1:len(self.data)]
            return __local_0__
        
        def read(self, count):
            __local_0__ = self.data[0:count]
            self.data = self.data[count:len(self.data)]
            return __local_0__
        
        def read_all(self):
            __local_0__ = self.data
            self.data = ""
            return __local_0__
        
        def at_end(self):
            return len(self.data) == 0
    
    class cbytes:
        def __init__(self):
            self.buffer = bytearray()
        
        def put(self, value):
            self.buffer.append(value)
        
        def put_short(self, value):
            self.buffer.append(value & 0xFF)
            self.buffer.append((value >> 8) & 0xFF)
        
        def put_int(self, value):
            self.buffer.append(value & 0xFF)
            self.buffer.append((value >> 8) & 0xFF)
            self.buffer.append((value >> 16) & 0xFF)
            self.buffer.append((value >> 24) & 0xFF)
        
        def puts(self, data):
            self.buffer.extend(data.buffer)
        
        def tell(self):
            return len(self.buffer)
        
        def patch(self, pos, value):
            self.buffer[pos] = value
        
        def patch_short(self, pos, value):
            self.buffer[pos] = value & 0xFF
            self.buffer[pos+1] = (value >> 8) & 0xFF
        
        def patch_int(self, pos, value):
            self.buffer[pos] = value & 0xFF
            self.buffer[pos+1] = (value >> 8) & 0xFF
            self.buffer[pos+2] = (value >> 16) & 0xFF
            self.buffer[pos+3] = (value >> 24) & 0xFF
        
        def get(self, index):
            return self.buffer[index]
    
    def ctext(text, font, size, color):
        return Text(text, font=font, size=size, color=color)
    
    class render_object (renpy.Displayable):
        def __init__(self, surf):
            renpy.Displayable.__init__(self)
            self.surf = surf
        
        def render(self, xs, ys, st, at):
            return self.surf
    
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
    
    import pygame
    
    class main_display (renpy.Displayable):
        def __init__(self, child):
            renpy.Displayable.__init__(self)
            self.child = child
            self.pressed = False
            self.x_size = SIZE_X
            self.y_size = SIZE_Y
            self.surface = None
            self.clock = pygame.time.Clock()
            child.launch(SIZE_X, SIZE_Y)
        
        def render(self, x_size, y_size, time, anim):
            self.x_size = x_size
            self.y_size = y_size
            self.child.update()
            self.child.display = renpy.Render(SIZE_X, SIZE_Y)
            self.child.render()
            self.surface = cscale(self.child.display, x_size, y_size)
            self.clock.tick(24)
            renpy.redraw(self, 0)
            return self.surface
        
        def key_event(self, event, char):
            self.child.event(event, 0, 0, char)
        
        def event(self, event, x, y, time):
            import pygame
            if event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
                self.child.event(1, x, y, None)
            elif event.type == pygame.MOUSEBUTTONUP and event.button == 1:
                self.child.event(2, x, y, None)
            elif event.type == pygame.MOUSEMOTION:
                self.child.event(3, x, y, None)
            else:
                raise renpy.IgnoreEvent()
    
    char_set = "abcdefghijklmnopqrstuvwxyz"
    char_digit = "0123456789"
    char_special = " ()[]*/%+-<>=!&|^~.,;:\"'#"
    
    def key_event(dis, key):
        def callable():
            dis.key_event(key, None)
        return callable

screen main_screen (child):
    $ display = main_display(child)
    $ insert_symbol = lambda e, x: lambda: (e.key_event(20, x), e.key_event(21, x))[0]
    
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
    
    key "keydown_K_UP" action key_event(display, 6)
    key "keydown_K_DOWN" action key_event(display, 7)
    key "keydown_K_LEFT" action key_event(display, 8)
    key "keydown_K_RIGHT" action key_event(display, 9)
    key "keydown_K_RETURN" action key_event(display, 10)
    key "keydown_K_BACKSPACE" action key_event(display, 11)
    key "keydown_K_LCTRL" action key_event(display, 12)
    
    key "keyup_K_UP" action key_event(display, 13)
    key "keyup_K_DOWN" action key_event(display, 14)
    key "keyup_K_LEFT" action key_event(display, 15)
    key "keyup_K_RIGHT" action key_event(display, 16)
    key "keyup_K_RETURN" action key_event(display, 17)
    key "keyup_K_BACKSPACE" action key_event(display, 18)
    key "keyup_K_LCTRL" action key_event(display, 19)
    
    for char in char_set:
        key char action insert_symbol(display, char) capture True
        key char.upper() action insert_symbol(display, char.upper()) capture True
    
    for char in char_digit + char_special:
        key char action insert_symbol(display, char) capture True
    
    key "K_ESCAPE" action Return(None)

label start:
    $ main = shark_import_module(__MAIN__)
    
    call screen main_screen(main.main_activity())

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

import pygame
from pygame import Color
from pygame.locals import *

import studio.asset as asset
import studio.core as core
from studio.core import *

import os

project_dir = os.path.join(os.path.split(__file__)[0], "projects")

icon_save = asset.load_image("icon_0001.png")
icon_new = asset.load_image("icon_0003.png")
icon_close = asset.load_image("icon_0004.png")
icon_prev = asset.load_image("icon_0005.png")
icon_next = asset.load_image("icon_0006.png")
icon_more = asset.load_image("icon_0007.png")
icon_doc = asset.load_image("icon_0008.png")
icon_dir = asset.load_image("icon_0009.png")
icon_open = asset.load_image("icon_0010.png")
icon_reload = asset.load_image("icon_0011.png")
icon_delete = asset.load_image("icon_0012.png")
icon_accept = asset.load_image("icon_0013.png")
icon_hover = asset.load_image("icon_0014.png")
icon_console = asset.load_image("icon_0015.png")
icon_gameshark = asset.load_image("icon_0016.png")
icon_library = asset.load_image("icon_0017.png")
icon_run = asset.load_image("icon_0018.png")
icon_build = asset.load_image("icon_0019.png")

class Solid (UIObject):
    def __init__(self, x, y, x_size, y_size, color):
        self.x = x
        self.y = y
        self.surf = pygame.Surface((x_size, y_size))
        self.surf.fill(color)
    
    def get_size(self):
        return self.surf.size()
    
    def render(self, screen):
        screen.blit(self.surf, (self.x, self.y))

class Callable (UIObject):
    def __init__(self, parent):
        self.parent = parent
        self.parent.add_child(self)
        self.returned = False
    
    def return_value(self, value):
        self.returned = True
        self.__rv = value
        self.parent.children.remove(self)
        self.parent.update()
    
    def get_value(self):
        if self.returned:
            return self.__rv
        else:
            return None

class MonospaceTextStyle:
    def __init__(self, font_name, font_size):
        self.font_name = font_name
        self.font_size = font_size
        self.font = asset.load_font(font_name, font_size)
        self.sample = self.font.render(" ", 0, (0, 0, 0, 0))
        self.line_height = self.sample.get_height()
        line_sample = self.font.render(" " * 128, 0, (0, 0, 0, 0))
        line_width = line_sample.get_width()
        self.char_width = line_width / 128

class CodeEditorStyle:
    font = "courier_new.ttf"
    font_size = 14
    font_color = (0, 0, 0)
    keyword_color = (0, 0, 255)
    number_color = (255, 128, 0)
    string_color = (128, 128, 128)
    operator_color = font_color
    comment_color = string_color
    error_color = (255, 0, 0)
    bg_color = (255, 255, 255)
    lineno_bg_color = (230, 230, 230)
    current_line_bg_color = lineno_bg_color
    cursor_color = (0, 0, 0)
    window_color = (0, 0, 0, 128)
    
    def __init__(self, init = True):
        self.__init = False
        if init: self.init()
    
    def init(self):
        if not self.__init:
            self.text_style = MonospaceTextStyle(self.font, self.font_size)
            self.__init = True

class DarkCodeEditorStyle (CodeEditorStyle):
    font = "courier_new.ttf"
    font_size = 14
    font_color = (245, 245, 245)
    keyword_color = (167, 219, 119)
    number_color = (255, 225, 54)
    string_color = (255, 138, 20)
    operator_color = font_color
    comment_color = (149, 162, 170)
    error_color = (255, 0, 0)
    bg_color = (41, 49, 52)
    lineno_bg_color = (63, 75, 78)
    current_line_bg_color = lineno_bg_color
    cursor_color = (255, 255, 255)
    window_color = (0, 0, 0, 192)

NULL_KEYS = { K_BACKSPACE, K_TAB, K_CLEAR, K_RETURN, K_PAUSE,
              K_ESCAPE, K_DELETE, K_UP, K_DOWN, K_RIGHT, K_LEFT,
              K_INSERT, K_HOME, K_END, K_PAGEUP, K_PAGEDOWN,
              K_F1, K_F2, K_F3, K_F4, K_F5, K_F6, K_F7, K_F8, K_F9,
              K_F10, K_F11, K_F12, K_F13, K_F14, K_F15, K_NUMLOCK,
              K_CAPSLOCK, K_SCROLLOCK, K_RSHIFT, K_LSHIFT, K_RCTRL,
              K_LCTRL, K_RALT, K_LALT, K_RMETA, K_LMETA, K_LSUPER,
              K_RSUPER, K_MODE, K_HELP, K_PRINT, K_SYSREQ, K_BREAK,
              K_MENU, K_POWER, K_EURO }

class TextInput (Callable):
    def __init__(self, x, y, x_size, y_size, parent, prompt, default, style):
        Callable.__init__(self, parent)
        self.style = style
        self.prompt_text = style.text_style.font.render(prompt, 1, style.font_color)
        self.data = default
        self.x_size = min(x_size, max(self.prompt_text.get_width() + 100, int(style.text_style.char_width * 12 + 100)))
        self.y_size = style.text_style.line_height * 2 + 80
        self.x = max(x, x + (x_size // 2 - self.x_size // 2))
        self.y = max(y, y + (y_size // 2 - self.y_size // 2))
        self.surf = pygame.Surface((self.x_size, self.y_size), SRCALPHA)
        self.width = int((self.x_size - 100) / style.text_style.char_width)
        self.dirty = True
        self.locked = False
    
    def get_size(self):
        return self.x_size, self.y_size
    
    def event(self, event):
        if event.type == KEYDOWN:
            if event.key == K_RETURN:
                self.return_value(self.data)
            elif event.key == K_ESCAPE:
                self.return_value(None)
            elif event.key == K_BACKSPACE:
                if not self.locked:
                    if self.data:
                        self.data = self.data[:-1]
                    self.dirty = True
            elif event.key in NULL_KEYS or event.mod & KMOD_CTRL:
                pass
            elif event.unicode:
                if not self.locked:
                    self.data += event.unicode
                    self.dirty = True
        return True
    
    def render(self, screen):
        if self.dirty:
            self.surf.fill(self.style.window_color)
            self.surf.blit(self.prompt_text, (50, 40))
            text = self.style.text_style.font.render(
                self.data[max(0, len(self.data) - self.width):],
                1, self.style.keyword_color)
            self.surf.blit(text, (50, 40 + self.style.text_style.line_height))
            self.dirty = False
        screen.blit(self.surf, (self.x, self.y))

import os

class FileBrowser (Callable):
    def __init__(self, x, y, x_size, y_size, parent, prompt, default, root, style):
        Callable.__init__(self, parent)
        self.style = style
        self.prompt_text = style.text_style.font.render(prompt, 1, style.font_color)
        self.x_size = x_size - 100
        self.y_size = y_size - 80
        self.x = x + 50
        self.y = y + 40
        self.surf = pygame.Surface((self.x_size, self.y_size), SRCALPHA)
        self.data = default
        self.width = int((self.x_size - 100) / style.text_style.char_width)
        self.text_input_bg = pygame.Surface((self.x_size - 100, style.text_style.line_height))
        self.text_input_bg.fill(style.current_line_bg_color)
        self.file_browser_x_size = self.x_size - 140
        self.file_browser_y_size = self.y_size - 120 - style.text_style.line_height * 2
        self.file_browser_x = self.x + 70
        self.file_browser_y = self.y + 60 + style.text_style.line_height
        self.file_browser_item_height = max(icon_dir.get_height(), style.text_style.line_height)
        self.file_browser_item_count = self.file_browser_y_size // self.file_browser_item_height
        self.file_browser_y_offset = 0
        self.root = root
        self.configure(True, False)
        self.file_browser_surf = pygame.Surface((self.file_browser_x_size, self.file_browser_y_size), SRCALPHA)
        self.file_browser_dirty = True
        self.dirty = True
    
    def configure(self, lock_root, select_dir):
        self.lock_root = lock_root
        self.select_dir = select_dir
        self.set_current_dir(self.root)
    
    def set_current_dir(self, dir):
        self.current_dir = dir
        self.current_dir_list = os.listdir(self.current_dir)
        if self.select_dir:
            self.current_dir_list = [ dir for dir in self.current_dir_list if os.path.isdir(os.path.join(self.current_dir, dir)) ]
        if not (self.lock_root and self.current_dir == self.root):
            self.current_dir_list.insert(0, "<...>")
        self.file_browser_y_offset = 0
        self.file_browser_dirty = True
        self.dirty = True
    
    def get_size(self):
        return self.x_size, self.y_size
    
    def event(self, event):
        if event.type == MOUSEBUTTONDOWN:
            x, y = event.pos
            if (event.button == 1
            and x >= self.file_browser_x and x < self.file_browser_x + self.file_browser_x_size
            and y >= self.file_browser_y and y < self.file_browser_y + self.file_browser_y_size):
                file = self.file_browser_y_offset + (y - self.file_browser_y) // self.file_browser_item_height
                if file < len(self.current_dir_list):
                    filename = self.current_dir_list[file]
                    if filename == "<...>":
                        self.set_current_dir(os.path.split(self.current_dir)[0])
                        return True
                    path = os.path.join(self.current_dir, filename)
                    if os.path.isdir(path):
                        if self.select_dir:
                            self.return_value(path)
                        else:
                            self.set_current_dir(path)
                    else:
                        self.return_value(path)
            elif event.button == 4:
                self.file_browser_y_offset = max(0, self.file_browser_y_offset - 3)
                self.file_browser_dirty = True
                self.dirty = True
            elif event.button == 5:
                self.file_browser_y_offset = max(0, min(len(self.current_dir_list) - self.file_browser_item_count, self.file_browser_y_offset + 3))
                self.file_browser_dirty = True
                self.dirty = True
        elif event.type == KEYDOWN:
            if event.key == K_RETURN:
                self.return_value(os.path.join(self.current_dir, self.data))
            elif event.key == K_ESCAPE:
                self.return_value(None)
            elif event.key == K_BACKSPACE:
                if self.data:
                    self.data = self.data[:-1]
                self.dirty = True
            elif event.key in NULL_KEYS or event.mod & KMOD_CTRL:
                pass
            elif event.unicode:
                self.data += event.unicode
                self.dirty = True
        return True
    
    def render(self, screen):
        if self.file_browser_dirty:
            self.file_browser_surf.fill((0, 0, 0, 0))
            for file in range(self.file_browser_y_offset,
                    min(len(self.current_dir_list),
                    self.file_browser_y_offset + self.file_browser_item_count)):
                y = self.file_browser_item_height * (file - self.file_browser_y_offset)
                filename = self.current_dir_list[file]
                path = os.path.join(self.current_dir, filename)
                if filename == "<...>" or os.path.isdir(path):
                    icon = icon_dir
                else:
                    icon = icon_doc
                self.file_browser_surf.blit(icon, (0, y + (self.file_browser_item_height // 2 - icon.get_height() // 2)))
                text = self.style.text_style.font.render(filename, 1, self.style.font_color)
                self.file_browser_surf.blit(text, (icon.get_width() + 10, y + (self.file_browser_item_height // 2 - text.get_height() // 2)))
                self.file_browser_dirty = False
        if self.dirty:
            self.surf.fill(self.style.window_color)
            self.surf.blit(self.prompt_text, (50, 40))
            text = self.style.text_style.font.render(
                self.data[max(0, len(self.data) - self.width):],
                1, self.style.keyword_color)
            self.surf.blit(text, (50, self.y_size - 40 - self.style.text_style.line_height))
            self.surf.blit(self.file_browser_surf, (self.file_browser_x - self.x, self.file_browser_y - self.y))
            self.dirty = False
        screen.blit(self.surf, (self.x, self.y))

class OptionSelector (Callable):
    def __init__(self, x, y, x_size, y_size, parent, prompt, options, action, style):
        Callable.__init__(self, parent)
        self.style = style
        self.prompt_text = style.text_style.font.render(prompt, 1, style.font_color)
        self.x_size = x_size - 100
        self.y_size = y_size - 80
        self.x = x + 50
        self.y = y + 40
        self.surf = pygame.Surface((self.x_size, self.y_size), SRCALPHA)
        self.width = int((self.x_size - 100) / style.text_style.char_width)
        self.options_x_size = self.x_size - 140
        self.options_y_size = self.y_size - 120 - style.text_style.line_height * 2
        self.options_x = self.x + 70
        self.options_y = self.y + 60 + style.text_style.line_height
        self.options_item_height = style.text_style.line_height
        self.options_item_count = self.options_y_size // self.options_item_height
        self.options_y_offset = 0
        self.options = options
        self.options_surf = pygame.Surface((self.options_x_size, self.options_y_size), SRCALPHA)
        self.options_dirty = True
        self.action = action
        self.dirty = True
    
    def get_size(self):
        return self.x_size, self.y_size
    
    def event(self, event):
        if event.type == MOUSEBUTTONDOWN:
            x, y = event.pos
            if (event.button == 1
            and x >= self.options_x and x < self.options_x + self.options_x_size
            and y >= self.options_y and y < self.options_y + self.options_y_size):
                option_id = self.options_y_offset + (y - self.options_y) // self.options_item_height
                if option_id < len(self.options):
                    option = self.options[option_id]
                    self.action(option)
                    self.return_value(None)
            elif event.button == 4:
                self.options_y_offset = max(0, self.options_y_offset - 3)
                self.options_dirty = True
                self.dirty = True
            elif event.button == 5:
                self.options_y_offset = max(0, min(len(self.options) - self.options_item_count, self.options_y_offset + 3))
                self.options_dirty = True
                self.dirty = True
        return True
    
    def render(self, screen):
        if self.options_dirty:
            self.options_surf.fill((0, 0, 0, 0))
            for option_id in range(self.options_y_offset,
                    min(len(self.options),
                    self.options_y_offset + self.options_item_count)):
                y = self.options_item_height * (option_id - self.options_y_offset)
                option = self.options[option_id]
                text = self.style.text_style.font.render(option, 1, self.style.font_color)
                self.options_surf.blit(text, (0, y + (self.options_item_height // 2 - text.get_height() // 2)))
                self.options_dirty = False
        if self.dirty:
            self.surf.fill(self.style.window_color)
            self.surf.blit(self.prompt_text, (50, 40))
            self.surf.blit(self.options_surf, (self.options_x - self.x, self.options_y - self.y))
            self.dirty = False
        screen.blit(self.surf, (self.x, self.y))

from .lexer import tokenize

class IconButton:
    def __init__(self, icon, name, action):
        self.icon = icon
        self.name = name
        self.action = action

class IconButtonBox:
    def __init__(self, x, y, x_size, y_size, buttons, style):
        self.x = x
        self.y = y
        self.x_size = x_size
        self.y_size = y_size
        self.buttons = buttons
        self.style = style
        self.surf = pygame.Surface((x_size, y_size), SRCALPHA)
        self.icon_surf = pygame.Surface((icon_hover.get_width() * len(buttons), icon_hover.get_height()), SRCALPHA)
        for button in range(len(buttons)):
            x = icon_hover.get_width() * button
            self.icon_surf.blit(buttons[button].icon, (x, 0))
        self.hover = None
        self.dirty = True
        self.mouse_x = 0
        self.mouse_y = 0
        self.floating_text = None
    
    def event(self, event):
        if event.type == MOUSEMOTION:
            x, y = event.pos
            hover = self.hover
            if (x >= self.x and x < self.x + self.x_size
            and y >= self.y and y < self.y + self.icon_surf.get_height()):
                button = (x - self.x) // icon_hover.get_width()
                if button < len(self.buttons):
                    self.hover = button
                else:
                    self.hover = None
                self.mouse_x = x
                self.mouse_y = y
            else:
                self.hover = None
            if hover != self.hover:
                self.dirty = True
        elif event.type == MOUSEBUTTONDOWN and event.button == 1:
            if self.hover is not None:
                self.buttons[self.hover].action()
    
    def render(self, screen):
        if self.dirty:
            self.surf.fill((0, 0, 0, 0))
            self.surf.blit(self.icon_surf, (0, 0))
            if self.hover is not None:
                self.surf.blit(icon_hover, (self.hover * icon_hover.get_width(), 0))
                text = self.style.text_style.font.render(self.buttons[self.hover].name, 1, self.style.font_color)
                self.floating_text = pygame.Surface((text.get_width() + 20, text.get_height() + 20))
                self.floating_text.fill(self.style.bg_color)
                self.floating_text.blit(text, (10, 10))
            else:
                self.floating_text = None
            self.dirty = False
        screen.blit(self.surf, (self.x, self.y))
        if self.floating_text:
            screen.blit(self.floating_text, (self.mouse_x, self.mouse_y - self.floating_text.get_height()))

class ToolBar (UIObject):
    def __init__(self, x, y, x_size, y_size, studio, text, buttons, style):
        self.x = x
        self.y = y
        self.x_size = x_size
        self.y_size = y_size
        self.text = text
        self.buttons = IconButtonBox(10, 20 + style.text_style.line_height, x_size - 10, y_size - 20 - style.text_style.line_height, buttons, style)
        self.style = style
        self.surf = pygame.Surface((x_size, y_size))
        self.dirty = True
    
    def set_text(self, text):
        self.text = text
        self.dirty = True
    
    def event(self, event):
        return self.buttons.event(event)
    
    def render(self, screen):
        if self.dirty:
            self.surf.fill(self.style.lineno_bg_color)
            self.surf.blit(self.style.text_style.font.render(self.text, 1, self.style.font_color), (10, 10))
            self.dirty = False
        screen.blit(self.surf, (self.x, self.y))
        self.buttons.render(screen)

class ProjectView (UIObject):
    def __init__(self, x, y, x_size, y_size, studio, style):
        self.x = x
        self.y = y
        self.x_size = x_size
        self.y_size = y_size
        self.studio = studio
        self.style = style
        self.title = style.text_style.font.render("Project View:", 0, style.font_color)
        self.surf = pygame.Surface((x_size, y_size))
        self.files_x = x + 20
        self.files_y = y + 20 + style.text_style.line_height
        self.files_x_size = x_size - 20
        self.files_y_size = y_size - 20 - icon_hover.get_height() - self.files_y
        self.files_surf = pygame.Surface((self.files_x_size, self.files_y_size), SRCALPHA)
        self.files_max = self.files_y_size // style.text_style.line_height
        self.files_off = 0
        self.files_list = [ ]
        self.add_button = IconButton(icon_new, "Create", self.create)
        self.rel_button = IconButton(icon_reload, "Reload", self.reload)
        self.buttons = IconButtonBox(x + 10, y + self.files_y + self.files_y_size + 10, x_size - 10, icon_hover.get_height(), [self.add_button, self.rel_button], style)
        self.dirty = True
        self.request = None
        self.prompt = None
    
    def update(self):
        if self.request is None:
            return
        elif self.request == "new":
            if self.prompt is None:
                self.prompt = FileBrowser(self.studio.x, self.studio.y, self.studio.x_size, self.studio.y_size, self.studio, "Create a file:", "new_file.shk", self.studio.src_dir, self.style)
            elif self.prompt.returned:
                filename = self.prompt.get_value()
                self.request = None
                self.prompt = None
                if filename is None:
                    return
                if not filename.startswith(self.studio.src_dir):
                    return
                if not filename.endswith(".shk"):
                    if not os.path.exists(filename):
                        os.mkdir(filename)
                else:
                    file = filename[len(self.studio.src_dir) + 1:]
                    if not os.path.isfile(filename):
                        with open(filename, "w") as source:
                            source.write("\n// project '%s', file '%s'\n" % (self.studio.project_name, file))
                    self.studio.open(file)
                    self.reload()
    
    def create(self):
        self.request = "new"
        self.update()
    
    def reload(self):
        def listdir(root, base, name):
            list = [ ]
            for child in os.listdir(os.path.join(root, base, name)):
                if os.path.isdir(os.path.join(root, base, name, child)):
                    list.extend(listdir(root, os.path.join(base, name), child))
                elif child.endswith(".shk"):
                    list.append(os.path.join(base, name, child))
            return list
        self.files_list = sorted(listdir(self.studio.src_dir, "", ""))
        self.dirty = True
    
    def event(self, event):
        if event.type == MOUSEBUTTONDOWN:
            x, y = event.pos
            if (x >= self.files_x and x < self.files_x + self.files_x_size
            and y >= self.files_y and y < self.files_y + self.files_y_size):
                if event.button == 1:
                    file = self.files_off + (y - self.files_y) // self.style.text_style.line_height
                    if file < len(self.files_list):
                        self.studio.open(self.files_list[file])
                elif event.button == 4:
                    self.files_off = max(0, self.files_off - 3)
                    self.dirty = True
                elif event.button == 5:
                    self.files_off = max(0, min(len(self.files_list) - self.files_max, self.files_off + 3))
                    self.dirty = True
                return True
        return self.buttons.event(event)
    
    def render(self, screen):
        if self.dirty:
            self.surf.fill(self.style.bg_color)
            self.surf.blit(self.title, (10, 10))
            self.files_surf.fill((0, 0, 0, 0))
            y = 0
            for file in range(self.files_off,
                    min(len(self.files_list), self.files_off + self.files_max)):
                text = self.style.text_style.font.render(self.files_list[file], 1, self.style.font_color)
                self.files_surf.blit(text, (0, y))
                y += text.get_height()
            self.surf.blit(self.files_surf, (self.files_x - self.x, self.files_y - self.y))
            self.dirty = False
        screen.blit(self.surf, (self.x, self.y))
        self.buttons.render(screen)

class CodeEditor (UIObject):
    def __init__(self, x, y, x_size, y_size, style):
        self.x = x
        self.y = y
        self.x_size = x_size
        self.y_size = y_size
        self.style = style
        self.font = style.text_style.font
        self.sample = style.text_style.sample
        self.line_height = style.text_style.line_height
        self.line_count = y_size // self.line_height
        self.char_width = style.text_style.char_width
        self.lineno_x_size = int(self.char_width * 6)
        self.lineno_surf = pygame.Surface((self.lineno_x_size, y_size))
        x_size -= self.lineno_x_size
        self.surf = pygame.Surface((x_size, y_size))
        self.char_count = x_size // self.char_width
        self.cursor = pygame.Surface((1, self.line_height))
        self.cursor.fill(style.cursor_color)
        self.current_line_bg = pygame.Surface((x_size, self.line_height))
        self.current_line_bg.fill(style.current_line_bg_color)
        self.data = [ "" ]
        self.lineno_data = [ self.sample ] * self.line_count
        self.render_data = [ self.sample ] * self.line_count
        self.viewport_x = 0
        self.viewport_y = 0
        self.cursor_x = 0
        self.cursor_y = 0
        self.update()
    
    def get_size(self):
        return self.surf.get_size()
    
    def update(self):
        max_y = min(len(self.data), self.viewport_y + self.line_count)
        for line in range(self.viewport_y, max_y):
            self.render_data[line - self.viewport_y] = self.color_line(self.data[line])
            self.lineno_data[line - self.viewport_y] = self.font.render(str(line + 1), 0, self.style.font_color)
        for line in range(max_y, self.viewport_y + self.line_count):
            self.render_data[line - self.viewport_y] = self.sample
            self.lineno_data[line - self.viewport_y] = self.sample
    
    def color_line(self, line_data):
        tokens = tokenize(line_data)
        size_x = int(self.style.text_style.char_width * len(line_data)) + len(tokens)
        surf = pygame.Surface((size_x, self.line_height), SRCALPHA)
        x = 0
        for type, value in tokens:
            color = self.style.font_color
            if type == "keyword":
                color = self.style.keyword_color
            elif type == "number":
                color = self.style.number_color
            elif type == "string" or type == "char":
                color = self.style.string_color
            elif type == "operator":
                color = self.style.operator_color
            elif type == "comment":
                color = self.style.comment_color
            elif type == "error":
                color = self.style.error_color
            render = self.font.render(value, 1, color)
            surf.blit(render, (x, 0))
            x += render.get_width()
        return surf
    
    def center_cursor(self, update = True):
        if self.cursor_x < self.viewport_x:
            self.viewport_x = self.cursor_x
        elif self.cursor_x >= self.viewport_x + self.char_count:
            self.viewport_x = self.cursor_x - self.char_count + 1
        if self.cursor_y < self.viewport_y:
            self.viewport_y = self.cursor_y
            if update: self.update()
        elif self.cursor_y >= self.viewport_y + self.line_count:
            self.viewport_y = self.cursor_y - self.line_count + 1
            if update: self.update()
    
    def render(self, screen):
        self.lineno_surf.fill(self.style.lineno_bg_color)
        self.surf.fill(self.style.bg_color)
        self.surf.blit(self.current_line_bg, (0, (self.cursor_y - self.viewport_y) * self.line_height))
        for line in range(self.line_count):
            self.lineno_surf.blit(self.lineno_data[line],
                (int(self.lineno_x_size - self.char_width - self.lineno_data[line].get_width()),
                self.line_height * line))
            self.surf.blit(self.render_data[line], (0 - self.viewport_x * self.char_width, self.line_height * line))
        self.surf.blit(self.cursor, (int((self.cursor_x - self.viewport_x) * self.char_width),
            (self.cursor_y - self.viewport_y) * self.line_height))
        screen.blit(self.lineno_surf, (self.x, self.y))
        screen.blit(self.surf, (self.x + self.lineno_x_size, self.y))
    
    def event(self, event):
        if event.type == MOUSEBUTTONDOWN:
            event_x, event_y = event.pos
            if (event_x >= self.x + self.lineno_x_size and event_x < self.x + self.x_size
            and event_y >= self.y and event_y < self.y + self.y_size):
                if event.button == 1:
                    self.cursor_y = min(len(self.data) - 1,
                        self.viewport_y + (event_y - self.y) // self.line_height)
                    self.cursor_x = min(len(self.data[self.cursor_y]),
                        self.viewport_x + int((event_x - self.x - self.lineno_x_size) / self.char_width))
                    return True
                elif event.button == 4:
                    self.viewport_y = max(0, self.viewport_y - 3)
                elif event.button == 5:
                    self.viewport_y = max(0, min(len(self.data) - self.line_count, self.viewport_y + 3))
                else:
                    return False
        elif event.type == KEYDOWN:
            if event.key == K_LEFT:
                self.cursor_x -= 1
                if self.cursor_x < 0:
                    self.cursor_y -= 1
                    if self.cursor_y < 0:
                        self.cursor_x = 0
                        self.cursor_y = 0
                    else:
                        self.cursor_x = len(self.data[self.cursor_y])
                self.center_cursor()
            elif event.key == K_RIGHT:
                self.cursor_x += 1
                if self.cursor_x > len(self.data[self.cursor_y]):
                    self.cursor_y += 1
                    if self.cursor_y >= len(self.data):
                        self.cursor_x -= 1
                        self.cursor_y -= 1
                    else:
                        self.cursor_x = 0
                self.center_cursor()
            elif event.key == K_UP:
                if self.cursor_y > 0:
                    self.cursor_y -= 1
                    self.cursor_x = min(self.cursor_x, len(self.data[self.cursor_y]))
                self.center_cursor()
            elif event.key == K_DOWN:
                if self.cursor_y < len(self.data) - 1:
                    self.cursor_y += 1
                    self.cursor_x = min(self.cursor_x, len(self.data[self.cursor_y]))
                self.center_cursor()
            elif event.key == K_RETURN:
                text = self.data[self.cursor_y]
                indent_size = len(text) - len(text.lstrip(' '))
                self.data[self.cursor_y] = ' ' * indent_size + text[self.cursor_x:]
                self.data.insert(self.cursor_y, text[:self.cursor_x])
                self.cursor_x = indent_size
                self.cursor_y += 1
                self.center_cursor(update=False)
                self.update()
            elif event.key == K_TAB:
                text = self.data[self.cursor_y]
                indent_size = 4 - self.cursor_x % 4
                self.data[self.cursor_y] = text[:self.cursor_x] + " " * indent_size + text[self.cursor_x:]
                self.cursor_x += indent_size
                self.center_cursor(update=False)
                self.update()
            elif event.key == K_BACKSPACE:
                if self.cursor_x == 0:
                    if self.cursor_y == 0:
                        self.center_cursor()
                    else:
                        self.cursor_y -= 1
                        self.cursor_x = len(self.data[self.cursor_y])
                        self.data[self.cursor_y] = self.data[self.cursor_y] + self.data.pop(self.cursor_y + 1)
                        self.center_cursor(update=False)
                        self.update()
                else:
                    text = self.data[self.cursor_y]
                    self.data[self.cursor_y] = text[:self.cursor_x - 1] + text[self.cursor_x:]
                    self.cursor_x -= 1
                    self.center_cursor(update=False)
                    self.update()
            elif event.key in NULL_KEYS or event.mod & KMOD_CTRL:
                return False
            elif event.unicode:
                text = self.data[self.cursor_y]
                self.data[self.cursor_y] = text[:self.cursor_x] + event.unicode + text[self.cursor_x:]
                self.cursor_x += 1
                self.center_cursor(update=False)
                self.update()
            return True

import sys, traceback

class GameSharkScreen (Callable):
    def __init__(self, x, y, x_size, y_size, parent, activity_class, style):
        Callable.__init__(self, parent)
        self.x = x
        self.y = y
        self.x_size = x_size
        self.y_size = y_size
        self.style = style
        self.surf = pygame.Surface((x_size, y_size), SRCALPHA)
        self.surf.fill(style.window_color)
        self.text = style.text_style.font.render("Running gameshark project, press (escape) to exit:", 1, style.font_color)
        self.surf.blit(self.text, (max(10, x_size // 2 - self.text.get_width() // 2), 10))
        max_x = x_size
        max_y = y_size - 20 - style.text_style.line_height
        self.child_x_size = max_x
        self.child_y_size = int(max_x / 320 * 192)
        if self.child_y_size > max_y:
            self.child_y_size = max_y
            self.child_x_size = int(max_y / 192 * 320)
        self.child_x = max_x // 2 - self.child_x_size // 2
        self.child_y = max_y // 2 - self.child_y_size // 2 + 20 + style.text_style.line_height
        try:
            self.child = activity_class()
            self.child.launch()
        except:
            print("Exception in user code:")
            traceback.print_exc(file=sys.stdout)
            self.return_value(None)
    
    def return_value(self, value):
        Callable.return_value(self, value)
        print("==== EXECUTION FINISHED ====")
    
    def event(self, event):
        import shark.event
        try:
            if event.type == KEYDOWN:
                if event.key == K_z:
                    self.child.event(shark.event.E_PRESS_X, 0, 0)
                elif event.key == K_x:
                    self.child.event(shark.event.E_PRESS_Y, 0, 0)
                elif event.key == K_UP:
                    self.child.event(shark.event.E_PRESS_UP, 0, 0)
                elif event.key == K_DOWN:
                    self.child.event(shark.event.E_PRESS_DOWN, 0, 0)
                elif event.key == K_LEFT:
                    self.child.event(shark.event.E_PRESS_LEFT, 0, 0)
                elif event.key == K_RIGHT:
                    self.child.event(shark.event.E_PRESS_RIGHT, 0, 0)
                elif event.key == K_ESCAPE:
                    self.return_value(None)
            elif event.type == KEYUP:
                if event.key == K_z:
                    self.child.event(shark.event.E_REL_X, 0, 0)
                elif event.key == K_x:
                    self.child.event(shark.event.E_REL_Y, 0, 0)
                elif event.key == K_UP:
                    self.child.event(shark.event.E_REL_UP, 0, 0)
                elif event.key == K_DOWN:
                    self.child.event(shark.event.E_REL_DOWN, 0, 0)
                elif event.key == K_LEFT:
                    self.child.event(shark.event.E_REL_LEFT, 0, 0)
                elif event.key == K_RIGHT:
                    self.child.event(shark.event.E_REL_RIGHT, 0, 0)
        except:
            print("Exception in user code:")
            traceback.print_exc(file=sys.stdout)
            self.return_value(None)
        return True
    
    def update(self):
        try:
            self.child.update()
        except:
            print("Exception in user code:")
            traceback.print_exc(file=sys.stdout)
            self.return_value(None)
    
    def render(self, screen):
        try:
            self.child.render()
        except:
            print("Exception in user code:")
            traceback.print_exc(file=sys.stdout)
            self.return_value(None)
        self.surf.blit(pygame.transform.scale(self.child.display, (self.child_x_size, self.child_y_size)), (self.child_x, self.child_y))
        screen.blit(self.surf, (self.x, self.y))

import shark.asset as _asset
import shark.persistent as persistent
from pyshark.runtime import shark_compile, add_import_path, load_module
from pyshark.main import exec_file

class Studio (Container):
    def __init__(self, x, y, x_size, y_size, style):
        self.x = x
        self.y = y
        self.x_size = x_size
        self.y_size = y_size
        self.style = style
        self.current_project = None
        self.toolbar = ToolBar(x, y, x_size, 30 + style.text_style.line_height + icon_hover.get_height(), self, "Project: no project", [
            IconButton(icon_save, "Save", self.save),
            IconButton(icon_open, "Open", self.new_project),
            IconButton(icon_build, "Build", self.build),
            IconButton(icon_run, "Run", self.run)], style)
        self.project_view = ProjectView(x, y + self.toolbar.y_size, max(x_size // 4, 120), y_size - self.toolbar.y_size, self, style)
        self.editor = CodeEditor(x + self.project_view.x_size, y + self.toolbar.y_size, x_size - self.project_view.x_size, y_size - self.toolbar.y_size, style)
        self.children = [ self.toolbar, self.project_view, self.editor ]
        self.request = "project_open"
        self.prompt = None
        self.project_type = None
        self.files = {  }
    
    def get_size(self):
        return self.x_size, self.y_size
    
    def new_project(self):
        self.save()
        self.request = "project_open"
        self.prompt = None
    
    def open(self, filename):
        if not filename in self.files:
            editor = CodeEditor(self.x + self.project_view.x_size, self.y + self.toolbar.y_size, self.x_size - self.project_view.x_size, self.y_size - self.toolbar.y_size, self.style)
            with open(os.path.join(self.src_dir, filename), "r") as source:
                editor.data = source.read().split("\n")
            self.files[filename] = editor
            self.editor = editor
        else:
            self.editor = self.files[filename]
        self.children[2] = self.editor
        self.editor.update()
        self.toolbar.set_text("Project: %s, file: %s" % (self.project_name, filename))
    
    def save(self):
        current = None
        for file in self.files:
            editor = self.files[file]
            if editor is self.editor:
                current = file
            with open(os.path.join(self.src_dir, file), "w") as source:
                source.write("\n".join(editor.data))
        if current:
            self.files = {current: self.files[current]}
        else:
            self.files = { }
    
    def build(self):
        self.save()
        self.prompt = OptionSelector(self.x, self.y, self.x_size, self.y_size, self,
            "Select a target to build:", [
                "None (Build no target)"
            ], self.build_target, self.style
        )
    
    def build_target(self, target):
        pass
    
    def run(self):
        self.save()
        try:
            game_dir = self.project_dir
            main_file = os.path.join(game_dir, "src/main.shk")
            bin_path = os.path.join(game_dir, "bin")
            _asset.__ASSET_PATH__ = os.path.join(game_dir, "asset")
            persistent.__SAVE_PATH__ = os.path.join(game_dir, "save")
            shark_compile(main_file, os.path.join(bin_path, "main.py"), True)
            context = exec_file(os.path.join(bin_path, "main.py"), main_file, [ ])
            GameSharkScreen(self.x, self.y, self.x_size, self.y_size, self, context["main_activity"], self.style)
        except:
            print("Exception in user code:")
            traceback.print_exc(file=sys.stdout)
            print("==== EXECUTION FINISHED ====")
            self.prompt = TextInput(self.x, self.y, self.x_size, self.y_size, self, "An error happened, see console output for details. (press enter)", "", self.style)
            self.prompt.locked = True
    
    def update(self):
        if self.request == "project_open":
            if self.prompt is None:
                self.prompt = FileBrowser(self.x, self.y, self.x_size, self.y_size, self, "Select or create a project:", "new_project", project_dir, self.style)
                self.prompt.configure(True, True)
            elif isinstance(self.prompt, FileBrowser) and self.prompt.returned:
                self.files = { }
                self.editor = CodeEditor(self.x + self.project_view.x_size, self.y + self.toolbar.y_size, self.x_size - self.project_view.x_size, self.y_size - self.toolbar.y_size, self.style)
                self.children[2] = self.editor
                self.project_dir = self.prompt.get_value()
                if self.project_dir is None:
                    self.request = "project_open"
                    self.prompt = None
                    return
                self.project_name = os.path.split(self.project_dir)[1]
                if not self.project_name:
                    self.request = "project_open"
                    self.prompt = None
                    return
                if not os.path.isdir(self.project_dir):
                    os.mkdir(self.project_dir)
                    os.mkdir(os.path.join(self.project_dir, "asset"))
                    os.mkdir(os.path.join(self.project_dir, "bin"))
                    os.mkdir(os.path.join(self.project_dir, "src"))
                    with open(os.path.join(self.project_dir, "src/main.shk"), "w") as source:
                        source.write("\n# import shark libraries\nimport shark.activity: activity\nimport shark.asset\nimport shark.event\n\n# main class\nclass main_activity (activity)\n    function launch()\n        pass\n    \n    function event(type, x, y)\n        pass\n    \n    function update()\n        pass\n    \n    function render()\n        pass\n")
                self.src_dir = os.path.join(self.project_dir, "src")
                main_file = "main.shk"
                if not os.path.isfile(os.path.join(self.src_dir, main_file)):
                    raise ValueError("can't find project's main file.")
                self.open(main_file)
                self.project_view.reload()
                self.request = None
                self.prompt = None
        Container.update(self)

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

import os
import sys
import math

import pygame
from pygame import K_BACKSPACE, K_TAB, K_CLEAR, K_RETURN, K_PAUSE, K_ESCAPE, K_DELETE, K_UP, K_DOWN, K_RIGHT, K_LEFT, K_INSERT, K_HOME, K_END, K_PAGEUP, K_PAGEDOWN, K_F1, K_F2, K_F3, K_F4, K_F5, K_F6, K_F7, K_F8, K_F9, K_F10, K_F11, K_F12, K_F13, K_F14, K_F15, K_NUMLOCK, K_CAPSLOCK, K_SCROLLOCK, K_RSHIFT, K_LSHIFT, K_RCTRL, K_LCTRL, K_RALT, K_LALT, K_RMETA, K_LMETA, K_LSUPER, K_RSUPER, K_MODE, K_HELP, K_PRINT, K_SYSREQ, K_BREAK, K_MENU, K_POWER, K_EURO

class texture:
    def __init__(self, surface):
        self.surface = surface
    
    def get_size_x(self):
        return self.surface.get_width()
    
    def get_size_y(self):
        return self.surface.get_height()

class font:
    def __init__(self, font, size, color):
        self.font = font
        self.size = size
        self.color = pygame.Color(color >> 24 & 0xFF, color >> 16 & 0xFF, color >> 8 & 0xFF, color & 0xFF)
    
    def get_height(self):
        return self.font.get_height()
    
    def get_width(self, data):
        return self.font.size(data)

__SAVE_PATH__ = "save"

def get_save_file(mode):
    return open(__SAVE_PATH__, mode)

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

text_event = False

def set_text_event(status):
    text_event = status
    return True

pressed = False

NULL_KEYS = { K_BACKSPACE, K_TAB, K_CLEAR, K_RETURN, K_PAUSE,
              K_ESCAPE, K_DELETE, K_UP, K_DOWN, K_RIGHT, K_LEFT,
              K_INSERT, K_HOME, K_END, K_PAGEUP, K_PAGEDOWN,
              K_F1, K_F2, K_F3, K_F4, K_F5, K_F6, K_F7, K_F8, K_F9,
              K_F10, K_F11, K_F12, K_F13, K_F14, K_F15, K_NUMLOCK,
              K_CAPSLOCK, K_SCROLLOCK, K_RSHIFT, K_LSHIFT, K_RCTRL,
              K_LCTRL, K_RALT, K_LALT, K_RMETA, K_LMETA, K_LSUPER,
              K_RSUPER, K_MODE, K_HELP, K_PRINT, K_SYSREQ, K_BREAK,
              K_MENU, K_POWER, K_EURO }

def dispatch():
    global pressed
    while True:
        ev = pygame.event.poll()
        if ev.type == pygame.NOEVENT:
            return None
        elif ev.type == pygame.MOUSEBUTTONDOWN and ev.button == 1:
            pressed = True
            event(E_PRESS, ev.pos[0], ev.pos[1])
        elif ev.type == pygame.MOUSEBUTTONUP and ev.button == 1:
            pressed = False
            event(E_RELEASE, ev.pos[0], ev.pos[1])
        elif ev.type == pygame.MOUSEBUTTONDOWN and ev.button == 4:
            event(E_ROLL_UP, ev.pos[0], ev.pos[1])
        elif ev.type == pygame.MOUSEBUTTONDOWN and ev.button == 5:
            event(E_ROLL_DOWN, ev.pos[0], ev.pos[1])
        elif ev.type == pygame.MOUSEMOTION and pressed:
            event(E_MOVE, ev.pos[0], ev.pos[1])
        elif ev.type == pygame.KEYDOWN:
            if not text_event and ev.key == pygame.K_z:
                event(E_PRESS_X, 0, 0)
            elif not text_event and ev.key == pygame.K_x:
                event(E_PRESS_Y, 0, 0)
            elif ev.key == pygame.K_RETURN:
                event(E_PRESS_X, 0, 0)
            elif ev.key == pygame.K_BACKSPACE:
                event(E_PRESS_Y, 0, 0)
            elif ev.key == pygame.K_UP:
                event(E_PRESS_UP, 0, 0)
            elif ev.key == pygame.K_DOWN:
                event(E_PRESS_DOWN, 0, 0)
            elif ev.key == pygame.K_LEFT:
                event(E_PRESS_LEFT, 0, 0)
            elif ev.key == pygame.K_RIGHT:
                event(E_PRESS_RIGHT, 0, 0)
            elif ev.key not in NULL_KEYS:
                text_event(ev.unicode)
        elif ev.type == pygame.KEYUP:
            if not text_event and ev.key == pygame.K_z:
                event(E_REL_X, 0, 0)
            elif not text_event and ev.key == pygame.K_x:
                event(E_REL_Y, 0, 0)
            elif ev.key == pygame.K_RETURN:
                event(E_REL_X, 0, 0)
            elif ev.key == pygame.K_BACKSPACE:
                event(E_REL_Y, 0, 0)
            elif ev.key == pygame.K_UP:
                event(E_REL_UP, 0, 0)
            elif ev.key == pygame.K_DOWN:
                event(E_REL_DOWN, 0, 0)
            elif ev.key == pygame.K_LEFT:
                event(E_REL_LEFT, 0, 0)
            elif ev.key == pygame.K_RIGHT:
                event(E_REL_RIGHT, 0, 0)
            elif ev.key not in NULL_KEYS:
                cancel_event()
        elif ev.type == pygame.QUIT:
            pygame.quit()
            sys.exit(0)
        else:
            continue

ERR_ASSET_NOT_FOUND = 1000
__ASSET_PATH__ = "."

def load_texture(name):
    try:
        surface = pygame.image.load(os.path.join(__ASSET_PATH__, name))
    except:
        surface = None
    if surface == None:
        set_err(ERR_ASSET_NOT_FOUND)
        return None
    else:
        return texture (surface)

def load_font(name, size, color):
    try:
        font_object = pygame.font.Font(os.path.join(__ASSET_PATH__, name), size)
    except:
        font_object = None
    if font_object == None:
        set_err(ERR_ASSET_NOT_FOUND)
        return None
    else:
        return font (font_object, size, color)

pygame.init()

DEFAULT_X_SIZE = 320
DEFAULT_Y_SIZE = 192

DESKTOP_SCALE_FACTOR = 3

display = pygame.Surface([DEFAULT_X_SIZE, DEFAULT_Y_SIZE])

def draw(text, x, y):
    display.blit(text.surface, [x, y])

def draw_ex(text, x, y, origin_x, origin_y, rotation, scale_x, scale_y):
    flip_x = scale_x < 0
    flip_y = scale_y < 0
    if flip_x: scale_x = -scale_x
    if flip_y: scale_y = -scale_y
    t = pygame.transform.rotate(
        pygame.transform.scale(
            pygame.transform.flip(text.surface, flip_x, flip_y),
            [int(text.surface.get_width() * scale_x),
            int(text.surface.get_height() * scale_y)]),
        rotation)
    r = math.radians(rotation)
    ox = (math.cos(r) * origin_x + math.sin(r) * origin_y) * scale_x
    oy = (math.cos(r) * origin_y - math.sin(r) * origin_x) * scale_y
    x = x + ox - t.get_width() / 2
    y = y + oy - t.get_height() / 2
    display.blit(t, [x, y])

def draw_text(text, font, x, y):
    display.blit(font.font.render(text, 1, font.color), [x, y])

def text_event(text):
    pass

def cancel_event():
    pass

def main(args):
    global __ASSET_PATH__, __SAVE_PATH__
    __ASSET_PATH__ = os.path.join(os.path.split(args[0])[0], "asset")
    __SAVE_PATH__ = os.path.join(os.path.split(args[0])[0], "save")
    screen = pygame.display.set_mode([
        DEFAULT_X_SIZE * DESKTOP_SCALE_FACTOR,
        DEFAULT_Y_SIZE * DESKTOP_SCALE_FACTOR
    ])
    timer = pygame.time.Clock()
    launch()
    while True:
        dispatch()
        update()
        render()
        screen.blit(pygame.transform.scale(display, [
            DEFAULT_X_SIZE * DESKTOP_SCALE_FACTOR,
            DEFAULT_Y_SIZE * DESKTOP_SCALE_FACTOR]),
            [0, 0])
        pygame.display.flip()
        timer.tick(24)

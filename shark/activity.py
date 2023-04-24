###############################################################################
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

import sys
import pygame
import math
import shark.event as event

DEFAULT_X_SIZE = 320
DEFAULT_Y_SIZE = 192

DESKTOP_SCALE_FACTOR = 3

class activity:
    def __init__(self):
        self.display = pygame.Surface((DEFAULT_X_SIZE, DEFAULT_Y_SIZE))
    
    def draw(self, text, x, y):
        self.display.blit(text.surface, (x, y))
    
    def draw_ex(self, text, x, y, origin_x, origin_y, rotation, scale_x, scale_y):
        flip_x = False
        flip_y = False
        if scale_x == 0 or scale_y == 0:
            return
        if scale_x < 0:
            flip_x = True
            scale_x = -scale_x
        if scale_y < 0:
            flip_y = True
            scale_y = -scale_y
        t = pygame.transform.rotate(pygame.transform.scale(pygame.transform.flip(text.surface, flip_x, flip_y), (int(text.surface.get_width() * scale_x), int(text.surface.get_height() * scale_y))), rotation)
        r = math.radians(rotation)
        ox = (math.cos(r) * origin_x + math.sin(r) * origin_y) * scale_x
        oy = (math.cos(r) * origin_y - math.sin(r) * origin_x) * scale_y
        x = x + ox - t.get_width() / 2
        y = y + oy - t.get_height() / 2
        self.display.blit(t, (x, y))
    
    def draw_text(self, text, font, x, y):
        self.display.blit(font.font.render(text, 1, font.color), (x, y))
    
    def launch(self):
        pass
    
    def event(self, type, x, y):
        pass
    
    def update(self):
        pass
    
    def render(self):
        pass

def launch_activity(activity_class):
    screen = pygame.display.set_mode((DEFAULT_X_SIZE * DESKTOP_SCALE_FACTOR, DEFAULT_Y_SIZE * DESKTOP_SCALE_FACTOR))
    activity = activity_class()
    timer = pygame.time.Clock()
    activity.launch()
    try:
        while True:
            event.dispatch(activity)
            activity.update()
            activity.render()
            screen.blit(pygame.transform.scale(activity.display,
                (DEFAULT_X_SIZE * DESKTOP_SCALE_FACTOR, DEFAULT_Y_SIZE * DESKTOP_SCALE_FACTOR)), (0, 0))
            pygame.display.flip()
            timer.tick(24)
    except SystemExit:
        pass

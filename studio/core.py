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

pygame.init()
pygame.key.set_repeat(250, 30)

root = None

class UIObject:
    x = 0
    y = 0
    
    def __init__(self):
        pass
    
    def get_size(self):
        return (0, 0)
    
    def event(self, event):
        pass
    
    def update(self):
        pass
    
    def render(self, screen):
        pass

class Container (UIObject):
    def __init__(self):
        self.children = [ ]
    
    def add_child(self, child):
        self.children.append(child)
    
    def event(self, event):
        for child in reversed(self.children):
            if child.event(event):
                return True
        return False
    
    def update(self):
        for child in self.children:
            child.update()
    
    def render(self, screen):
        for child in self.children:
            child.render(screen)

class Application (Container):
    x_size = 1200
    y_size = 680
    
    def execute(self):
        global root
        root = self
        
        self.timer = pygame.time.Clock()
        self.display = pygame.display.set_mode((self.x_size, self.y_size))
        
        while True:
            while True:
                event = pygame.event.poll()
                if event.type == pygame.NOEVENT:
                    break
                elif event.type == pygame.QUIT:
                    pygame.quit()
                    return
                else:
                    self.event(event)
            self.update()
            self.render(self.display)
            pygame.display.flip()
            self.timer.tick(24)

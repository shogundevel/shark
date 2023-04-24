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

pressed = False

def dispatch(consumer):
    global pressed
    while True:
        event = pygame.event.poll()
        if event.type == pygame.NOEVENT:
            return None
        if event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
            pressed = True
            consumer.event(E_PRESS, event.pos[0], event.pos[1])
        elif event.type == pygame.MOUSEBUTTONUP and event.button == 1:
            pressed = False
            consumer.event(E_RELEASE, event.pos[0], event.pos[1])
        elif event.type == pygame.MOUSEMOTION and pressed:
            consumer.event(E_MOVE, event.pos[0], event.pos[1])
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_z:
                consumer.event(E_PRESS_X, 0, 0)
            elif event.key == pygame.K_x:
                consumer.event(E_PRESS_Y, 0, 0)
            elif event.key == pygame.K_UP:
                consumer.event(E_PRESS_UP, 0, 0)
            elif event.key == pygame.K_DOWN:
                consumer.event(E_PRESS_DOWN, 0, 0)
            elif event.key == pygame.K_LEFT:
                consumer.event(E_PRESS_LEFT, 0, 0)
            elif event.key == pygame.K_RIGHT:
                consumer.event(E_PRESS_RIGHT, 0, 0)
        elif event.type == pygame.KEYUP:
            if event.key == pygame.K_z:
                consumer.event(E_REL_X, 0, 0)
            elif event.key == pygame.K_x:
                consumer.event(E_REL_Y, 0, 0)
            elif event.key == pygame.K_UP:
                consumer.event(E_REL_UP, 0, 0)
            elif event.key == pygame.K_DOWN:
                consumer.event(E_REL_DOWN, 0, 0)
            elif event.key == pygame.K_LEFT:
                consumer.event(E_REL_LEFT, 0, 0)
            elif event.key == pygame.K_RIGHT:
                consumer.event(E_REL_RIGHT, 0, 0)
        elif event.type == pygame.QUIT:
            pygame.quit()
            sys.exit(0)
        else:
            continue

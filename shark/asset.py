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

import os
import pygame
from shark.text import font
from shark.texture import texture
from pyshark.runtime import import_module

err = import_module("system.error")

ERR_ASSET_NOT_FOUND = 1000

__ASSET_PATH__ = "."

def load_texture(name):
    try:
        return texture(pygame.image.load(os.path.join(__ASSET_PATH__, name)))
    except pygame.error:
        err.set_err(ERR_ASSET_NOT_FOUND)
        return None

def load_font(name, size, color):
    try:
        return font(pygame.font.Font(os.path.join(__ASSET_PATH__, name), size), size, color)
    except IOError:
        err.set_err(ERR_ASSET_NOT_FOUND)
        return None

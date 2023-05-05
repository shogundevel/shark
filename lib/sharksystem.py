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
    print(data, end = '')

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

if hasattr(time, "clock"):
    clock = time.clock
else:
    def clock():
        raise NotImplementedError()

def free(value):
    pass

free_strbuf = free
free_striter = free
free_strmap = free
free_list = free

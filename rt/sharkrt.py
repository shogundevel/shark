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
    print(data, end = '')

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

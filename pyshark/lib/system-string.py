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
from pyshark.runtime import *
import pyshark.runtime as __shark_rt__

ERR_INVALID_LITERAL = 2
CHAR_SIZE = 4

def itos(x):
  return str(x)

def ftos(x):
  return str(x)

def ctos(x):
  return x

def stoi(x):
  try:
    return int(x)
  except ValueError:
    return None

def stof(x):
  try:
    return float(x)
  except ValueError:
    return None

def islower(x):
  return x >= 'a' and x <= 'z'

def isupper(x):
  return x >= 'A' and x <= 'Z'

def isalpha(x):
  return islower(x) or isupper(x)

def isdigit(x):
  return x >= '0' and x <= '9'

def isalnum(x):
  return isalpha(x) or isdigit(x)

def isident(x):
  return isalnum(x) or x == '_'

def ishex(x):
  return isdigit(x) or (x >= 'a' and x <= 'f') or (x >= 'A' and x <= 'F')

def isascii(x):
  return len(x.encode()) == 1

def issurrogate(x):
  return False

def tolower(x):
  return x.tolower()

def toupper(x):
  return x.toupper()

len = len

def index(data, index):
  return data[index]

def slice(data, start, end):
  return data[start:end]

def find(x, y):
  return x.find(y)

def concat(x, y):
  return x + y

def join(sep, args):
  return sep.join(args)

def split(s, sep):
  return s.split(sep)

def format(x, y):
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

def normal(data):
    return eval(data)

def quote(data):
    return repr(data)

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

class bytes:
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

def encode(data):
    bytebuf = bytes()
    bytebuf.buffer = bytearray(data.encode())
    return bytebuf

def decode(data):
    return data.buffer.decode()

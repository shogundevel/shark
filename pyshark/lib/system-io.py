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
__temp_0__ = __shark_rt__.import_module('system.error')
ERR_UNKNOWN = __temp_0__.ERR_UNKNOWN
set_err = __temp_0__.set_err
__temp_1__ = __shark_rt__.import_module('system.string')
format = __temp_1__.format
bytes = __temp_1__.bytes
ERR_FILE_NOT_FOUND = 2
class text_file:
  def __init__(self, stream):
    self.stream = stream
  def size(self):
    return -1
  def put(self, value):
    self.stream.write(value)
  def puts(self, data):
    self.stream.write(data)
  def printf(self, data, args):
    self.stream.write(format(data, args))
  def fetch(self):
    __local_0__ = self.stream.read(1)
    if __local_0__ == "":
      return '\0'
    else:
      return __local_0__
  def read(self, count):
    return self.stream.read(count)
  def read_all(self):
    return self.stream.read()
  def at_end(self):
    return __feof__(self)
  def close(self):
    self.stream.close()

class binary_file:
    def __init__(self, stream):
        self.stream = stream
    
    def put(self, value):
        self.stream.write(bytearray([value]))
    
    def puts(self, data):
        self.stream.write(data.buffer)
    
    def fetch(self):
        try:
            return self.stream.read(1)[0]
        except IndexError:
            return 0
    
    def read(self, count):
        r = bytes()
        r.buffer = bytearray(self.stream.read(count))
        return r
    
    def at_end(self):
        return __feof__(self)
    
    def close(self):
        self.stream.close()

def __feof__(self):
    __local_0__ = self.stream.tell()
    __local_1__ = False
    if not self.stream.read(1):
      __local_1__ = True
    self.stream.seek(__local_0__)
    return __local_1__

__py_open__ = open
def open(filename, mode):
  __local_0__ = None
  try:
    __local_0__ = __py_open__(filename, mode)
  except Exception:
    set_err(ERR_UNKNOWN)
    return None
  if "b" in mode:
    return binary_file(__local_0__)
  else:
    return text_file(__local_0__)
__py_print__ = print
def put(value):
  __py_print__(value, end = '')
def puts(data):
  __py_print__(data, end = '')
def printf(data, args):
  __py_print__(format(data, args), end = '')
def fetch():
  return __py_input__()
def read_line():
  return __py_input__()
def input(prompt):
  return __py_input__(prompt)

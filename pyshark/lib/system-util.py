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

def copy(self):
  if isinstance(self, list):
    return self[:]
  return self.copy()
def slice(self, start, end):
  return self[start:end]
def index(self, index):
  return self[index]
def pop(self):
  return self.pop()
def popindex(self, index):
  return self.pop(index)
def find(self, value):
  return self.find(value)
def count(self, value):
  return self.count(value)
def concat(self, other):
  return self + other
def extend(self, other):
  self.extend(other)
def reverse(self):
  self.reverse()
def sort(self, comp):
  self.sort()
def map_sort(self, comp, map):
  self.sort(map.map)
def remove(self, index):
  return self.pop(index)
def update(self, other):
  self.update(other)

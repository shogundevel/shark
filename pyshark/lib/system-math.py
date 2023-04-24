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

import math

pi = math.pi
e = math.e

abs = abs

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

min = min
max = max

import random as pyrandom

def random(n):
    return pyrandom.randint(0, n - 1)

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

os = __shark_rt__.import_module('os')
sep = os.path.sep

def get_base(path):
  return os.path.split(path)[0]

def get_tail(path):
  return os.path.split(path)[1]

def get_ext(path):
  return os.path.splitext(path)[1]

def remove_ext(path):
  return os.path.splitext(path)[0]

def join(base, tail):
  return os.path.join(base, tail)

def getdir():
    return os.getcwd()

def setdir(path):
    os.chdir(path)

def listdir(path):
    try:
        if path is None:
            return os.listdir()
        else:
            return os.listdir(path)
    except:
        return None

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

import shutil

def copy(source, target):
    try:
        shutil.copy(source, target)
        return True
    except:
        return False

def unlink(filename):
    try:
        os.unlink(filename)
        return True
    except:
        return False

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

ERR_NONE = 0
ERR_UNKNOWN = 1

__error__ = 0

def get_err():
    return __error__

def has_err():
    return __error__ != ERR_NONE

def set_err(value):
    global __error__
    __error__ = value

def clear_err():
    set_err(0)

class error_t (Exception):
    def __init__(self, message):
        super().__init__(message)
        self.message = message

def error(message):
    raise error_t (message)

def pcall(callee, args):
    try:
        return callee(*args)
    except error_t as err:
        set_err(ERR_UNKNOWN)
        return err.message

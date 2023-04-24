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
import importlib

from .compiler import compile as shark_compile

base = os.path.split(__file__)[0]

__import_path = [ os.path.join(base, "lib") ]
__import_record = { }

def add_import_path(import_path):
    __import_path.append(import_path)

class module:
    def __init__(self, filename, path):
        self.__file__ = filename
        self.__path__ = path
        self.__this__ = self

def load_module(filename, import_path):
    mod = module(filename, import_path)
    __import_record[import_path] = mod
    with open(filename, "r") as source:
        exec(compile(source.read(), filename, "exec"), mod.__dict__)
    return mod

def import_module(module_path):
    try:
        return __import_record[module_path]
    except KeyError:
        split = module_path.split('.')
        python_tail = '-'.join(split) + '.py'
        shark_tail = '/'.join(split) + '.shk'
        for import_path in reversed(__import_path):
            filename = os.path.join(import_path, python_tail)
            if os.path.isfile(filename):
                module = load_module(filename, module_path)
                break
            filename = os.path.join(import_path, shark_tail)
            if os.path.isfile(filename):
                out = os.path.join(base, "temp", python_tail)
                shark_compile(filename, out, False)
                module = load_module(out, module_path)
                break
            continue
        else:
            module = importlib.import_module(module_path)
        return module

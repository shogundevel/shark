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

import sys
import os

sys.path.append(os.path.split(os.path.split(__file__)[0])[0])

from pyshark.runtime import *

def exec_file(filename, source_file, args):
    try:
        source = open(filename, "r")
    except IOError as err:
        print("can't open file '%s' for reading, execution aborted." % filename)
        return
    context = { }
    context["__file__"] = source_file
    exec(compile(source.read(), filename, "exec"), context)
    return context

def clear_temp():
    tempdir = os.path.join(base, "temp")
    for file in os.listdir(tempdir):
        try:
            os.unlink(os.path.join(tempdir, file))
        except:
            pass

def main(args):
    if len(args) == 1:
        print("usage: pyshark.main <filename> <args>")
        return
    filename, args = args[1], args[2:]
    sys.argv = sys.argv[1:]
    ext = os.path.splitext(filename)[1]
    if ext == ".py":
        exec_file(filename, filename, args)
    elif ext == ".shk":
        out = os.path.join(base, "temp", os.path.splitext(os.path.split(filename)[1])[0] + '.py')
        shark_compile(filename, out, True)
        try:
            exec_file(out, filename, args)
        except Exception:
            clear_temp()
            raise
        clear_temp()
    else:
        print("invalid input file '%s', expected a '.py' or '.shk' source file.")

if __name__ == "__main__":
    main(sys.argv)

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

from studio.core import Application
from studio.ui import Studio, DarkCodeEditorStyle

class Main (Application):
    x_size = 960
    y_size = 640
    def __init__(self):
        Application.__init__(self)
        self.add_child(Studio(0, 0, self.x_size, self.y_size, DarkCodeEditorStyle()))

def main():
    import sys
    if len(sys.argv) == 2:
        import os
        import studio.ui
        studio.ui.project_dir = sys.argv[1]
        if not os.path.isdir(studio.ui.project_dir):
            print("invalid projects directory.")
            print("\tusage: <shark-studio> <projects-dir>")
            return
    else:
        import os
        import studio.ui
        studio.ui.project_dir = os.path.split(sys.argv[0])[0] + "/demo"
    try:
        Main().execute()
    except Exception as err:
        print(err)
        input()

if __name__ == "__main__":
    main()

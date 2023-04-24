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

import re

__identifier = re.compile("[a-zA-Z_]+[a-zA-Z0-9_]*")
__hex_lit = re.compile("0(x|X)[0-9a-fA-F]+")
__dec_lit = re.compile("(0|([1-9]+[0-9]*))")
__byte_lit = re.compile("0(b|B)[0-9a-fA-F][0-9a-fA-F]")
__float_tail = re.compile("[0-9]+")

__keywords = ["package", "import", "class", "pass", "var", "function",
              "if", "then", "else", "loop", "while", "do", "for", "in", "range",
              "break", "continue", "delete", "return", "not", "and", "or",
              "self", "super", "new", "instanceof", "sizeof",
              "null", "true", "false"]

__delimiter = ['->', '::', ':', ';', '.', ',',
               '[', ']', '{', '}', '(', ')']

__eq_operator = ['<=', '>=', '==', '!=']

__assign = ['*=', '/=', '%=', '+=', '-=', '=', '<<']

__operator = ['*', '/', '%', '+', '-',
              '<', '>', '&', '^', '|', '~']

__separator = __delimiter + __eq_operator + __assign + __operator
__separator_set = set(__separator)

def split(source):
    lines = source.split("\n")
    for index in range(len(lines)):
        line = lines[index]
        result = [ ]
        in_string = False
        delimiter = None
        escape = False
        start = 0
        for i in range(len(line)):
            char = line[i]
            if char in '"\'' and not escape:
                if delimiter == char:
                    result.append(line[start:i + 1])
                    delimiter = None
                    start = i + 1
                    in_string = False
                elif not in_string:
                    result.append(line[start:i])
                    in_string = True
                    delimiter = char
                    start = i
            elif char == "\\":
                if in_string and not escape:
                    escape = True
                    continue
            elif char == '#' and not in_string:
                result.append(line[start:i])
                start = i
                break
            escape = False
        last = line[start:]
        if not last.startswith('#'):
            result.append(last)
        lines[index] = result
    for index in range(len(lines)):
        line = lines[index]
        result = [ ]
        result.append(" " * (len(line[0]) - len(line[0].lstrip(" "))))
        for token in line:
            if not token.startswith("'") and not token.startswith('"'):
                result.extend(token.split())
            else:
                result.append(token)
        lines[index] = result
    for sep in __separator:
        for index in range(len(lines)):
            line = lines[index]
            result = [ ]
            for token in line:
                if token in __separator_set:
                    result.append(token)
                    continue
                elif token.startswith('"') or token.startswith("'"):
                    result.append(token)
                    continue
                split = token.split(sep)
                for i in range(len(split) - 1):
                    result.append(split[i])
                    result.append(sep)
                if len(split) >= 1:
                    result.append(split[-1])
            lines[index] = result
    return lines

class token:
    def __init__(self, lineno, type, value):
        self.lineno = lineno
        self.type = type
        self.value = value
    
    def __repr__(self):
        return "(%s, %s, %s)" % (self.lineno, self.type, self.value)

def fullmatch(p, s):
    m = p.match(s)
    if m and m.end() == len(s):
        return True
    else:
        return False

def tokenize(source):
    lines = split(source)
    tokens = [ ]
    lineno = 1
    indent_stack = [ 0 ]
    indent_level = 0
    delimiter_count = 0
    empty_line = True
    
    def emit(type, value):
        nonlocal empty_line
        if empty_line:
            empty_line = False
            if not delimiter_count:
                indent()
        tokens.append(token(lineno, type, value))
    
    def indent():
        if indent_level > indent_stack[-1]:
            tokens.append(token(lineno, "indent", ""))
            indent_stack.append(indent_level)
            return
        elif indent_level == indent_stack[-1]:
            return
        else:
            while indent_level < indent_stack[-1]:
                tokens.append(token(lineno, "dedent", ""))
                indent_stack.pop()
            if indent_level != indent_stack[-1]:
                tokens.append(token(lineno, "dedent-error", ""))
    
    for line in lines:
        indent_level = len(line[0])
        empty_line = True
        position = 1
        while position < len(line):
            c = line[position]
            decimal = fullmatch(__dec_lit, c)
            if not c:
                pass
            elif c in __keywords:
                emit("keyword", c)
            elif c in __delimiter:
                if c in "([{":
                    delimiter_count += 1
                elif c in ")]}":
                    delimiter_count -= 1
                emit("delimiter", c)
            elif c in __assign:
                emit("assign", c)
            elif c in __eq_operator or c in __operator:
                emit("operator", c)
            elif fullmatch(__identifier, c):
                emit("identifier", c)
            elif c.startswith('"'):
                if not c.endswith('"'):
                    emit("error", c)
                else:
                    emit("string", c)
            elif c.startswith("'"):
                if not c.endswith("'"):
                    emit("error", c)
                else:
                    emit("char", c)
            elif fullmatch(__byte_lit, c):
                emit("byte", c)
            elif fullmatch(__hex_lit, c) or decimal:
                if (decimal and len(line) > position + 1
                and line[position + 1] == '.'):
                    position += 2
                    if (position < len(line)
                    and fullmatch(__float_tail, line[position])):
                        emit("float", c + '.' + line[position])
                        position += 1
                    else:
                        emit("error", c + '.')
                    continue
                else:
                    emit("int", c)
                    position += 1
                    continue
            else:
                emit("error", c)
            position += 1
        if not empty_line and not delimiter_count:
            emit("newline", "")
        lineno += 1
    indent_level = 0
    indent()
    tokens.append(token(lineno, "delimiter", "EOF"))
    return tokens

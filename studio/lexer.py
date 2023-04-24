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

import re

__identifier = re.compile("[a-zA-Z_]+[a-zA-Z0-9_]*")
__hex_lit = re.compile("0(x|X)[0-9a-fA-F]+")
__dec_lit = re.compile("(0|([1-9]+[0-9]*))")
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

__whitespace = [" ", "\t", "\r"]

__separator = __delimiter + __eq_operator + __assign + __operator + __whitespace
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
            comment = False
        result.append(line[start:])
        lines[index] = result
    for sep in __separator:
        for index in range(len(lines)):
            line = lines[index]
            result = [ ]
            for token in line:
                if not token:
                    continue
                elif token in __separator_set:
                    result.append(token)
                    continue
                elif token.startswith('"') or token.startswith("'") or token.startswith("#"):
                    result.append(token)
                    continue
                split = token.split(sep)
                result.append(split[0])
                for i in range(1, len(split)):
                    result.append(sep)
                    result.append(split[i])
            lines[index] = result
    return lines

def fullmatch(p, s):
    m = p.match(s)
    if m and m.end() == len(s):
        return True
    else:
        return False

def tokenize(source):
    lines = split(source)
    tokens = [ ]
    position = 0
    
    def emit(type, value):
        tokens.append((type, value))
    
    for line in lines:
        position = 0
        while position < len(line):
            c = line[position]
            decimal = fullmatch(__dec_lit, c)
            if not c:
                pass
            elif c in __whitespace:
                emit("space", c)
            elif c in __keywords:
                emit("keyword", c)
            elif c in __delimiter:
                emit("delimiter", c)
            elif c in __assign:
                emit("assign", c)
            elif c in __eq_operator or c in __operator:
                emit("operator", c)
            elif fullmatch(__identifier, c):
                emit("identifier", c)
            elif c.startswith('#'):
                emit("comment", c)
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
            elif fullmatch(__hex_lit, c) or decimal:
                if (decimal and len(line) > position + 1
                and line[position + 1] == '.'):
                    position += 2
                    if (position < len(line)
                    and fullmatch(__float_tail, line[position])):
                        emit("number", c + '.' + line[position])
                        position += 1
                    else:
                        emit("error", c + '.')
                    continue
                else:
                    emit("number", c)
                    position += 1
                    continue
            else:
                emit("error", c)
            position += 1
    return tokens

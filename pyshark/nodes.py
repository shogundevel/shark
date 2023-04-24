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

class node:
    def __init__(self, lineno):
        self.lineno = lineno
    
    def accept(self, visitor):
        raise NotImplementedError()

class source_file(node):
    def __init__(self, filename, package, imports, source):
        self.lineno = 0
        self.filename = filename
        self.package = package
        self.imports = imports
        self.source = source
    
    def accept(self, visitor):
        return visitor.source_file(self)

class import_decl(node):
    def __init__(self, lineno, import_path, alias, target):
        self.lineno = lineno
        self.import_path = import_path
        self.alias = alias
        self.target = target
    
    def accept(self, visitor):
        return visitor.import_decl(self)

class custom_type(node):
    def __init__(self, lineno, path):
        self.lineno = lineno
        self.path = path
    
    def accept(self, visitor):
        return visitor.custom_type(self)

class class_decl(node):
    def __init__(self, lineno, name, super, body):
        self.lineno = lineno
        self.name = name
        self.super = super
        self.body = body
    
    def accept(self, visitor):
        return visitor.class_decl(self)

class var(node):
    def __init__(self, lineno, name, value):
        self.lineno = lineno
        self.name = name
        self.value = value
    
    def accept(self, visitor):
        return visitor.var(self)

class function(node):
    def __init__(self, lineno, name, args, body):
        self.lineno = lineno
        self.name = name
        self.args = args
        self.body = body
    
    def accept(self, visitor):
        return visitor.function(self)

class block(node):
    def __init__(self, lineno, body):
        self.lineno = lineno
        self.body = body
    
    def accept(self, visitor):
        return visitor.block(self)

class if_stat(node):
    def __init__(self, lineno, cond, then, else_):
        self.lineno = lineno
        self.cond = cond
        self.then = then
        self.else_ = else_
    
    def accept(self, visitor):
        return visitor.if_stat(self)

class while_stat(node):
    def __init__(self, lineno, cond, body):
        self.lineno = lineno
        self.cond = cond
        self.body = body
    
    def accept(self, visitor):
        return visitor.while_stat(self)

class for_stat(node):
    def __init__(self, lineno, name, iter, body):
        self.lineno = lineno
        self.name = name
        self.iter = iter
        self.body = body
    
    def accept(self, visitor):
        return visitor.for_stat(self)

class range_exp(node):
    def __init__(self, lineno, start, stop):
        self.lineno = lineno
        self.start = start
        self.stop = stop
    
    def accept(self, visitor):
        return visitor.range_exp(self)

class break_stat(node):
    def accept(self, visitor):
        return visitor.break_stat(self)

class continue_stat(node):
    def accept(self, visitor):
        return visitor.continue_stat(self)

class delete(node):
    def __init__(self, lineno, target):
        self.lineno = lineno
        self.target = target
    
    def accept(self, visitor):
        return visitor.delete(self)

class return_stat(node):
    def __init__(self, lineno, value):
        self.lineno = lineno
        self.value = value
    
    def accept(self, visitor):
        return visitor.return_stat(self)

class insert_stat(node):
    def __init__(self, lineno, target, value):
        self.lineno = lineno
        self.target = target
        self.value = value
    
    def accept(self, visitor):
        return visitor.insert_stat(self)

class assign_stat(node):
    def __init__(self, lineno, op, target, value):
        self.lineno = lineno
        self.op = op
        self.target = target
        self.value = value
    
    def accept(self, visitor):
        return visitor.assign_stat(self)

class exp_stat(node):
    def __init__(self, lineno, exp):
        self.lineno = lineno
        self.exp = exp
    
    def accept(self, visitor):
        return visitor.exp_stat(self)

class binary_op(node):
    def __init__(self, lineno, op, left, right):
        self.lineno = lineno
        self.op = op
        self.left = left
        self.right = right
    
    def accept(self, visitor):
        return visitor.binary_op(self)

class unary_op(node):
    def __init__(self, lineno, op, child):
        self.lineno = lineno
        self.op = op
        self.child = child
    
    def accept(self, visitor):
        return visitor.unary_op(self)

class function_call(node):
    def __init__(self, lineno, source, args):
        self.lineno = lineno
        self.source = source
        self.args = args
    
    def accept(self, visitor):
        return visitor.function_call(self)

class slice_op(node):
    def __init__(self, lineno, source, start, end):
        self.lineno = lineno
        self.source = source
        self.start = start
        self.end = end
    
    def accept(self, visitor):
        return visitor.slice_op(self)

class index_op(node):
    def __init__(self, lineno, source, index):
        self.lineno = lineno
        self.source = source
        self.index = index
    
    def accept(self, visitor):
        return visitor.index_op(self)

class get_field(node):
    def __init__(self, lineno, exp, field):
        self.lineno = lineno
        self.exp = exp
        self.field = field
    
    def accept(self, visitor):
        return visitor.get_field(self)

class address(node):
    def __init__(self, lineno, path):
        self.lineno = lineno
        self.path = path
    
    def accept(self, visitor):
        return visitor.address(self)

class self_exp(node):
    def accept(self, visitor):
        return visitor.self_exp(self)

class super_call(node):
    def __init__(self, lineno, args):
        self.lineno = lineno
        self.args = args
    
    def accept(self, visitor):
        return visitor.super_call(self)

class sizeof(node):
    def __init__(self, lineno, value):
        self.lineno = lineno
        self.value = value
    
    def accept(self, visitor):
        return visitor.sizeof(self)

class new(node):
    def __init__(self, lineno, type, args):
        self.lineno = lineno
        self.type = type
        self.args = args
    
    def accept(self, visitor):
        return visitor.new(self)

class instanceof(node):
    def __init__(self, lineno, value, type):
        self.lineno = lineno
        self.value = value
        self.type = type
    
    def accept(self, visitor):
        return visitor.instanceof(self)

class array_exp(node):
    def __init__(self, lineno, values):
        self.lineno = lineno
        self.values = values
    
    def accept(self, visitor):
        return visitor.array_exp(self)

class table_exp(node):
    def __init__(self, lineno, values):
        self.lineno = lineno
        self.values = values
    
    def accept(self, visitor):
        return visitor.table_exp(self)

class nested_exp(node):
    def __init__(self, lineno, exp):
        self.lineno = lineno
        self.exp = exp
    
    def accept(self, visitor):
        return visitor.nested_exp(self)

class literal(node):
    def __init__(self, lineno, type, value):
        self.lineno = lineno
        self.type = type
        self.value = value
    
    def accept(self, visitor):
        return visitor.literal(self)

class visitor:
    def source_file(self, node):
        pass
    
    def import_decl(self, node):
        pass
    
    def custom_type(self, node):
        pass
    
    def class_decl(self, node):
        pass
    
    def var(self, node):
        pass
    
    def function(self, node):
        pass
    
    def block(self, node):
        pass
    
    def if_stat(self, node):
        pass
    
    def loop_stat(self, node):
        pass
    
    def while_stat(self, node):
        pass
    
    def do_while_stat(self, node):
        pass
    
    def for_stat(self, node):
        pass
    
    def range_exp(self, node):
        pass
    
    def break_stat(self, node):
        pass
    
    def continue_stat(self, node):
        pass
    
    def delete(self, node):
        pass
    
    def return_stat(self, node):
        pass
    
    def insert_stat(self, node):
        pass
    
    def assign_stat(self, node):
        pass
    
    def exp_stat(self, node):
        pass
    
    def binary_op(self, node):
        pass
    
    def unary_op(self, node):
        pass
    
    def function_call(self, node):
        pass
    
    def slice_op(self, node):
        pass
    
    def index_op(self, node):
        pass
    
    def get_field(self, node):
        pass
    
    def address(self, node):
        pass
    
    def self_exp(self, node):
        pass
    
    def super_call(self, node):
        pass
    
    def sizeof(self, node):
        pass
    
    def new(self, node):
        pass
    
    def instanceof(self, node):
        pass
    
    def array_exp(self, node):
        pass
    
    def table_exp(self, node):
        pass
    
    def nested_exp(self, node):
        pass
    
    def literal(self, node):
        pass

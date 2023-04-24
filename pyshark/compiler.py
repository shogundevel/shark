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

from .parser import *
from .nodes import *

class block_context:
    def __init__(self, parent):
        self.parent = parent
        self.namespace = { }

class compiler:
    def __init__(self):
        self.filename = None
        self.main = False
        self.out = None
        self.indent_level = 0
        self.local_id = 0
        self.context = None
        self.class_context = None
        self.function_context = None
        self.globals = None
        self.assign_target = False
    
    def compile(self, filename, out, main):
        self.filename = filename
        self.main = main
        with open(out, "w") as output:
            self.out = output
            self.indent_level = 0
            self.local_id = 0
            self.context = None
            self.function_context = None
            self.globals = None
            self.assign_target = False
            with open(filename, "r") as source:
                self.source_file(parse(filename, source.read()))
    
    def print(self, line):
        self.out.write("  " * self.indent_level)
        self.out.write(line)
        self.out.write("\n")
    
    def indent(self):
        self.indent_level += 1
    
    def dedent(self):
        self.indent_level -= 1
    
    def get_local(self):
        local = "__local_%d__" % self.local_id
        self.local_id += 1
        return local
    
    def enter(self):
        self.context = block_context(self.context)
    
    def exit(self):
        self.context = self.context.parent
    
    def define(self, name):
        if self.context is not None:
            local_id = self.get_local()
            self.context.namespace[name] = local_id
            return local_id
        else:
            return name
    
    def source_file(self, node):
        self.print("import pyshark.runtime as __shark_rt__")
        if self.main:
            self.print("import os")
            self.print("__shark_rt__.add_import_path(os.path.split(__file__)[0])")
        for import_decl in node.imports:
            import_decl.accept(self)
        if self.main:
            self.print("main = None")
        for child in node.source:
            child.accept(self)
        if self.main:
            self.print("if main:")
            self.indent()
            self.print("import sys")
            self.print("main(sys.argv)")
            self.dedent()
    
    def import_decl(self, node):
        if node.target == None:
            name = node.alias or node.import_path[-1]
        else:
            name = self.get_local()
        self.print("%s = __shark_rt__.import_module('%s')" % (name, ".".join(node.import_path)))
        if node.target != None:
            for target in node.target:
                self.print("%s = %s.%s" % (target, name, target))
    
    def custom_type(self, node):
        return ".".join(node.path)
    
    def class_decl(self, node):
        super = ""
        if node.super != None:
            super = '(%s)' % node.super.accept(self)
        self.print("class %s%s:" % (node.name, super))
        self.indent()
        if not node.body:
            self.print("pass")
        else:
            self.class_context = node
            for child in node.body:
                child.accept(self)
            self.class_context = None
        self.dedent()
    
    def var(self, node):
        name = self.define(node.name)
        self.print("%s = %s" % (name, node.value.accept(self) if node.value != None else "None"))
    
    def function(self, node):
        args = ", ".join(node.args if not self.class_context else [ "self" ] + node.args)
        self.print("def %s(%s):" % (node.name, args))
        self.indent()
        if node.body:
            self.local_id = 0
            self.function_context = node
            self.globals = set()
            self.enter()
            for arg in node.args:
                self.context.namespace[arg] = arg
            node.body.accept(self)
            self.exit()
            self.function_context = None
        else:
            self.print("raise NotImplementedError()")
        self.dedent()
        if node.name == "init" and self.class_context:
            self.print("def __init__(%s):" % args)
            self.indent()
            self.print("self.init(%s)" % ", ".join(node.args))
            self.dedent()
    
    def block(self, node):
        if not node.body:
            self.print("pass")
        else:
            self.enter()
            for child in node.body:
                child.accept(self)
            self.exit()
    
    def if_stat(self, node):
        self.print("if %s:" % node.cond.accept(self))
        self.indent()
        node.then.accept(self)
        self.dedent()
        while isinstance(node.else_, if_stat):
            node = node.else_
            self.print("elif %s:" % node.cond.accept(self))
            self.indent()
            node.then.accept(self)
            self.dedent()
        if isinstance(node.else_, block):
            self.print("else:")
            self.indent()
            node.else_.accept(self)
            self.dedent()
    
    def while_stat(self, node):
        self.print("while %s:" % node.cond.accept(self))
        self.indent()
        node.body.accept(self)
        self.dedent()
    
    def for_stat(self, node):
        self.enter()
        name = self.define(node.name)
        self.print("for %s in %s:" % (name, node.iter.accept(self)))
        self.indent()
        node.body.accept(self)
        self.exit()
        self.dedent()
    
    def range_exp(self, node):
        stop = node.stop.accept(self)
        if node.start != None:
            start = node.start.accept(self)
            return "range(%s, %s)" % (start, stop)
        return "range(%s)" % stop
    
    def break_stat(self, node):
        self.print("break")
    
    def continue_stat(self, node):
        self.print("continue")
    
    def delete(self, node):
        node.target.accept(self)
    
    def return_stat(self, node):
        if node.value != None:
            self.print("return %s" % node.value.accept(self))
        else:
            self.print("return")
    
    def insert_stat(self, node):
        value = node.value.accept(self)
        if isinstance(node.target, index_op):
            self.print("%s.insert(%s, %s)" % (
                node.target.source.accept(self), node.target.index.accept(self), value))
        else:
            self.print("%s.append(%s)" % (node.target.accept(self), value))
    
    def assign_stat(self, node):
        if isinstance(node.target, address) and len(node.target.path) == 1:
            target = self.search(node.target.path[0])
        else:
            self.assign_target = True
            target = node.target.accept(self)
            self.assign_target = False
        self.print("%s %s %s" % (target, node.op, node.value.accept(self)))
    
    def exp_stat(self, node):
        self.print(node.exp.accept(self))
    
    def binary_op(self, node):
        op = node.op
        if op == "not": op = "not in"
        return "%s %s %s" % (node.left.accept(self), op, node.right.accept(self))
    
    def unary_op(self, node):
        if node.op == "not":
            return "not %s" % node.child.accept(self)
        else:
            return "%s%s" % (node.op, node.child.accept(self))
    
    def function_call(self, node):
        args = ", ".join([arg.accept(self) for arg in node.args])
        return "%s(%s)" % (node.source.accept(self), args)
    
    def slice_op(self, node):
        return "%s[%s:%s]" % (node.source.accept(self), node.start.accept(self), node.end.accept(self))
    
    def index_op(self, node):
        return "%s[%s]" % (node.source.accept(self), node.index.accept(self))
    
    def get_field(self, node):
        return "%s.%s" % (node.exp.accept(self), node.field)
    
    def search(self, name):
        context = self.context
        while context != None:
            if name in context.namespace:
                return context.namespace[name]
            context = context.parent
        if self.function_context != None and self.assign_target and not name in self.globals:
            self.print("global %s" % name)
            self.globals.add(name)
        return name
    
    def address(self, node):
        base = node.path[0]
        prev_assign_target = self.assign_target
        self.assign_target = False
        node.path[0] = self.search(base)
        self.assign_target = prev_assign_target
        path = ".".join(node.path)
        node.path[0] = base
        return path
    
    def self_exp(self, node):
        return "self"
    
    def super_call(self, node):
        return "super().%s(%s)" % (self.function_context.name, ", ".join([arg.accept(self) for arg in node.args]))
    
    def sizeof(self, node):
        return "len(%s)" % node.value.accept(self)
    
    def new(self, node):
        return "%s(%s)" % (node.type.accept(self), ", ".join([arg.accept(self) for arg in node.args]))
    
    def instanceof(self, node):
        return "isinstance(%s, %s)" % (node.value.accept(self), node.type.accept(self))
    
    def array_exp(self, node):
        return "[%s]" % ", ".join([value.accept(self) for value in node.values])
    
    def table_exp(self, node):
        return "{%s}" % ", ".join(["%s:%s" % (key.accept(self), value.accept(self) if value else "None") for key, value in node.values])
    
    def nested_exp(self, node):
        return "(%s)" % node.exp.accept(self)
    
    def literal(self, node):
        if node.type == "void":
            return "None"
        elif node.type == "bool":
            return "True" if node.value == "true" else "False"
        else:
            return node.value

__compiler = compiler()
compile = __compiler.compile

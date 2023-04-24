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

from .scanner import *
from .nodes import *

class parser_error (Exception):
    pass

class parser:
    def error(self, message):
        raise parser_error("File '%s', line %d in token %s: %s" % (self.filename, self.lineno, self.token, message))
    
    def next(self):
        value = self.token.value
        self.position += 1
        self.token = self.token_stream[self.position]
        self.lineno = self.token.lineno
        if self.token.type == "error":
            self.error("invalid syntax.")
        return value
    
    def match(self, value):
        if self.token.value == value:
            return self.next()
    
    def consume(self, type):
        if self.token.type != type:
            self.error("invalid token type, expected '%s' but got '%s'." % (type, self.token.type))
        return self.next()
    
    def expect(self, value):
        if not self.match(value):
            self.error("invalid token, expected '%s' but got '%s'." % (value, self.token.value))
    
    def eol(self):
        if not self.match(';') and not self.token.type == "newline":
            self.error("expected EOL but got '%s' token." % self.token.type)
        self.seol()
    
    def seol(self):
        if self.token.type == "newline":
            self.next()
    
    def at_end(self):
        return self.token.value == "EOF" and self.token.type == "delimiter"
    
    def match_indent(self):
        if self.token.type == "indent":
            self.next()
            return True
        return False
    
    def match_dedent(self):
        if self.token.type == "dedent":
            self.next()
            return True
        return False
    
    def indent(self):
        if not self.match_indent():
            self.error("expected an indented block.")
    
    def dedent(self, lineno):
        if not self.match_dedent():
            self.error("expected the end of the block (started at line %d)." % lineno)
    
    def parse(self, filename, source):
        self.filename = filename
        self.token_stream = tokenize(source)
        self.position = 0
        self.token = self.token_stream[0]
        self.lineno = self.token.lineno
        
        self.in_type = False
        self.in_function = False
        self.in_loop = False
        
        return self.source_file()
    
    # top level
    
    def source_file(self):
        package = None
        if self.match("package"):
            package = self.import_path()
            self.eol()
        imports = [ ]
        while self.match("import"):
            imports.append(self.import_decl())
        source = [ ]
        while not self.at_end():
            decl = self.decl()
            if not decl: self.error("expected global declaration or statement.")
            source.append(decl)
        return source_file(self.filename, package, imports, source)
    
    def import_path(self):
        import_path = [ self.consume("identifier") ]
        while self.match('.'):
            import_path.append(self.consume("identifier"))
        return import_path
    
    def import_decl(self):
        lineno = self.lineno
        import_path = self.import_path()
        alias = None
        target = None
        if self.match("->"):
            alias = self.consume("identifier")
        elif self.match(':'):
            target = [ self.consume("identifier") ]
            while self.match(','):
                target.append(self.consume("identifier"))
        self.eol()
        return import_decl(lineno, import_path, alias, target)
    
    def decl(self):
        return (self.class_decl() or self.var() or self.function() or self.statement())
    
    # types
    
    def type(self):
        lineno = self.lineno
        path = self.path()
        if not path: self.error("expected a custom type.")
        return custom_type(lineno, path)
    
    def path(self):
        if self.token.type == "identifier":
            path = [ self.next() ]
            while self.match('::'):
                path.append(self.consume("identifier"))
            return path
    
    # type definition
    
    def class_decl(self):
        if self.match("class"):
            lineno = self.lineno
            name = self.consume("identifier")
            super = None
            if self.match('('):
                super = self.type()
                self.expect(')')
            self.eol()
            if self.match_indent():
                body = [ ]
                if self.pass_decl():
                    self.dedent(lineno)
                else:
                    self.in_type = True
                    while not self.match_dedent():
                        decl = self.var() or self.function()
                        if not decl: self.error("expected a declaration in class body.")
                        body.append(decl)
                    self.in_type = False
            return class_decl(lineno, name, super, body)
    
    def pass_decl(self):
        if self.match("pass"):
            self.eol()
            return True
        else:
            return False
    
    # variables
    
    def var(self):
        if self.match("var"):
            lineno = self.lineno
            name = self.consume("identifier")
            value = None
            if self.match('='):
                if self.in_type and not self.in_function:
                    self.error("field declaration can't have an initializer.")
                value = self.exp()
            elif self.in_function:
                self.error("local variable should be initialized.")
            self.eol()
            return var(lineno, name, value)
    
    # functions
    
    def function(self):
        if self.match('function'):
            lineno = self.lineno
            name = self.consume("identifier")
            args = self.args()
            self.eol()
            body = None
            if self.token.type == 'indent':
                self.in_function = True
                body = self.indented_block()
                self.in_function = False
            return function(lineno, name, args, body)
    
    def args(self):
        self.expect('(')
        args = [ ]
        if not self.match(')'):
            args.append(self.consume("identifier"))
            while self.match(','):
                args.append(self.consume("identifier"))
            self.expect(')')
        return args
    
    # statements
    
    def block(self):
        lineno = self.lineno
        child = self.simple_statement()
        if child is not None:
            return block(lineno, [child])
        else:
            return self.indented_block()
    
    def indented_block(self):
        lineno = self.lineno
        self.seol()
        self.indent()
        body = [ ]
        if self.pass_decl():
            self.dedent(lineno)
        else:
            while not self.match_dedent():
                decl = self.var() or self.statement()
                if not decl: self.error("expected declaration or statement.")
                body.append(decl)
        return block(lineno, body)
    
    def statement(self):
        lineno = self.lineno
        if self.match("if"):
            cond = self.exp()
            self.expect("then")
            then = self.block()
            else_ = None
            if self.match("else"):
                if self.token.value == "if":
                    else_ = self.statement()
                else:
                    else_ = self.block()
            return if_stat(lineno, cond, then, else_)
        elif self.match("while"):
            cond = self.exp()
            self.expect("do")
            prev_in_loop = self.in_loop
            self.in_loop = True
            body = self.block()
            self.in_loop = prev_in_loop
            return while_stat(lineno, cond, body)
        elif self.match("for"):
            name = self.consume("identifier")
            self.expect("in")
            iter = self.iter_exp()
            self.expect("do")
            prev_in_loop = self.in_loop
            self.in_loop = True
            body = self.block()
            self.in_loop = prev_in_loop
            return for_stat(lineno, name, iter, body)
        else:
            return self.simple_statement()
    
    def iter_exp(self):
        return self.range_exp() or self.exp()
    
    def range_exp(self):
        if self.match("range"):
            lineno = self.lineno
            self.expect('(')
            start = None
            stop = self.exp()
            if self.match(','):
                start = stop
                stop = self.exp()
            self.expect(')')
            return range_exp(lineno, start, stop)
    
    def simple_statement(self):
        lineno = self.lineno
        if self.match("break"):
            if not self.in_loop: self.error("break statement outside loop.")
            self.eol()
            return break_stat(lineno)
        elif self.match("continue"):
            if not self.in_loop: self.error("continue statement outside loop.")
            self.eol()
            return continue_stat(lineno)
        elif self.match("delete"):
            target = self.exp()
            self.eol()
            return delete(lineno, target)
        elif self.match("return"):
            if not self.in_function:
                self.error("return statement outside function.")
            value = self.optional_exp()
            self.eol()
            return return_stat(lineno, value)
        else:
            return self.assign_stat()
    
    def assign_stat(self):
        node = self.assign()
        if not node: return None
        self.eol()
        return node
    
    def assign(self):
        lineno = self.lineno
        target = self.optional_exp()
        if not target: return None
        elif self.match("<<"):
            return insert_stat(lineno, target, self.exp())
        elif self.token.type == "assign":
            if not isinstance(target, (address, slice_op, index_op, get_field)):
                self.error("invalid assignment target.")
            elif isinstance(target, slice_op) and self.token.value != '=':
                self.error("invalid assign op in slice.")
            return assign_stat(lineno, self.next(), target, self.exp())
        return exp_stat(lineno, target)
    
    # expressions
    
    op_priority = {'or': 1,
                   'and': 2,
                   '==': 6,
                   '!=': 6,
                   'not': 6,
                   'in': 6,
                   '<': 7,
                   '<=': 7,
                   '>': 7,
                   '>=': 7,
                   '+': 9,
                   '-': 9,
                   '*': 10,
                   '/': 10,
                   '%': 10}
    
    def binary_op(self, priority = 0):
        exp = self.unary_op()
        while exp:
            lineno = self.lineno
            op = self.token.value
            if (op in parser.op_priority
                and parser.op_priority[op] >= priority):
                self.next()
                if op == "not": self.expect("in")
                exp = binary_op(lineno, op, exp,
                    self.binary_op(parser.op_priority[op]))
                if not exp: self.error("expected expression after '%s'." % op)
            else: break
        return exp
    
    unary_operators = {"not", '-'}
    
    def unary_op(self):
        lineno = self.lineno
        op = self.token.value
        if op in parser.unary_operators:
            self.next()
            child = self.post_op()
            if not child: self.error("expected expression after '%s'." % op)
            return unary_op(lineno, op, child)
        return self.post_op()
    
    def post_op(self):
        exp = self.primary_exp()
        while exp:
            lineno = self.lineno
            if self.match('('):
                args = [ ]
                if not self.match(')'):
                    args = [ self.exp() ]
                    while self.match(','):
                        args.append(self.exp())
                    self.expect(')')
                exp = function_call(lineno, exp, args)
            elif self.match('['):
                index = self.exp()
                if self.match(':'):
                    exp = slice_op(lineno, exp, index, self.exp())
                else:
                    exp = index_op(lineno, exp, index)
                self.expect(']')
            elif self.match('.'):
                exp = get_field(lineno, exp, self.consume("identifier"))
            else:
                break
        return exp
    
    def primary_exp(self):
        lineno = self.lineno
        exp = self.literal()
        if exp: return exp
        elif self.token.type == "identifier":
            return address(lineno, self.path())
        elif self.match("self"):
            if not self.in_type:
                self.error("self expression outside class.")
            return self_exp(lineno)
        elif self.match("super"):
            if not self.in_type:
                self.error("super call outside class.")
            args = [ ]
            self.expect('(')
            if not self.match(')'):
                args.append(self.exp())
                while self.match(','):
                    args.append(self.exp())
                self.expect(')')
            return super_call(lineno, args)
        elif self.match("sizeof"):
            self.expect('(')
            value = self.exp()
            self.expect(')')
            return sizeof(lineno, value)
        elif self.match("new"):
            type = self.type()
            self.expect('(')
            args = [ ]
            if not self.match(')'):
                args = [ self.exp() ]
                while self.match(','):
                    args.append(self.exp())
                self.expect(')')
            return new(lineno, type, args)
        elif self.match("instanceof"):
            self.expect('(')
            value = self.exp()
            self.expect(',')
            type = self.type()
            self.expect(')')
            return instanceof(lineno, value, type)
        elif self.match('['):
            values = [ ]
            if not self.match(']'):
                values.append(self.exp())
                while self.match(','):
                    values.append(self.exp())
                self.expect(']')
            return array_exp(lineno, values)
        elif self.match('{'):
            values = [ ]
            if not self.match('}'):
                key = self.exp()
                value = None
                if self.match(':'): value = self.exp()
                values.append((key, value))
                while self.match(','):
                    key = self.exp()
                    value = None
                    if self.match(':'): value = self.exp()
                    values.append((key, value))
                self.expect('}')
            return table_exp(lineno, values)
        elif self.match('('):
            exp = self.exp()
            self.expect(')')
            return nested_exp(lineno, exp)
        else:
            return None
    
    def literal(self):
        lineno = self.lineno
        if self.match("null"):
            return literal(lineno, "void", "null")
        elif self.token.value in ["true", "false"]:
            return literal(lineno, "bool", self.next())
        elif self.token.type == "int":
            return literal(lineno, "int", self.next())
        elif self.token.type == "float":
            return literal(lineno, "float", self.next())
        elif self.token.type == "char":
            return literal(lineno, "char", self.next())
        elif self.token.type == "string":
            return literal(lineno, "string", self.next())
        else:
            return None
    
    def optional_exp(self):
        return self.binary_op(0)
    
    def exp(self):
        exp = self.binary_op(0)
        if not exp: self.error("expected expression.")
        return exp

__parser = parser()
parse = __parser.parse

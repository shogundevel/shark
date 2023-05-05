--------------------------------------------------------------------------------
--- Copyright ------------------------------------------------------------------
-- 
-- Copyright 2022 Daniel Alvarez <shogundevel@gmail.com>
-- 
-- Permission is hereby granted, free of charge, to any person
-- obtaining a copy of this software and associated documentation files
-- (the "Software"), to deal in the Software without restriction,
-- including without limitation the rights to use, copy, modify, merge,
-- publish, distribute, sublicense, and/or sell copies of the Software,
-- and to permit persons to whom the Software is furnished to do so,
-- subject to the following conditions:
-- 
-- The above copyright notice and this permission notice shall be
-- included in all copies or substantial portions of the Software.
-- 
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
-- EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
-- MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
-- IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
-- CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
-- TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
-- SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
-- 
--------------------------------------------------------------------------------

EXIT_SUCCESS = 0
EXIT_FAILURE = -1

function exit(code)
    os.exit(code)
end

ERR_NONE = 0
ERR_UNKNOWN = 1

ERR = ERR_NONE

function get_err()
    return ERR
end

function set_err(value)
    ERR = value
end

function clear_err()
    set_err(ERR_NONE)
end

ERR_INVALID_LITERAL = 2

function stoi(x)
    return tonumber(x) or 0
end

function stof(x)
    return tonumber(x) or 0
end

function itos(x)
    return tostring(x)
end

function ftos(x)
    return tostring(x)
end

function ctoi(x)
    return byte(x, 1)
end

function itoc(x)
    return char(x)
end

function islower(x)
    return string.gmatch(x, "(%l)")() ~= nil
end

function isupper(x)
    return string.gmatch(x, "(%u)")() ~= nil
end

function tolower(x)
    return string.lower(x)
end

function toupper(x)
    return string.upper(x)
end

function isalpha(x)
    return string.gmatch(x, "(%a)")() ~= nil
end

function isdigit(x)
    return string.gmatch(x, "(%d)")() ~= nil
end

function isalnum(x)
    return string.gmatch(x, "(%w)")() ~= nil
end

function isident(x)
    return string.gmatch(x, "([%l%u%d_])")() ~= nil
end

strbuf = __shark_create_class("strbuf", __shark_object)
strbuf.constructor = "strbuf"

function strbuf.strbuf(self)
    self.buffer = { }
end

function strbuf.put(self, value)
    self.buffer[#self.buffer + 1] = value
end

function strbuf.write(self, data)
    self.buffer[#self.buffer + 1] = data
end

function strbuf.get(self)
    local data = table.concat(self.buffer)
    self.buffer = { }
    return data
end

striter = __shark_create_class("striter", __shark_object)
striter.constructor = "striter"

function striter.striter(self, data)
    self.data = data
    self.pos = 1
end

function striter.tell(self)
    return self.pos
end

function striter.seek(self, pos)
    self.pos = pos
end

function striter.next(self)
    local c = string.sub(self.data, self.pos, self.pos)
    self.pos = self.pos + 1
    return c
end

function striter.empty(self)
    return self.pos > string.len(self.data)
end

strmap = __shark_create_class("strmap", __shark_object)
strmap.constructor = "strmap"

function strmap.strmap(self)
    self.data = { }
end

function strmap.get(self, index)
    return self.data[index]
end

function strmap.set(self, index, value)
    self.data[index] = value
end

function strmap.has(self, index)
    return self.data[index] ~= nil
end

function strmap.pop(self, index)
    local value = self.data[index]
    self.data[index] = nil
    return value
end

ERR_FILE_NOT_FOUND = 3

FILE = __shark_create_class("FILE", __shark_object)
FILE.constructor = "FILE"

function FILE.FILE(self, stream)
    self.stream = stream
end

function FILE.close(self)
    self.stream:close()
end

function FILE.at_end(self)
    return self.stream:read(0) == nil
end

function FILE.put(self, value)
    self.stream:write(value)
end

function FILE.write(self, data)
    self.stream:write(data)
end

function FILE.fetch(self)
    return self.stream:read(1) or '\0'
end

function open(filename, mode)
    local stream = io.open(filename, mode)
    if stream == nil then
        set_err(ERR_FILE_NOT_FOUND)
        return nil
    end
    return __shark_object_new(FILE, stream)
end

function write(data)
    io.write(data)
end

function read()
    return io.read("*l")
end

list = __shark_create_class("list", __shark_object)
list.constructor = "list"

function list.list(self)
    self.data = { }
    self.length = 0
end

function list.size(self)
    return self.length
end

function list.push(self, value)
    self.data[self.length] = value
    self.length = self.length + 1
end

function list.pop(self)
    self.length = self.length - 1
    local value = self.data[self.length]
    self.data[self.length] = nil
    return value
end

function list.get(self, index)
    return self.data[index]
end

function list.set(self, index, value)
    self.data[index] = value
end

function idiv(x, y)
    return math.floor(x / y)
end

pi = math.pi
e = 2.718281

abs = math.abs

acos = math.acos
asin = math.asin
atan = math.atan
atan2 = math.atan2

cos = math.cos
cosh = math.cosh
sin = math.sin
sinh = math.sinh
tan = math.tan
tanh = math.tanh

exp = math.exp
log = math.log
log10 = math.log10
pow = math.pow
sqrt = math.sqrt

ceil = math.ceil
floor = math.floor

min = math.min
max = math.max

function random(x)
    return math.random(x) - 1
end

function clock()
    return os.clock()
end

function free(value) end

free_strbuf = free
free_striter = free
free_strmap = free
free_list = free

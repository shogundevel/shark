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

__shark_rt_imports = { }
__shark_rt_modules = { }

function __shark_rt_import(module)
    if __shark_rt_imports[module] then
        return __shark_rt_imports[module]
    elseif __shark_rt_modules[module] then
        return __shark_rt_modules[module]()
    else
        local sucess, mod = require(module)
        if not sucess then
            error("can't resolve import '" .. module .. "'.", 2)
        else
            return mod
        end
    end
end

__shark_object__ = { }

__shark_object__.__index = function (self, name)
    if rawget(self, name) then
        return rawget(self, name)
    else
        local meta = getmetatable(self)
        if meta[name] then
            return meta[name]
        else
            while meta.super ~= nil do
                meta = meta.super
                if meta[name] then
                    return meta[name]
                end
            end
            return nil
        end
    end
end

function __shark_rt_create_class(name, super)
    local class = { }
    class.name = name
    class.super = super
    class.__index = __shark_object__.__index
    return class
end

function __shark_object_new(class)
    local object = { }
    setmetatable(object, class)
    return object
end

function __shark_object_instanceof(object, class)
    local meta = getmetatable(object)
    if class == meta then
        return true
    elseif meta.super ~= nil then
        while meta.super ~= nil do
            meta = meta.super
            if class == meta then
                return true
            end
        end
    end
    return false
end

__shark_rt_null_object = { }
__shark_rt_list_object = { }

function sizeof(self)
    if getmetatable(self) == __shark_rt_list_object then
        return #rawget(self, "data")
    else
        return #self
    end
end

__shark_rt_list_object.__index = function (self, index)
    local value = rawget(self, "data")[index + 1]
    if value == nil then
        error("list index " .. index .. " out of range(" .. sizeof(self) .. ")")
    elseif value == __shark_rt_null_object then
        return nil
    else
        return value
    end
end

__shark_rt_list_object.__newindex = function (self, index, value)
    if value == nil then value = __shark_rt_null_object end
    rawget(self, "data")[index + 1] = value
end

function __shark_rt_list(table, length)
    for i = 1, length do
        if table[i] == nil then
            table[i] = __shark_rt_null_object
        end
    end
    table = { data = table }
    setmetatable(table, __shark_rt_list_object)
    return table
end

function __shark_rt_append(table, value)
    if value == nil then value = __shark_rt_null_object end
    local data = rawget(table, "data")
    data[#data + 1] = value
end

function __shark_rt_insert(table, index, value)
    local current = sizeof(table)
    while current > index do
        table[current] = table[current - 1]
    end
    table[index] = value
end

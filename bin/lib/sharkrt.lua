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

__shark_object = nil

__shark_object_get_field = function (self, name)
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

function __shark_create_class(name, super)
    class = { }
    class.name = name
    class.super = super
    class.__index = __shark_object_get_field
    return class
end

function __shark_object_new(class, ...)
    local object = { }
    setmetatable(object, class)
    class[class.constructor](object, ...)
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

function __shark_array(size)
    local table = { }
    table[size] = table
    return table
end

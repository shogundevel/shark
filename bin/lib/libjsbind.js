/******************************************************************************
*** Copyright *****************************************************************
** 
** Copyright 2022 Daniel Alvarez <shogundevel@gmail.com>
** 
** Permission is hereby granted, free of charge, to any person
** obtaining a copy of this software and associated documentation files
** (the "Software"), to deal in the Software without restriction,
** including without limitation the rights to use, copy, modify, merge,
** publish, distribute, sublicense, and/or sell copies of the Software,
** and to permit persons to whom the Software is furnished to do so,
** subject to the following conditions:
** 
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
** 
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
** 
******************************************************************************/

function __js_bind_0x1(callee, args) {
    return function () {
        return callee(args[0]);
    }
}

function __js_bind_0x2(callee, args) {
    return function () {
        return callee(args[0], args[1]);
    }
}

function __js_bind_0x3(callee, args) {
    return function () {
        return callee(args[0], args[1], args[2]);
    }
}

function __js_bind_0x4(callee, args) {
    return function () {
        return callee(args[0], args[1], args[2], args[3]);
    }
}

function __js_bind_1x1(callee, args) {
    return function (x) {
        return callee(args[0], x);
    }
}

function __js_bind_1x2(callee, args) {
    return function (x) {
        return callee(args[0], args[1], x);
    }
}

function __js_bind_1x3(callee, args) {
    return function (x) {
        return callee(args[0], args[1], args[2], x);
    }
}

function __js_bind_1x4(callee, args) {
    return function (x) {
        return callee(args[0], args[1], args[2], args[3], x);
    }
}

function __js_bind_2x1(callee, args) {
    return function (x, y) {
        return callee(args[0], x, y);
    }
}

function __js_bind_2x2(callee, args) {
    return function (x, y) {
        return callee(args[0], args[1], x, y);
    }
}

function __js_bind_2x3(callee, args) {
    return function (x, y) {
        return callee(args[0], args[1], args[2], x, y);
    }
}

function __js_bind_2x4(callee, args) {
    return function (x, y) {
        return callee(args[0], args[1], args[2], args[3], x, y);
    }
}

function __js_bind_3x1(callee, args) {
    return function (x, y, z) {
        return callee(args[0], x, y, z);
    }
}

function __js_bind_3x2(callee, args) {
    return function (x, y, z) {
        return callee(args[0], args[1], x, y, z);
    }
}

function __js_bind_3x3(callee, args) {
    return function (x, y, z) {
        return callee(args[0], args[1], args[2], x, y, z);
    }
}

function __js_bind_3x4(callee, args) {
    return function (x, y, z) {
        return callee(args[0], args[1], args[2], x, y, z);
    }
}

var __js_bind_table = [
    [null, __js_bind_0x1, __js_bind_0x2, __js_bind_0x3, __js_bind_0x4],
    [null, __js_bind_1x1, __js_bind_1x2, __js_bind_1x3, __js_bind_1x4],
    [null, __js_bind_2x1, __js_bind_2x2, __js_bind_2x3, __js_bind_2x4],
    [null, __js_bind_3x1, __js_bind_3x2, __js_bind_3x3, __js_bind_3x4]
];

function __js_bind(callee, args, count) {
    return __js_bind_table[count][args.length](callee, args);
}

__shark_rt_modules["javascript.bind"] = function () {
    var __global = { };
    __shark_rt_imports["javascript.bind"] = __global;
    __global.bind = __js_bind;
    return __global;
};

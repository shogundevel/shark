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

var __shark_rt_imports = { };
var __shark_rt_modules = { };

function __shark_rt_import(module)
{
    if (__shark_rt_imports[module] != undefined) {
        return __shark_rt_imports[module];
    } else if (__shark_rt_modules[module] != undefined) {
        return __shark_rt_modules[module]();
    } else {
        throw "can't resolve import '" + module + "'.";
    }
}

function error(message) {
    throw message;
}

function call_0(callee, args) {
    return callee();
}

function call_1(callee, args) {
    return callee(args[0]);
}

function call_2(callee, args) {
    return callee(args[0], args[1]);
}

function call_3(callee, args) {
    return callee(args[0], args[1], args[2]);
}

function call_4(callee, args) {
    return callee(args[0], args[1], args[2], args[3]);
}

function call_5(callee, args) {
    return callee(args[0], args[1], args[2], args[3], args[4]);
}

function call_6(callee, args) {
    return callee(args[0], args[1], args[2], args[3], args[4], args[5]);
}

function call_7(callee, args) {
    return callee(args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
}

var call_table = [ call_0, call_1, call_2, call_3, call_4, call_5, call_6, call_7 ];

function pcall(callee, args) {
    try {
        return call_table[args.length](callee, args);
    } catch (err) {
        return err;
    }
}

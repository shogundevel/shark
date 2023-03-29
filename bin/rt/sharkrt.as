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

import flash.display.BitmapData;

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

var current_object = this;
var current_child = null;

var objects = [ ];
var prev_objects = [ ];

var E_NONE = 0;
var E_PRESS = 1;
var E_MOVE = 2;
var E_RELEASE = 3;
var E_PRESS_UP = 4;
var E_PRESS_DOWN = 5;
var E_PRESS_LEFT = 6;
var E_PRESS_RIGHT = 7;
var E_PRESS_X = 8;
var E_PRESS_Y = 9;
var E_REL_UP = 10;
var E_REL_DOWN = 11;
var E_REL_LEFT = 12;
var E_REL_RIGHT = 13;
var E_REL_X = 14;
var E_REL_Y = 15;

var key_listener = new Object ();

key_listener.onKeyDown = function () {
    var key = Key.getCode();
    if (key == Key.UP) {
        current_child.event(E_PRESS_UP, 0, 0);
    } else if (key == Key.DOWN) {
        current_child.event(E_PRESS_DOWN, 0, 0);
    } else if (key == Key.LEFT) {
        current_child.event(E_PRESS_LEFT, 0, 0);
    } else if (key == Key.RIGHT) {
        current_child.event(E_PRESS_RIGHT, 0, 0);
    } else {
        var code = Key.getAscii();
        if (code == "z" || code == "Z") {
            current_child.event(E_PRESS_X, 0, 0);
        } else if (code == "x" || code == "x") {
            current_child.event(E_PRESS_Y, 0, 0);
        }
    }
};

key_listener.onKeyUp = function () {
    var key = Key.getCode();
    if (key == Key.UP) {
        current_child.event(E_REL_UP, 0, 0);
    } else if (key == Key.DOWN) {
        current_child.event(E_REL_DOWN, 0, 0);
    } else if (key == Key.LEFT) {
        current_child.event(E_REL_LEFT, 0, 0);
    } else if (key == Key.RIGHT) {
        current_child.event(E_REL_RIGHT, 0, 0);
    } else {
        var code = Key.getAscii();
        if (code == "z" || code == "Z") {
            current_child.event(E_REL_X, 0, 0);
        } else if (code == "x" || code == "X") {
            current_child.event(E_REL_Y, 0, 0);
        }
    }
};

Key.addListener(key_listener);

var mouse_listener = new Object ();
var pressed = false;

mouse_listener.onMouseDown = function () {
    pressed = true;
    current_child.event(E_PRESS, _xmouse, _ymouse);
}

mouse_listener.onMouseUp = function () {
    pressed = false;
    current_child.event(E_RELEASE, _xmouse, _ymouse);
}

mouse_listener.onMouseMove = function () {
    if (pressed) current_child.event(E_MOVE, _xmouse, _ymouse);
}

Mouse.addListener(mouse_listener);

this.onEnterFrame = function () {
    for (var i = 0; i < objects.length; i++) {
        objects[i]._x = 0;
        objects[i]._y = 0;
        objects[i].clear();
    }
    objects.splice(0, objects.length);
    current_child.update();
    current_child.render();
};

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

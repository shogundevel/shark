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
__shark_rt_modules['system.error'] = function () {
    var __globals__ = __shark_rt_imports['system.error'] = { };
    __globals__.ERR_NONE = 0;
    __globals__.ERR_UNKNOWN = 1;
    __globals__.ERROR = __globals__.ERR_NONE;
    __globals__.get_err = function () {
        return __globals__.ERROR;
        return null;
    };
    __globals__.has_err = function () {
        return __globals__.ERROR != __globals__.ERR_NONE;
        return null;
    };
    __globals__.set_err = function (value) {
        __globals__.ERROR = value
        return null;
    };
    __globals__.clear_err = function () {
        __globals__.ERROR = __globals__.ERR_NONE
        return null;
    };
    __globals__.error = error;
    __globals__.pcall = pcall;
    return __globals__;
};
__shark_rt_modules['system.exit'] = function () {
    var __globals__ = __shark_rt_imports['system.exit'] = { };
    __globals__.FAILURE = - 1;
    __globals__.SUCCESS = 0;
    __globals__.exit = function (code) {
        throw 'function exit is not implemented';
    };
    return __globals__;
};
__shark_rt_modules['system.string'] = function () {
    var __globals__ = __shark_rt_imports['system.string'] = { };
    __globals__.ERR_INVALID_LITERAL = 2;
    __globals__.CHAR_SIZE = 2;
    __globals__.String = String;
    __globals__.Number = Number;
    __globals__.itos = function (x) {
        return "" + x;
        return null;
    };
    __globals__.ftos = function (x) {
        return "" + x;
        return null;
    };
    __globals__.ctos = function (x) {
        return __globals__.String.fromCharCode(x);
        return null;
    };
    __globals__.stoi = function (x) {
        return __globals__.Number(x);
        return null;
    };
    __globals__.stof = function (x) {
        return __globals__.Number(x);
        return null;
    };
    __globals__.islower = function (x) {
        return x >= 'a' && x <= 'z';
        return null;
    };
    __globals__.isupper = function (x) {
        return x >= 'A' && x <= 'Z';
        return null;
    };
    __globals__.isalpha = function (x) {
        return __globals__.islower(x) || __globals__.isupper(x);
        return null;
    };
    __globals__.isdigit = function (x) {
        return x >= '0' && x <= '9';
        return null;
    };
    __globals__.isalnum = function (x) {
        return __globals__.isalpha(x) || __globals__.isdigit(x);
        return null;
    };
    __globals__.isident = function (x) {
        return __globals__.isalnum(x) || x == '_';
        return null;
    };
    __globals__.ishex = function (x) {
        return __globals__.isdigit(x) || (x >= 'a' && x <= 'f') || (x >= 'A' && x <= 'F');
        return null;
    };
    __globals__.isascii = function (x) {
        return x <= 127;
        return null;
    };
    __globals__.issurrogate = function (x) {
        return false;
        return null;
    };
    __globals__.tolower = function (x) {
        if (__globals__.isupper(x)) {
            return x + ('a' - 'A');
        } else {
            return x;
        }
        return null;
    };
    __globals__.toupper = function (x) {
        if (__globals__.islower(x)) {
            return x + ('A' - 'a');
        } else {
            return x;
        }
        return null;
    };
    __globals__.len = function (data) {
        return data.length;
        return null;
    };
    __globals__.index = function (data, index) {
        return data.charCodeAt(index);
        return null;
    };
    __globals__.slice = function (data, start, end) {
        return data.substring(start, end);
        return null;
    };
    __globals__.find = function (x, y) {
        return x.indexOf(y);
        return null;
    };
    __globals__.concat = function (x, y) {
        return x + y;
        return null;
    };
    __globals__.join = function (sep, list) {
        return list.join(sep);
        return null;
    };
    __globals__.split = function (data, sep) {
        return data.split(sep);
        return null;
    };
    __globals__.format = function (data, args) {
        var __local_0__ = null;
        __local_0__ = data.split("%");
        var __local_1__ = null;
        __local_1__ = "";
        var __local_2__ = null;
        var __local_3__ = null;
        __local_3__ = args.length;
        for (__local_2__ = 0; __local_2__ < __local_3__; __local_2__++) {
                __local_1__ += __local_0__[__local_2__]
                __local_1__ += args[__local_2__]
        }
        __local_1__ += __local_0__[__local_0__.length - 1]
        return null;
    };
    __globals__.strbuf = function () { };
    __globals__.strbuf.is_shark_class = true;
    __globals__.strbuf.super_class = Object;
    __globals__.strbuf.prototype = new Object();
    __globals__.strbuf.prototype.constructor = __globals__.strbuf;
    __globals__.strbuf.prototype.init = function () {
        this.buffer = ""
        return this;
    };
    __globals__.strbuf.prototype.put = function (value) {
        this.buffer += __globals__.String.fromCharCode(value)
        return null;
    };
    __globals__.strbuf.prototype.puts = function (data) {
        this.buffer += data
        return null;
    };
    __globals__.strbuf.prototype.printf = function (data, args) {
        this.buffer += __globals__.format(data, args)
        return null;
    };
    __globals__.strbuf.prototype.read_all = function () {
        var __local_0__ = null;
        __local_0__ = this.buffer;
        this.buffer = ""
        return __local_0__;
        return null;
    };
    return __globals__;
};
__shark_rt_modules['system.io'] = function () {
    var __globals__ = __shark_rt_imports['system.io'] = { };
    __globals__.string = __shark_rt_import('system.string')
    __globals__.console = console;
    __globals__.prompt = prompt;
    __globals__.puts = function (data) {
        __globals__.console.log(data)
        return null;
    };
    __globals__.printf = function (data, args) {
        __globals__.console.log(__globals__.string.format(data, args))
        return null;
    };
    __globals__.read_line = function () {
        return __globals__.prompt("");
        return null;
    };
    return __globals__;
};
__shark_rt_modules['system.math'] = function () {
    var __globals__ = __shark_rt_imports['system.math'] = { };
    __globals__.Math = Math;
    __globals__.pi = __globals__.Math.PI;
    __globals__.e = __globals__.Math.E;
    __globals__.abs = __globals__.Math.abs;
    __globals__.acos = __globals__.Math.acos;
    __globals__.asin = __globals__.Math.asin;
    __globals__.atan = __globals__.Math.atan;
    __globals__.atan2 = __globals__.Math.atan2;
    __globals__.cos = __globals__.Math.cos;
    __globals__.sin = __globals__.Math.sin;
    __globals__.tan = __globals__.Math.tan;
    __globals__.exp = __globals__.Math.exp;
    __globals__.log = __globals__.Math.log;
    __globals__.log10 = __globals__.Math.log10;
    __globals__.pow = __globals__.Math.pow;
    __globals__.sqrt = __globals__.Math.sqrt;
    __globals__.ceil = __globals__.Math.ceil;
    __globals__.floor = __globals__.Math.floor;
    __globals__.min = __globals__.Math.min;
    __globals__.max = __globals__.Math.max;
    __globals__.random = function (x) {
        return __globals__.Math.floor(x * __globals__.Math.random());
        return null;
    };
    return __globals__;
};
__shark_rt_modules['system.path'] = function () {
    var __globals__ = __shark_rt_imports['system.path'] = { };
    __globals__.get_base = function (path) {
        throw 'function get_base is not implemented';
    };
    __globals__.get_tail = function (path) {
        throw 'function get_tail is not implemented';
    };
    __globals__.get_ext = function (path) {
        throw 'function get_ext is not implemented';
    };
    __globals__.remove_ext = function (path) {
        throw 'function remove_ext is not implemented';
    };
    __globals__.join = function (base, tail) {
        throw 'function join is not implemented';
    };
    __globals__.listdir = function (path) {
        throw 'function listdir is not implemented';
    };
    __globals__.mkdir = function (path) {
        throw 'function mkdir is not implemented';
    };
    __globals__.rmdir = function (path) {
        throw 'function rmdir is not implemented';
    };
    __globals__.unlink = function (filename) {
        throw 'function unlink is not implemented';
    };
    return __globals__;
};
__shark_rt_modules['system.time'] = function () {
    var __globals__ = __shark_rt_imports['system.time'] = { };
    __globals__.clock = function () {
        throw 'function clock is not implemented';
    };
    return __globals__;
};
__shark_rt_modules['system.util'] = function () {
    var __globals__ = __shark_rt_imports['system.util'] = { };
    __globals__.copy = function (object) {
        var __local_0__ = null;
        __local_0__ = [];
        var __local_1__ = null;
        var __local_2__ = null;
        var __local_3__ = null;
        __local_2__ = object;
        for (__local_1__ = 0; __local_1__ < __local_2__.length; __local_1__++) {
            __local_3__ = __local_2__[__local_1__];
                __local_0__.push(__local_3__)
        }
        return __local_0__;
        return null;
    };
    __globals__.slice = function (list, start, end) {
        var __local_0__ = null;
        __local_0__ = [];
        var __local_1__ = null;
        var __local_2__ = null;
        __local_2__ = end;
        for (__local_1__ = start; __local_1__ < __local_2__; __local_1__++) {
                __local_0__.push(list[__local_1__])
        }
        return __local_0__;
        return null;
    };
    __globals__.pop = function (list) {
        return list.pop();
        return null;
    };
    __globals__.popindex = function (list, index) {
        var __local_0__ = null;
        __local_0__ = list[index];
        list.splice(index, 1)
        return __local_0__;
        return null;
    };
    __globals__.find = function (list, value) {
        var __local_0__ = null;
        var __local_1__ = null;
        __local_1__ = list.length;
        for (__local_0__ = 0; __local_0__ < __local_1__; __local_0__++) {
                if (list[__local_0__] == value) {
                    return __local_0__;
                }
        }
        return - 1;
        return null;
    };
    __globals__.concat = function (list, other) {
        return list.concat(other);
        return null;
    };
    __globals__.extend = function (list, other) {
        var __local_0__ = null;
        var __local_1__ = null;
        var __local_2__ = null;
        __local_1__ = other;
        for (__local_0__ = 0; __local_0__ < __local_1__.length; __local_0__++) {
            __local_2__ = __local_1__[__local_0__];
                list.push(__local_2__)
        }
        return null;
    };
    __globals__.remove = function (table, index) {
        throw 'function remove is not implemented';
    };
    __globals__.update = function (table, other) {
        throw 'function update is not implemented';
    };
    return __globals__;
};
__shark_rt_modules['system'] = function () {
    var __globals__ = __shark_rt_imports['system'] = { };
    __globals__.error = __shark_rt_import('system.error')
    __globals__.exit = __shark_rt_import('system.exit')
    __globals__.io = __shark_rt_import('system.io')
    __globals__.math = __shark_rt_import('system.math')
    __globals__.path = __shark_rt_import('system.path')
    __globals__.string = __shark_rt_import('system.string')
    __globals__.time = __shark_rt_import('system.time')
    __globals__.util = __shark_rt_import('system.util')
    return __globals__;
};
__shark_rt_modules['shark.texture'] = function () {
    var __globals__ = __shark_rt_imports['shark.texture'] = { };
    __globals__.texture = function () { };
    __globals__.texture.is_shark_class = true;
    __globals__.texture.super_class = Object;
    __globals__.texture.prototype = new Object();
    __globals__.texture.prototype.constructor = __globals__.texture;
    __globals__.texture.prototype.init = function (image_data) {
        this.image_data = image_data
        return this;
    };
    __globals__.texture.prototype.get_size_x = function () {
        return this.image_data.width;
        return null;
    };
    __globals__.texture.prototype.get_size_y = function () {
        return this.image_data.height;
        return null;
    };
    return __globals__;
};
__shark_rt_modules['shark.text'] = function () {
    var __globals__ = __shark_rt_imports['shark.text'] = { };
    __globals__.font = function () { };
    __globals__.font.is_shark_class = true;
    __globals__.font.super_class = Object;
    __globals__.font.prototype = new Object();
    __globals__.font.prototype.constructor = __globals__.font;
    __globals__.font.prototype.init = function (size, color) {
        this.size = size
        this.color = color
        return this;
    };
    __globals__.font.prototype.get_height = function () {
        return this.size;
        return null;
    };
    __globals__.font.prototype.get_width = function (data) {
        return 0;
        return null;
    };
    return __globals__;
};
__shark_rt_modules['shark.asset'] = function () {
    var __globals__ = __shark_rt_imports['shark.asset'] = { };
    __globals__.__local_0__ = __shark_rt_import('shark.texture')
    __globals__.texture = __globals__.__local_0__.texture
    __globals__.__local_1__ = __shark_rt_import('shark.text')
    __globals__.font = __globals__.__local_1__.font
    __globals__.ERR_ASSET_NOT_FOUND = 1000;
    __globals__.__pending_images = 0;
    __globals__.Image = Image;
    __globals__.on_texture_load = function () {
        __globals__.__pending_images -= 1
        return null;
    };
    __globals__.load_texture = function (name) {
        __globals__.__pending_images += 1
        var __local_0__ = null;
        __local_0__ = (__globals__.Image.is_shark_class ? new __globals__.Image().init(): new __globals__.Image());
        __local_0__.src = "asset/" + name
        __local_0__.addEventListener("load", __globals__.on_texture_load, false)
        return (__globals__.texture.is_shark_class ? new __globals__.texture().init(__local_0__): new __globals__.texture(__local_0__));
        return null;
    };
    __globals__.load_font = function (name, size, color) {
        return (__globals__.font.is_shark_class ? new __globals__.font().init(size, color): new __globals__.font(size, color));
        return null;
    };
    return __globals__;
};
__shark_rt_modules['shark.event'] = function () {
    var __globals__ = __shark_rt_imports['shark.event'] = { };
    __globals__.E_NONE = 0;
    __globals__.E_PRESS = 1;
    __globals__.E_MOVE = 2;
    __globals__.E_RELEASE = 3;
    __globals__.E_PRESS_UP = 4;
    __globals__.E_PRESS_DOWN = 5;
    __globals__.E_PRESS_LEFT = 6;
    __globals__.E_PRESS_RIGHT = 7;
    __globals__.E_PRESS_X = 8;
    __globals__.E_PRESS_Y = 9;
    __globals__.E_REL_UP = 10;
    __globals__.E_REL_DOWN = 11;
    __globals__.E_REL_LEFT = 12;
    __globals__.E_REL_RIGHT = 13;
    __globals__.E_REL_X = 14;
    __globals__.E_REL_Y = 15;
    return __globals__;
};
__shark_rt_modules['shark.activity'] = function () {
    var __globals__ = __shark_rt_imports['shark.activity'] = { };
    __globals__.asset = __shark_rt_import('shark.asset')
    __globals__.event = __shark_rt_import('shark.event')
    __globals__.main = null;
    __globals__.document = document;
    __globals__.Math = Math;
    __globals__.pending_draw_op = [];
    __globals__.pressed = false;
    __globals__.on_mouse_down = function (ev) {
        __globals__.pressed = true
        if (ev.clientX) {
            __globals__.main.event(__globals__.event.E_PRESS, (ev.clientX - __globals__.context_border_x) / __globals__.context_size_x, (ev.clientY - __globals__.context_border_y) / __globals__.context_size_y)
        } else {
            __globals__.main.event(__globals__.event.E_PRESS, (ev.screenX - __globals__.context_border_x) / __globals__.context_size_x, (ev.screenY - __globals__.context_border_y) / __globals__.context_size_y)
        }
        return null;
    };
    __globals__.on_mouse_up = function (ev) {
        __globals__.pressed = false
        if (ev.clientX) {
            __globals__.main.event(__globals__.event.E_RELEASE, (ev.clientX - __globals__.context_border_x) / __globals__.context_size_x, (ev.clientY - __globals__.context_border_y) / __globals__.context_size_y)
        } else {
            __globals__.main.event(__globals__.event.E_RELEASE, (ev.screenX - __globals__.context_border_x) / __globals__.context_size_x, (ev.screenY - __globals__.context_border_y) / __globals__.context_size_y)
        }
        return null;
    };
    __globals__.on_mouse_move = function (ev) {
        if (__globals__.pressed) {
            if (ev.clientX) {
                __globals__.main.event(__globals__.event.E_MOVE, (ev.clientX - __globals__.context_border_x) / __globals__.context_size_x, (ev.clientY - __globals__.context_border_y) / __globals__.context_size_y)
            } else {
                __globals__.main.event(__globals__.event.E_MOVE, (ev.screenX - __globals__.context_border_x) / __globals__.context_size_x, (ev.screenY - __globals__.context_border_y) / __globals__.context_size_y)
            }
        }
        return null;
    };
    __globals__.on_touch_down = function (ev) {
        ev.preventDefault()
        ev = ev.changedTouches[ev.changedTouches.length - 1]
        __globals__.pressed = true
        if (ev.clientX) {
            __globals__.main.event(__globals__.event.E_PRESS, (ev.clientX - __globals__.context_border_x) / __globals__.context_size_x, (ev.clientY - __globals__.context_border_y) / __globals__.context_size_y)
        } else {
            __globals__.main.event(__globals__.event.E_PRESS, (ev.screenX - __globals__.context_border_x) / __globals__.context_size_x, (ev.screenY - __globals__.context_border_y) / __globals__.context_size_y)
        }
        return null;
    };
    __globals__.on_touch_up = function (ev) {
        ev.preventDefault()
        ev = ev.changedTouches[ev.changedTouches.length - 1]
        __globals__.pressed = false
        if (ev.clientX) {
            __globals__.main.event(__globals__.event.E_RELEASE, (ev.clientX - __globals__.context_border_x) / __globals__.context_size_x, (ev.clientY - __globals__.context_border_y) / __globals__.context_size_y)
        } else {
            __globals__.main.event(__globals__.event.E_RELEASE, (ev.screenX - __globals__.context_border_x) / __globals__.context_size_x, (ev.screenY - __globals__.context_border_y) / __globals__.context_size_y)
        }
        return null;
    };
    __globals__.on_touch_move = function (ev) {
        ev.preventDefault()
        ev = ev.changedTouches[ev.changedTouches.length - 1]
        if (__globals__.pressed) {
            if (ev.clientX) {
                __globals__.main.event(__globals__.event.E_MOVE, (ev.clientX - __globals__.context_border_x) / __globals__.context_size_x, (ev.clientY - __globals__.context_border_y) / __globals__.context_size_y)
            } else {
                __globals__.main.event(__globals__.event.E_MOVE, (ev.screenX - __globals__.context_border_x) / __globals__.context_size_x, (ev.screenY - __globals__.context_border_y) / __globals__.context_size_y)
            }
        }
        return null;
    };
    __globals__.console = console;
    __globals__.on_key_down = function (ev) {
        if (ev.code == "ArrowUp") {
            __globals__.main.event(__globals__.event.E_PRESS_UP, 0, 0)
        } else if (ev.code == "ArrowDown") {
            __globals__.main.event(__globals__.event.E_PRESS_DOWN, 0, 0)
        } else if (ev.code == "ArrowLeft") {
            __globals__.main.event(__globals__.event.E_PRESS_LEFT, 0, 0)
        } else if (ev.code == "ArrowRight") {
            __globals__.main.event(__globals__.event.E_PRESS_RIGHT, 0, 0)
        } else if (ev.code == "KeyZ") {
            __globals__.main.event(__globals__.event.E_PRESS_X, 0, 0)
        } else if (ev.code == "KeyX") {
            __globals__.main.event(__globals__.event.E_PRESS_Y, 0, 0)
        } else if (ev.keyCode == 38) {
            __globals__.main.event(__globals__.event.E_PRESS_UP, 0, 0)
        } else if (ev.keyCode == 40) {
            __globals__.main.event(__globals__.event.E_PRESS_DOWN, 0, 0)
        } else if (ev.keyCode == 37) {
            __globals__.main.event(__globals__.event.E_PRESS_LEFT, 0, 0)
        } else if (ev.keyCode == 39) {
            __globals__.main.event(__globals__.event.E_PRESS_RIGHT, 0, 0)
        } else if (ev.keyCode == 90) {
            __globals__.main.event(__globals__.event.E_PRESS_X, 0, 0)
        } else if (ev.keyCode == 88) {
            __globals__.main.event(__globals__.event.E_PRESS_Y, 0, 0)
        } else {
            return null;
        }
        ev.preventDefault()
        return null;
    };
    __globals__.on_key_up = function (ev) {
        if (ev.code == "ArrowUp") {
            __globals__.main.event(__globals__.event.E_REL_UP, 0, 0)
        } else if (ev.code == "ArrowDown") {
            __globals__.main.event(__globals__.event.E_REL_DOWN, 0, 0)
        } else if (ev.code == "ArrowLeft") {
            __globals__.main.event(__globals__.event.E_REL_LEFT, 0, 0)
        } else if (ev.code == "ArrowRight") {
            __globals__.main.event(__globals__.event.E_REL_RIGHT, 0, 0)
        } else if (ev.code == "KeyZ") {
            __globals__.main.event(__globals__.event.E_REL_X, 0, 0)
        } else if (ev.code == "KeyX") {
            __globals__.main.event(__globals__.event.E_REL_Y, 0, 0)
        } else if (ev.keyCode == 38) {
            __globals__.main.event(__globals__.event.E_REL_UP, 0, 0)
        } else if (ev.keyCode == 40) {
            __globals__.main.event(__globals__.event.E_REL_DOWN, 0, 0)
        } else if (ev.keyCode == 37) {
            __globals__.main.event(__globals__.event.E_REL_LEFT, 0, 0)
        } else if (ev.keyCode == 39) {
            __globals__.main.event(__globals__.event.E_REL_RIGHT, 0, 0)
        } else if (ev.keyCode == 90) {
            __globals__.main.event(__globals__.event.E_REL_X, 0, 0)
        } else if (ev.keyCode == 88) {
            __globals__.main.event(__globals__.event.E_REL_Y, 0, 0)
        } else {
            return null;
        }
        ev.preventDefault()
        return null;
    };
    __globals__.window = window;
    __globals__.element = null;
    __globals__.context = null;
    __globals__.context_size_x = 1;
    __globals__.context_size_y = 1;
    __globals__.context_size_z = 1;
    __globals__.context_border_x = 0;
    __globals__.context_border_y = 0;
    __globals__.set_element_size = function () {
        __globals__.context.scale(1 / __globals__.context_size_z, 1 / __globals__.context_size_z)
        __globals__.context.translate(- __globals__.context_border_x, - __globals__.context_border_y)
        __globals__.element.width = __globals__.window.innerWidth
        __globals__.element.height = __globals__.window.innerHeight
        __globals__.context_size_x = __globals__.element.width / 320
        __globals__.context_size_y = __globals__.element.height / 192
        if (__globals__.context_size_x > __globals__.context_size_y) {
            __globals__.context_size_z = __globals__.context_size_y
            __globals__.context_border_x = (__globals__.element.width - 320 * __globals__.context_size_y) / 2
            __globals__.context_border_y = 0
        } else if (__globals__.context_size_y > __globals__.context_size_x) {
            __globals__.context_size_z = __globals__.context_size_x
            __globals__.context_border_x = 0
            __globals__.context_border_y = (__globals__.element.height - 192 * __globals__.context_size_x) / 2
        } else {
            __globals__.context_size_z = __globals__.context_size_x
            __globals__.context_border_x = 0
            __globals__.context_border_y = 0
        }
        __globals__.context.fillStyle = "black"
        __globals__.context.fillRect(0, 0, __globals__.element.width, __globals__.element.height)
        __globals__.context.translate(__globals__.context_border_x, __globals__.context_border_y)
        __globals__.context.scale(__globals__.context_size_z, __globals__.context_size_z)
        __globals__.context.textBaseline = "top"
        return null;
    };
    __globals__.activity = function () { };
    __globals__.activity.is_shark_class = true;
    __globals__.activity.super_class = Object;
    __globals__.activity.prototype = new Object();
    __globals__.activity.prototype.constructor = __globals__.activity;
    __globals__.activity.prototype.init = function () {
        __globals__.element = __globals__.document.getElementById("screen")
        __globals__.context = __globals__.element.getContext("2d")
        __globals__.set_element_size()
        this.context = __globals__.context
        this.context.textColor = "#000000FF"
        this.context.textBaseline = "top"
        __globals__.window.addEventListener("resize", __globals__.set_element_size, false)
        __globals__.window.addEventListener("mousedown", __globals__.on_mouse_down, false)
        __globals__.window.addEventListener("mouseup", __globals__.on_mouse_up, false)
        __globals__.window.addEventListener("mousemove", __globals__.on_mouse_move, false)
        __globals__.window.addEventListener("touchstart", __globals__.on_touch_down, false)
        __globals__.window.addEventListener("touchend", __globals__.on_touch_up, false)
        __globals__.window.addEventListener("touchmove", __globals__.on_touch_move, false)
        __globals__.window.addEventListener("keyup", __globals__.on_key_up, false)
        __globals__.window.addEventListener("keydown", __globals__.on_key_down, false)
        return this;
    };
    __globals__.activity.prototype.draw = function (text, x, y) {
        if (__globals__.asset.__pending_images == 0) {
            this.__do_draw(text, x, y)
        } else {
            __globals__.pending_draw_op.push([text, x, y])
        }
        return null;
    };
    __globals__.activity.prototype.draw_text = function (text, font, x, y) {
        if (__globals__.asset.__pending_images == 0) {
            this.__do_draw_text(text, font, x, y)
        } else {
            __globals__.pending_draw_op.push([text, font, x, y])
        }
        return null;
    };
    __globals__.activity.prototype.__do_draw = function (text, x, y) {
        this.context.drawImage(text.image_data, x, y)
        return null;
    };
    __globals__.activity.prototype.__do_draw_text = function (text, font, x, y) {
        this.context.font = font.size + "px verdana, sans-serif"
        this.context.fillText(text, x, y)
        return null;
    };
    __globals__.activity.prototype.__render = function () {
        if (__globals__.asset.__pending_images != 0) {
            __globals__.pending_draw_op = []
        } else {
            var __local_0__ = null;
            var __local_1__ = null;
            var __local_2__ = null;
            __local_1__ = __globals__.pending_draw_op;
            for (__local_0__ = 0; __local_0__ < __local_1__.length; __local_0__++) {
                __local_2__ = __local_1__[__local_0__];
                    if (__local_2__.length == 3) {
                        this.__do_draw(__local_2__[0], __local_2__[1], __local_2__[2])
                    } else {
                        this.__do_draw_text(__local_2__[0], __local_2__[1], __local_2__[2], __local_2__[3])
                    }
            }
        }
        return null;
    };
    __globals__.activity.prototype.launch = function () {
        return null;
    };
    __globals__.activity.prototype.event = function (type, x, y) {
        return null;
    };
    __globals__.activity.prototype.update = function () {
        return null;
    };
    __globals__.activity.prototype.render = function () {
        return null;
    };
    return __globals__;
};
__shark_rt_modules['shark.persistent'] = function () {
    var __globals__ = __shark_rt_imports['shark.persistent'] = { };
    __globals__.get_save_file = function (mode) {
        throw 'function get_save_file is not implemented';
    };
    return __globals__;
};
__shark_rt_modules['shark'] = function () {
    var __globals__ = __shark_rt_imports['shark'] = { };
    __globals__.system = __shark_rt_import('system')
    __globals__.activity = __shark_rt_import('shark.activity')
    __globals__.asset = __shark_rt_import('shark.asset')
    __globals__.event = __shark_rt_import('shark.event')
    __globals__.persistent = __shark_rt_import('shark.persistent')
    __globals__.text = __shark_rt_import('shark.text')
    __globals__.texture = __shark_rt_import('shark.texture')
    __globals__.__local_0__ = __shark_rt_import('main')
    __globals__.main_activity = __globals__.__local_0__.main_activity
    __globals__.window = window;
    __globals__.requestAnimationFrame = requestAnimationFrame;
    __globals__.main = null;
    __globals__.last_frame_time = 0;
    __globals__.update = function (time) {
        var __local_0__ = null;
        __local_0__ = __globals__.requestAnimationFrame;
        if (time - __globals__.last_frame_time < 1000 / 24) {
            __local_0__(__globals__.update)
            return null;
        }
        __globals__.activity.main.update()
        __globals__.activity.main.render()
        __globals__.activity.main.__render()
        __globals__.last_frame_time = time
        __local_0__(__globals__.update)
        return null;
    };
    __globals__.on_load = function () {
        __globals__.activity.main = (__globals__.main_activity.is_shark_class ? new __globals__.main_activity().init(): new __globals__.main_activity())
        __globals__.activity.main.launch()
        var __local_0__ = null;
        __local_0__ = __globals__.requestAnimationFrame;
        __local_0__(__globals__.update)
        return null;
    };
    __globals__.window.addEventListener("load", __globals__.on_load, false)
    return __globals__;
};
__shark_rt_modules['main'] = function () {
    var __globals__ = __shark_rt_imports['main'] = { };
    __globals__.__local_0__ = __shark_rt_import('shark.activity')
    __globals__.activity = __globals__.__local_0__.activity
    __globals__.asset = __shark_rt_import('shark.asset')
    __globals__.event = __shark_rt_import('shark.event')
    __globals__.math = __shark_rt_import('system.math')
    __globals__.__local_1__ = __shark_rt_import('system.util')
    __globals__.copy = __globals__.__local_1__.copy
    __globals__.__local_2__ = __shark_rt_import('system.string')
    __globals__.itos = __globals__.__local_2__.itos
    __globals__.square = __globals__.asset.load_texture("square.png");
    __globals__.bg = __globals__.asset.load_texture("bg.png");
    __globals__.font = __globals__.asset.load_font("courier_new.ttf", 12, 0xFF);
    __globals__.piece_data = [[[[true, true, true, true]], [[true], [true], [true], [true]]], [[[true, true], [true, true]]], [[[true, true, false], [false, true, true]], [[false, true], [true, true], [true, false]]], [[[false, true, true], [true, true, false]], [[true, false], [true, true], [false, true]]], [[[true, true, true], [false, false, true]], [[true, true], [true, false], [true, false]], [[true, false, false], [true, true, true]], [[false, true], [false, true], [true, true]]], [[[false, false, true], [true, true, true]], [[true, true], [false, true], [false, true]], [[true, true, true], [true, false, false]], [[true, false], [true, false], [true, true]]], [[[true, true, true], [false, true, false]], [[true, false], [true, true], [true, false]], [[false, true, false], [true, true, true]], [[false, true], [true, true], [false, true]]]];
    __globals__.W = 10;
    __globals__.H = 12;
    __globals__.EVENT_MAX = 3;
    __globals__.main_activity = function () { };
    __globals__.main_activity.is_shark_class = true;
    __globals__.main_activity.super_class = __globals__.activity;
    __globals__.main_activity.prototype = new __globals__.activity();
    __globals__.main_activity.prototype.constructor = __globals__.main_activity;
    __globals__.main_activity.prototype.launch = function () {
        this.new_game()
        this.left_down = false
        this.right_down = false
        this.down_down = false
        this.event_time = 0
        return null;
    };
    __globals__.main_activity.prototype.new_game = function () {
        var __local_0__ = null;
        __local_0__ = [];
        var __local_1__ = null;
        var __local_2__ = null;
        __local_2__ = __globals__.H;
        for (__local_1__ = 0; __local_1__ < __local_2__; __local_1__++) {
                __local_0__.push(false)
        }
        this.board = []
        var __local_3__ = null;
        var __local_4__ = null;
        __local_4__ = __globals__.W;
        for (__local_3__ = 0; __local_3__ < __local_4__; __local_3__++) {
                this.board.push(__globals__.copy(__local_0__))
        }
        this.score = 0
        this.next_piece = __globals__.math.random(__globals__.piece_data.length)
        this.next_r = __globals__.math.random(__globals__.piece_data[this.next_piece].length)
        this.new_piece()
        this.speed = 24
        this.current_time = 0
        this.touch_w = false
        this.touch_x = 0
        this.touch_y = 0
        return null;
    };
    __globals__.main_activity.prototype.new_piece = function () {
        this.piece = __globals__.piece_data[this.next_piece]
        this.r = this.next_r
        this.next_piece = __globals__.math.random(__globals__.piece_data.length)
        this.next_r = __globals__.math.random(__globals__.piece_data[this.next_piece].length)
        this.x = __globals__.math.random(this.board.length - this.piece[this.r].length)
        this.y = 0
        if (this.collision()) {
            this.new_game()
        }
        return null;
    };
    __globals__.main_activity.prototype.event = function (type, x, y) {
        if (type == __globals__.event.E_PRESS_LEFT) {
            this.left_down = true
            this.event_time = __globals__.EVENT_MAX
        } else if (type == __globals__.event.E_PRESS_RIGHT) {
            this.right_down = true
            this.event_time = __globals__.EVENT_MAX
        } else if (type == __globals__.event.E_PRESS_DOWN) {
            this.down_down = true
        } else if (type == __globals__.event.E_REL_LEFT) {
            this.left_down = false
        } else if (type == __globals__.event.E_REL_RIGHT) {
            this.right_down = false
        } else if (type == __globals__.event.E_REL_DOWN) {
            this.down_down = false
        } else if (type == __globals__.event.E_PRESS_UP) {
            this.rotate()
        }
        if (type == __globals__.event.E_PRESS) {
            this.touch_w = true
            this.touch_x = x
            this.touch_y = y
        } else if (type == __globals__.event.E_RELEASE) {
            if (this.touch_x == x && this.touch_y == y) {
                this.rotate()
            }
            this.touch_w = false
        } else if (type == __globals__.event.E_MOVE && this.touch_w) {
            var __local_0__ = null;
            __local_0__ = x - this.touch_x;
            var __local_1__ = null;
            __local_1__ = y - this.touch_y;
            if (__local_0__ < - 16) {
                this.move(- 1, 0)
                this.touch_x -= 16
            } else if (__local_0__ > 16) {
                this.move(1, 0)
                this.touch_x += 16
            } else if (__local_1__ > 16) {
                this.move(0, 1)
                this.touch_y += 16
            }
        }
        return null;
    };
    __globals__.main_activity.prototype.update = function () {
        this.current_time += 1
        this.event_time += 1
        if (this.event_time >= __globals__.EVENT_MAX) {
            this.event_time -= __globals__.EVENT_MAX
            if (this.left_down) {
                this.move(- 1, 0)
            }
            if (this.right_down) {
                this.move(1, 0)
            }
        }
        if (this.down_down && ! this.left_down && ! this.right_down) {
            this.move(0, 1)
        }
        if (this.current_time >= this.speed) {
            this.current_time -= this.speed
            this.advance()
        }
        return null;
    };
    __globals__.main_activity.prototype.advance = function () {
        if (! this.move(0, 1)) {
            this.freeze()
            this.new_piece()
        }
        return null;
    };
    __globals__.main_activity.prototype.rotate = function () {
        var __local_0__ = null;
        __local_0__ = this.r;
        this.r = (this.r + 1) % this.piece.length
        if (this.collision()) {
            this.r = __local_0__
        }
        return null;
    };
    __globals__.main_activity.prototype.move = function (x, y) {
        this.x += x
        this.y += y
        var __local_0__ = null;
        __local_0__ = this.collision();
        if (__local_0__) {
            this.x -= x
            this.y -= y
            return false;
        } else {
            return true;
        }
        return null;
    };
    __globals__.main_activity.prototype.collision = function () {
        var __local_0__ = null;
        __local_0__ = this.piece[this.r];
        if (__globals__.W < this.x + __local_0__.length || this.x < 0) {
            return true;
        } else if (__globals__.H < this.y + __local_0__[0].length) {
            return true;
        }
        var __local_1__ = null;
        var __local_2__ = null;
        __local_2__ = __local_0__.length;
        for (__local_1__ = 0; __local_1__ < __local_2__; __local_1__++) {
                var __local_3__ = null;
                __local_3__ = __local_0__[__local_1__];
                var __local_4__ = null;
                var __local_5__ = null;
                __local_5__ = __local_3__.length;
                for (__local_4__ = 0; __local_4__ < __local_5__; __local_4__++) {
                        if (__local_3__[__local_4__] && this.board[this.x + __local_1__][this.y + __local_4__]) {
                            return true;
                        }
                }
        }
        return false;
        return null;
    };
    __globals__.main_activity.prototype.freeze = function () {
        var __local_0__ = null;
        __local_0__ = this.piece[this.r];
        var __local_1__ = null;
        var __local_2__ = null;
        __local_2__ = __local_0__.length;
        for (__local_1__ = 0; __local_1__ < __local_2__; __local_1__++) {
                var __local_3__ = null;
                __local_3__ = __local_0__[__local_1__];
                var __local_4__ = null;
                var __local_5__ = null;
                __local_5__ = __local_3__.length;
                for (__local_4__ = 0; __local_4__ < __local_5__; __local_4__++) {
                        if (__local_3__[__local_4__]) {
                            this.board[this.x + __local_1__][this.y + __local_4__] = true
                        }
                }
        }
        this.clear_lines()
        return null;
    };
    __globals__.main_activity.prototype.clear_lines = function () {
        var __local_0__ = null;
        var __local_1__ = null;
        __local_1__ = __globals__.H;
        for (__local_0__ = 0; __local_0__ < __local_1__; __local_0__++) {
                var __local_2__ = null;
                __local_2__ = true;
                var __local_3__ = null;
                var __local_4__ = null;
                __local_4__ = __globals__.W;
                for (__local_3__ = 0; __local_3__ < __local_4__; __local_3__++) {
                        if (! this.board[__local_3__][__local_0__]) {
                            __local_2__ = false
                            break;
                        }
                }
                if (__local_2__) {
                    this.score += 1
                    var __local_5__ = null;
                    var __local_6__ = null;
                    __local_6__ = __globals__.W;
                    for (__local_5__ = 0; __local_5__ < __local_6__; __local_5__++) {
                            var __local_7__ = null;
                            __local_7__ = __local_0__;
                            while (__local_7__ > 0) {
                                    this.board[__local_5__][__local_7__] = this.board[__local_5__][__local_7__ - 1]
                                    __local_7__ -= 1
                            }
                            this.board[__local_5__][0] = false
                    }
                    this.speed = __globals__.math.max(24 - this.score, 8)
                }
        }
        return null;
    };
    __globals__.main_activity.prototype.render = function () {
        this.draw(__globals__.bg, 0, 0)
        var __local_0__ = null;
        var __local_1__ = null;
        __local_1__ = this.board.length;
        for (__local_0__ = 0; __local_0__ < __local_1__; __local_0__++) {
                var __local_2__ = null;
                __local_2__ = this.board[__local_0__];
                var __local_3__ = null;
                var __local_4__ = null;
                __local_4__ = __local_2__.length;
                for (__local_3__ = 0; __local_3__ < __local_4__; __local_3__++) {
                        if (__local_2__[__local_3__]) {
                            this.draw(__globals__.square, 80 + __local_0__ * 16, 0 + __local_3__ * 16)
                        }
                }
        }
        var __local_5__ = null;
        __local_5__ = this.piece[this.r];
        var __local_6__ = null;
        var __local_7__ = null;
        __local_7__ = __local_5__.length;
        for (__local_6__ = 0; __local_6__ < __local_7__; __local_6__++) {
                var __local_8__ = null;
                __local_8__ = __local_5__[__local_6__];
                var __local_9__ = null;
                var __local_10__ = null;
                __local_10__ = __local_8__.length;
                for (__local_9__ = 0; __local_9__ < __local_10__; __local_9__++) {
                        if (__local_8__[__local_9__]) {
                            this.draw(__globals__.square, 80 + (this.x + __local_6__) * 16, 0 + (this.y + __local_9__) * 16)
                        }
                }
        }
        var __local_11__ = null;
        __local_11__ = __globals__.piece_data[this.next_piece][this.next_r];
        var __local_12__ = null;
        var __local_13__ = null;
        __local_13__ = __local_11__.length;
        for (__local_12__ = 0; __local_12__ < __local_13__; __local_12__++) {
                var __local_14__ = null;
                __local_14__ = __local_11__[__local_12__];
                var __local_15__ = null;
                var __local_16__ = null;
                __local_16__ = __local_14__.length;
                for (__local_15__ = 0; __local_15__ < __local_16__; __local_15__++) {
                        if (__local_14__[__local_15__]) {
                            this.draw(__globals__.square, 248 + __local_12__ * 16, 8 + __local_15__ * 16)
                        }
                }
        }
        this.draw_text(__globals__.itos(this.score), __globals__.font, 260, 90)
        return null;
    };
    return __globals__;
};
var main = __shark_rt_import('shark');

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
import flash.geom.Rectangle;
import flash.events.MouseEvent;
import flash.events.KeyboardEvent;
import flash.ui.Keyboard;
import flash.text.TextField;
import flash.text.TextFormat;

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

function __shark_load_bitmap(name, object)
{
    var loader = new Loader ();
    
    loader.contentLoaderInfo.addEventListener(Event.COMPLETE, function (ev) {
        object.data = Bitmap(LoaderInfo(ev.target).content).bitmapData;
        trace("Loaded bitmap: " + name);
        trace(object.data);
    });
    
    loader.load(new URLRequest ("./asset/" + name));
    
    return object;
}

var current_object = this;
var current_child = null;

var objects = [ ];
var prev_objects = [ ];

var text_label: TextField = new TextField ();
var text_data = new BitmapData (480, 24, true, 0);
var text_rect = new Rectangle (0, 0, 480, 24);

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

this.addEventListener(KeyboardEvent.KEY_DOWN, function (ev: KeyboardEvent) {
    var key = ev.keyCode;
    if (key == Keyboard.UP) {
        current_child.event(E_PRESS_UP, 0, 0);
    } else if (key == Keyboard.DOWN) {
        current_child.event(E_PRESS_DOWN, 0, 0);
    } else if (key == Keyboard.LEFT) {
        current_child.event(E_PRESS_LEFT, 0, 0);
    } else if (key == Keyboard.RIGHT) {
        current_child.event(E_PRESS_RIGHT, 0, 0);
    } else if (key == Keyboard.Z) {
		current_child.event(E_PRESS_X, 0, 0);
	} else if (key == Keyboard.X) {
		current_child.event(E_PRESS_Y, 0, 0);
	}
});

this.addEventListener(KeyboardEvent.KEY_UP, function (ev: KeyboardEvent) {
    var key = ev.keyCode;
    if (key == Keyboard.UP) {
        current_child.event(E_REL_UP, 0, 0);
    } else if (key == Keyboard.DOWN) {
        current_child.event(E_REL_DOWN, 0, 0);
    } else if (key == Keyboard.LEFT) {
        current_child.event(E_REL_LEFT, 0, 0);
    } else if (key == Keyboard.RIGHT) {
        current_child.event(E_REL_RIGHT, 0, 0);
    } else if (key == Keyboard.Z) {
		current_child.event(E_REL_X, 0, 0);
	} else if (key == Keyboard.X) {
		current_child.event(E_REL_Y, 0, 0);
	}
});

var pressed = false;

this.addEventListener(MouseEvent.MOUSE_DOWN, function (ev) {
    pressed = true;
	current_child.event(E_PRESS, ev.stageX, ev.stageY);
});

this.addEventListener(MouseEvent.MOUSE_UP, function (ev) {
    pressed = false;
    current_child.event(E_RELEASE, ev.stageX, ev.stageY);
});

this.addEventListener(MouseEvent.MOUSE_MOVE, function (ev) {
    if (pressed)
		current_child.event(E_MOVE, ev.stageX, ev.stageY);
});

this.addEventListener(Event.ENTER_FRAME, function () {
    for (var i = 0; i < objects.length; i++) {
        objects[i].x = 0;
        objects[i].y = 0;
        objects[i].graphics.clear();
    }
    objects.splice(0, objects.length);
    current_child.update();
    current_child.render();
});

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

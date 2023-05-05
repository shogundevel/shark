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

var objects = [ ];
var prev_objects = [ ];

var text_label = this.createTextField("text_label", this.getNextHighestDepth(), 0, 0, 320, 24);
var text_data = new BitmapData (320, 24, true);
var text_rect = new Rectangle (0, 0, 320, 24);

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
        event(E_PRESS_UP, 0, 0);
    } else if (key == Key.DOWN) {
        event(E_PRESS_DOWN, 0, 0);
    } else if (key == Key.LEFT) {
        event(E_PRESS_LEFT, 0, 0);
    } else if (key == Key.RIGHT) {
        event(E_PRESS_RIGHT, 0, 0);
    } else {
        var code = Key.getAscii();
        if (code == "z" || code == "Z") {
            event(E_PRESS_X, 0, 0);
        } else if (code == "x" || code == "x") {
            event(E_PRESS_Y, 0, 0);
        }
    }
};

key_listener.onKeyUp = function () {
    var key = Key.getCode();
    if (key == Key.UP) {
        event(E_REL_UP, 0, 0);
    } else if (key == Key.DOWN) {
        event(E_REL_DOWN, 0, 0);
    } else if (key == Key.LEFT) {
        event(E_REL_LEFT, 0, 0);
    } else if (key == Key.RIGHT) {
        event(E_REL_RIGHT, 0, 0);
    } else {
        var code = Key.getAscii();
        if (code == "z" || code == "Z") {
            event(E_REL_X, 0, 0);
        } else if (code == "x" || code == "X") {
            event(E_REL_Y, 0, 0);
        }
    }
};

Key.addListener(key_listener);

var mouse_listener = new Object ();
var pressed = false;

mouse_listener.onMouseDown = function () {
    pressed = true;
    event(E_PRESS, _xmouse, _ymouse);
}

mouse_listener.onMouseUp = function () {
    pressed = false;
    event(E_RELEASE, _xmouse, _ymouse);
}

mouse_listener.onMouseMove = function () {
    if (pressed) event(E_MOVE, _xmouse, _ymouse);
}

Mouse.addListener(mouse_listener);

var __init = true;

this.onEnterFrame = function ()
{
    if (__init)
    {
        launch();
        __init = false;
    }
    
    for (var i = 0; i < objects.length; i++)
    {
        objects[i]._x = 0;
        objects[i]._y = 0;
        objects[i].clear();
    }
    
    for (var i = 0; i < text_labels.length; i++)
    {
        text_labels[i]._x = 480;
        text_labels[i]._y = 480;
        text_labels[i].text = "";
    }
    
    objects.splice(0, objects.length);
    text_labels.splice(0, text_labels.length);
    
    update();
    
    draw(null, 0, 0);
    draw(null, 0, 0);
    
    draw_text("", null, 0, 0);
    draw_text("", null, 0, 0);
    
    render();
};

function texture(image_data) {
    this.image_data = image_data;
}

texture.prototype.get_size_x = function () {
    return this.image_data.width;
}

texture.prototype.get_size_y = function () {
    return this.image_data.height;
}

function font(size, color)
{
    this.size = size;
    this.color = color;
}

font.prototype.get_height = function () {
    return this.size;
}

function get_save_file(mode) {
    return null;
}

var ERR_ASSET_NOT_FOUND = 1000;

function load_texture(name) {
    return new texture (BitmapData.loadBitmap(name.split(".")[0]));
}

function load_font(name, size, color) {
    return new font (size, color);
}

function draw(text, x, y)
{
    var object_id = objects.length;
    var object = null;
    if (object_id >= prev_objects.length) {
        object = this.createEmptyMovieClip("mc_" + object_id, this.getNextHighestDepth());
        prev_objects.push(object);
    } else {
        object = prev_objects[object_id];
    }
    objects.push(object);
    object.beginBitmapFill(text.image_data);
    object.lineTo(text.get_size_x(), 0);
    object.lineTo(text.get_size_x(), text.get_size_y());
    object.lineTo(0, text.get_size_y());
    object.lineTo(0, 0);
    object.endFill();
    object._x = x;
    object._y = y;
}

function draw_ex(text, x, y, xo, yo, r, xs, yx) {
    throw "invalid_draw_operation";
}

function draw_text(text, font, x, y)
{
    var object_id = objects.length;
    var object = null;
    if (object_id >= prev_objects.length) {
        object = this.createEmptyMovieClip("mc_" + object_id, this.getNextHighestDepth());
        prev_objects.push(object);
    } else {
        object = prev_objects[object_id];
    }
    objects.push(object);
    text_label.text = text;
    text_data.fillRect(text_rect, 0x00000000);
    text_data.draw(text_label);
    object.beginBitmapFill(text_data);
    object.lineTo(320, 0);
    object.lineTo(320, 24);
    object.lineTo(0, 24);
    object.lineTo(0, 0);
    object.endFill();
    object._x = x;
    object._y = y;
}

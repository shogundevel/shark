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

var ERR_ASSET_NOT_FOUND = 1000;
var __pending_images = 0;

function on_texture_load() {
    __pending_images -= 1;
}

function load_texture(name)
{
    __pending_images += 1;
    var image_data = new Image ();
    image_data.src = "asset/" + name;
    image_data.addEventListener("load", on_texture_load, false);
    return new texture (image_data);
}

function load_font(name, size, color) {
    return new font (size, color);
}

var main = null;
var pending_draw_op = [ ];
var pressed = false;

function on_mouse_down(ev)
{
    pressed = true;
    event(E_PRESS, ev.screenX / 3, ev.screenY / 3);
}

function on_mouse_up(ev)
{
    pressed = false;
    event(E_RELEASE, ev.screenX / 3, ev.screenY / 3);
}

function on_mouse_move(ev)
{
    if (pressed)
        event(E_MOVE, ev.screenX / 3, ev.screenY / 3);
}

function on_key_down(ev)
{
    // ev.code
    if (ev.code == "ArrowUp")
        event(E_PRESS_UP, 0, 0);
    else if (ev.code == "ArrowDown")
        event(E_PRESS_DOWN, 0, 0);
    else if (ev.code == "ArrowLeft")
        event(E_PRESS_LEFT, 0, 0);
    else if (ev.code == "ArrowRight")
        event(E_PRESS_RIGHT, 0, 0);
    else if (ev.code == "KeyZ")
        event(E_PRESS_X, 0, 0);
    else if (ev.code == "KeyX")
        event(E_PRESS_Y, 0, 0);
    // ev.keyCode
    else if (ev.keyCode == 38)
        event(E_PRESS_UP, 0, 0);
    else if (ev.keyCode == 40)
        event(E_PRESS_DOWN, 0, 0);
    else if (ev.keyCode == 37)
        event(E_PRESS_LEFT, 0, 0);
    else if (ev.keyCode == 39)
        event(E_PRESS_RIGHT, 0, 0);
    else if (ev.keyCode == 90)
        event(E_PRESS_X, 0, 0);
    else if (ev.keyCode == 88)
        event(E_PRESS_Y, 0, 0);
    else
        return;
    ev.preventDefault();
}

function on_key_up(ev)
{
    // ev.code
    if (ev.code == "ArrowUp")
        event(E_REL_UP, 0, 0)
    else if (ev.code == "ArrowDown")
        event(E_REL_DOWN, 0, 0)
    else if (ev.code == "ArrowLeft")
        event(E_REL_LEFT, 0, 0)
    else if (ev.code == "ArrowRight")
        event(E_REL_RIGHT, 0, 0)
    else if (ev.code == "KeyZ")
        event(E_REL_X, 0, 0)
    else if (ev.code == "KeyX")
        event(E_REL_Y, 0, 0)
    // ev.keyCode
    else if (ev.keyCode == 38)
        event(E_REL_UP, 0, 0);
    else if (ev.keyCode == 40)
        event(E_REL_DOWN, 0, 0);
    else if (ev.keyCode == 37)
        event(E_REL_LEFT, 0, 0);
    else if (ev.keyCode == 39)
        event(E_REL_RIGHT, 0, 0);
    else if (ev.keyCode == 90)
        event(E_REL_X, 0, 0);
    else if (ev.keyCode == 88)
        event(E_REL_Y, 0, 0);
}

var element = document.getElementById("screen");
var context = element.getContext("2d");

context.scale(3, 3);
context.textColor = "#000000FF";
context.textBaseline = "top";

window.addEventListener("mousedown", on_mouse_down, false);
window.addEventListener("mouseup", on_mouse_up, false);
window.addEventListener("mousemove", on_mouse_move, false);
window.addEventListener("keyup", on_key_up, false);
window.addEventListener("keydown", on_key_down, { "capture": true });

function draw(text, x, y)
{
    if (__pending_images == 0)
        __do_draw(text, x, y);
    else
        pending_draw_op.push([text, x, y]);
}

function draw_ex(text, x, y, origin_x, origin_y, rotation, scale_x, scale_y)
{
    if (__pending_images == 0)
        __do_draw_ex(text, x, y, origin_x, origin_y, rotation, scale_x, scale_y);
    else
        pending_draw_op.push([text, x, y, origin_x, origin_y, rotation, scale_x, scale_y])
}

function draw_text(text, font, x, y)
{
    if (__pending_images == 0)
        __do_draw_text(text, font, x, y)
    else
        pending_draw_op.push([text, font, x, y])
}

function __do_draw(text, x, y) {
    context.drawImage(text.image_data, x, y)
}

function __do_draw_ex(text, x, y, origin_x, origin_y, rotation, scale_x, scale_y)
{
    context.save()
    context.translate(x, y)
    context.translate(origin_x, origin_y)
    context.rotate(-rotation * Math.PI / 180)
    context.scale(scale_x, scale_y)
    context.drawImage(text.image_data, -text.image_data.width / 2, -text.image_data.height / 2)
    context.restore()
}

function __do_draw_text(text, font, x, y)
{
    context.font = font.size + "px verdana, sans-serif";
    context.fillText(text, x, y);
}

function __render()
{
    if (__pending_images == 0)
    {
        for (var i = 0; i < pending_draw_op.length; i++)
        {
            var draw_op = pending_draw_op[i];
            if (draw_op.length == 3)
                __do_draw(draw_op[0], draw_op[1], draw_op[2]);
            else if (draw_op.length == 4)
                __do_draw_text(draw_op[0], draw_op[1], draw_op[2], draw_op[3]);
            else
                __do_draw_ex(draw_op[0], draw_op[1], draw_op[2], draw_op[3], draw_op[4], draw_op[5], draw_op[6], draw_op[7]);
        }
        pending_draw_op = [ ];
    }
}

var last_frame_time = 0;

function update_callback(time)
{
    
    if (time - last_frame_time < 1000 / 24)
    {
        requestAnimationFrame(update_callback);
        return;
    }
    
    update();
    render();
    __render();
    
    last_frame_time = time;
    requestAnimationFrame(update_callback);
}

function on_load()
{
    launch()
    requestAnimationFrame(update_callback);
}

window.addEventListener("load", on_load, false);

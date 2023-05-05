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

var EXIT_SUCCESS = 0;
var EXIT_FAILURE = -1;

function exit(code) {
    throw "exit";
}

var ERR_NONE = 0;
var ERR_UNKNOWN = 1;

var ERR = ERR_NONE;

function get_err() {
    return ERR;
}

function set_err(value) {
    ERR = value;
}

function clear_err() {
    set_err(ERR_NONE);
}

var ERR_INVALID_LITERAL = 2;

function stoi(x)
{
    var y = new Number (x).valueOf();
    return y;
}

function stof(x)
{
    var y = new Number (x).valueOf();
    return y;
}

function itos(x) {
    return '' + x;
}

function ftos(x) {
    return '' + x;
}

function ctoi(x) {
    return x.charCodeAt(0);
}

function itoc(x) {
    return String.fromCharCode(x);
}

function islower(x) {
    return x.match('[a-z]+') != null;
}

function isupper(x) {
    return x.match('[A-Z]+') != null;
}

function tolower(x) {
    return x.toLowerCase();
}

function toupper(x) {
    return x.toUpperCase();
}

function isalpha(x) {
    return islower(x) || isupper(x);
}

function isdigit(x) {
    return x.match('[0-9]+') != null;
}

function isalnum(x) {
    return isalpha(x) || isdigit(x);
}

function isident(x) {
    return isalnum(x) || x == '_';
}

var strbuf = function () { }

strbuf.is_shark_class = true;
strbuf.prototype = new Object ();
strbuf.prototype.constructor = strbuf;

strbuf.prototype.construct = function () {
    this.buffer = '';
    return this;
};

strbuf.prototype.put = function(value) {
    this.buffer += value;
};

strbuf.prototype.write = function(value) {
    this.buffer += value;
};

strbuf.prototype.get = function()
{
    var get = this.buffer;
    this.buffer = '';
    return get;
};

var striter = function () { }

striter.is_shark_class = true;
striter.prototype = new Object ();
striter.prototype.constructor = strbuf;

striter.prototype.construct = function (data)
{
    this.data = data;
    this.pos = 0;
    return this;
};

striter.prototype.tell = function() {
    return this.pos;
};

striter.prototype.seek = function(pos) {
    this.pos = pos;
};

striter.prototype.next = function()
{
    var c = this.data.slice(this.pos, this.pos + 1);
    this.pos++;
    return c;
};

striter.prototype.empty = function(pos) {
    return this.pos >= this.data.length;
};

var strmap = function () { }

strmap.is_shark_class = true;
strmap.prototype = new Object ();
strmap.prototype.constructor = strmap;

strmap.prototype.construct = function () {
    this.data = { }; return this;
};

strmap.prototype.get = function(index) {
    return this.data[index];
};

strmap.prototype.set = function(index, value) {
    this.data[index] = value;
};

strmap.prototype.has = function(index) {
    return typeof(this.data[index]) == "number";
};

strmap.prototype.pop = function(index) {
    this.data[index] = undefined;
};

function write(data) {
    console.log(data);
}

var list = function () { }

list.is_shark_class = true;
list.prototype = new Object ();
list.prototype.constructor = list;

list.prototype.construct = function () {
    this.data = [ ]
    return this;
};

list.prototype.size = function() {
    return this.data.length;
};

list.prototype.push = function(value) {
    this.data.push(value);
};

list.prototype.pop = function() {
    return this.data.pop();
};

list.prototype.get = function(index) {
    return this.data[index];
};

list.prototype.set = function(index, value) {
    this.data[index] = value;
};

function idiv(x, y) {
    return Math.floor(x / y);
}

var pi = Math.PI;
var e = Math.E;

var abs = Math.abs;

var acos = Math.acos;
var asin = Math.asin;
var atan = Math.atan;
var atan2 = Math.atan2;

var cos = Math.cos;
var sin = Math.sin;
var tan = Math.tan;

var exp = Math.exp;
var log = Math.log;
// var log10 = Math.log10;
var pow = Math.pow;
var sqrt = Math.sqrt;

var ceil = Math.ceil;
var floor = Math.floor;

var min = Math.min;
var max = Math.max;

function random(x) {
    return Math.floor(x * Math.random());
}

function free(value) {
    // do nothing
}

var free_strbuf = free;
var free_striter = free;
var free_strmap = free;
var free_list = free;

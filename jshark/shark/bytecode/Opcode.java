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

package shark.bytecode;

public final class Opcode
{
public static final int
	END = 0,
    NULL = 1,
    TRUE = 2,
    FALSE = 3,
    LOAD_GLOBAL = 4,
    LOAD = 5,
    GET_FIELD = 6,
    ENTER_CLASS = 7,
    EXIT_CLASS = 8,
    DEFINE = 9,
    DEFINE_FIELD = 10,
    FUNCTION = 11,
    NOT_IMPLEMENTED = 12,
    EXIT = 13,
    DUP = 14,
    DROP = 15,
    SWAP = 16,
    MUL = 17,
    DIV = 18,
    MOD = 19,
    ADD = 20,
    SUB = 21,
    LT = 22,
    LE = 23,
    GT = 24,
    GE = 25,
    EQ = 26,
    NE = 27,
    IN = 28,
    NOT_IN = 29,
    NEG = 30,
    NOT = 31,
    FUNCTION_CALL = 32,
    METHOD_CALL = 33,
    GET_SLICE = 34,
    GET_INDEX = 35,
    SELF = 36,
    SUPER_CALL = 37,
    SIZEOF = 38,
    NEW = 39,
    INSTANCEOF = 40,
    ARRAY_NEW = 41,
    ARRAY_NEW_APPEND = 42,
    TABLE_NEW = 43,
    TABLE_NEW_INSERT = 44,
    CONST = 45,
    RETURN = 46,
    INSERT = 47,
    APPEND = 48,
    STORE_GLOBAL = 49,
    STORE = 50,
    SET_STATIC = 51,
    SET_FIELD = 52,
    SET_SLICE = 53,
    SET_INDEX = 54,
    GET_FIELD_TOP = 55,
    GET_INDEX_TOP = 56,
    GET_STATIC = 57,
    GET_STATIC_TOP = 58,
    IF = 59,
    JUMP = 60,
    LOOP = 61,
    ZERO = 62,
    INC = 63,
	OR = 64,
	AND = 65,
	SET_INDEX_AU = 66,
	SET_FIELD_AU = 67,
	SET_STATIC_AU = 68,
	ARRAY_CLOSE = 69,
	TABLE_CLOSE = 70;
}

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

import java.util.Arrays;
import java.util.HashMap;

import shark.core.RuntimeError;
import shark.core.WrapperException;
import shark.core.Function;

public class VirtualMachine
{
	public int PC;
	public byte[] code;
	public int TOS;
	public Object[] stack;
	
	public VirtualMachine()
	{
		PC = 0;
		code = null;
		TOS = 0;
		stack = new Object[256];
	}
	
	public int fetch() {
		return ((int) code[PC++]) & 0xFF;
	}
	
	public int fetch_short() {
		return ((int) code[PC++]) & 0xFF | (((int) code[PC++]) & 0xFF) << 8;
	}
	
	public int fetch_int() {
		return ((int) code[PC++]) & 0xFF | (((int) code[PC++]) & 0xFF) << 8 | (((int) code[PC++]) & 0xFF) << 16 | ((int) code[PC++] & 0xFF) << 24;
	}
	
	public void push(Object value) throws RuntimeError
	{
		stack[TOS++] = value;
		if (TOS >= stack.length)
			stack = Arrays.copyOf(stack, stack.length << 1);
	}
	
	public Object pop() throws RuntimeError
	{
		Object value = stack[--TOS];
		stack[TOS] = null;
		return value;
	}
	
	public static double binop(double x, double y, int i) throws RuntimeError
	{
		switch (i)
		{
		case Opcode.ADD:
			return x + y;
		case Opcode.SUB:
			return x - y;
		case Opcode.MUL:
			return x * y;
		case Opcode.DIV:
			return x / y;
		case Opcode.MOD:
			return x % y;
		default:
			throw new RuntimeError("Unknown operation " + i + ".");
		}
	}
	
	public Object execute(Module module, BytecodeFunction current_function, byte[] code, int base) throws RuntimeError
	{
		PC = 0;
		this.code = code;
		
		int PC_STORE = 0;
		
		HashMap<String, Object> globals = module.namespace;
		Object[] const_table = module.const_table;
		
		shark.core.Class current_class = null;
		shark.core.Array current_array = null;
		shark.core.Table current_table = null;
		
		double x;
		double y;
        
        int z;
        int w;
		
		Object xo;
		Object yo;
		Object zo;
		
		int argc;
		Function callee;
		shark.core.Object object;
		
		for (;;)
		{
			switch (fetch())
			{
			case Opcode.END:
				for (int i = TOS; i > base; i--)
					pop();
				return null;
			case Opcode.NULL:
				push(null);
				break;
			case Opcode.TRUE:
				push(true);
				break;
			case Opcode.FALSE:
				push(false);
				break;
			case Opcode.LOAD_GLOBAL:
				push(globals.get(const_table[fetch_short()]));
				break;
			case Opcode.LOAD:
				push(stack[base + fetch()]);
				break;
			case Opcode.GET_FIELD:
				push(((shark.core.Table) pop()).data.get(const_table[fetch_short()]));
				break;
			case Opcode.ENTER_CLASS:
				current_class = new shark.core.Class((String) const_table[fetch_short()], (shark.core.Class) pop());
				break;
			case Opcode.EXIT_CLASS:
				globals.put(current_class.name, current_class);
				current_class = null;
				break;
			case Opcode.DEFINE:
				globals.put((String) const_table[fetch_short()], pop());
				break;
			case Opcode.DEFINE_FIELD:
				fetch_short();
				break;
			case Opcode.FUNCTION:
				BytecodeFunction function = new BytecodeFunction();
				function.arity = fetch();
				function.name = (String) const_table[fetch_short()];
				function.set_owner_module(module);
				if (current_class != null) {
					function.is_method = true;
					function.owner_class = current_class;
					function.supermethod = current_class.methods.get(function.name);
					current_class.methods.put(function.name, function);
				} else {
					function.is_method = false;
					function.owner_class = null;
					function.supermethod = null;
					module.namespace.put(function.name, function);
				}
				int code_size = fetch_int();
				function.bytecode = Arrays.copyOfRange(code, PC, PC + code_size);
				PC += code_size;
				break;
			case Opcode.NOT_IMPLEMENTED:
				throw new RuntimeError ("function '" + current_function.name + "' is not implemented.");
			case Opcode.EXIT:
				int block_size = fetch();
				for (int i = 0; i < block_size; i++) pop();
				break;
			case Opcode.DUP:
				push(stack[TOS-1]);
				break;
			case Opcode.DROP:
				pop();
				break;
			case Opcode.SWAP:
				Object top = stack[TOS-1];
				stack[TOS-1] = stack[TOS-2];
				stack[TOS-2] = top;
				break;
			case Opcode.MUL:
				y = (double) pop();
				x = (double) pop();
				push(x * y);
				break;
			case Opcode.DIV:
				y = (double) pop();
				x = (double) pop();
				push(x / y);
				break;
			case Opcode.MOD:
				y = (double) pop();
				x = (double) pop();
				push(x % y);
				break;
			case Opcode.ADD:
				y = (double) pop();
				x = (double) pop();
				push(x + y);
				break;
			case Opcode.SUB:
				y = (double) pop();
				x = (double) pop();
				push(x - y);
				break;
			case Opcode.LT:
				y = (double) pop();
				x = (double) pop();
				push(x < y);
				break;
			case Opcode.LE:
				y = (double) pop();
				x = (double) pop();
				push(x <= y);
				break;
			case Opcode.GT:
				y = (double) pop();
				x = (double) pop();
				push(x > y);
				break;
			case Opcode.GE:
				y = (double) pop();
				x = (double) pop();
				push(x >= y);
				break;
			case Opcode.EQ:
				yo = pop();
				xo = pop();
				push(xo == yo || xo.equals(yo));
				break;
			case Opcode.NE:
				yo = pop();
				xo = pop();
				push(xo != yo && xo != null && !xo.equals(yo));
				break;
			case Opcode.IN:
				yo = pop();
				xo = pop();
				push(((shark.core.Table) yo).data.containsKey(xo));
				break;
			case Opcode.NOT_IN:
				yo = pop();
				xo = pop();
				push(!((shark.core.Table) yo).data.containsKey(xo));
				break;
			case Opcode.NEG:
				push(-(double) pop());
				break;
			case Opcode.NOT:
				push(!(boolean) pop());
				break;
			case Opcode.FUNCTION_CALL:
				argc = fetch();
				callee = (Function) stack[TOS - argc - 1];
				if (callee.arity != argc)
					throw new RuntimeError ("arity mismatch, expected exactly " + callee.arity + " arguments but got " + argc + ".");
				PC_STORE = PC;
				try {
					xo = callee.call(this);
				} catch (RuntimeError e) {
					e.stack_trace.add(callee);
					throw e;
				} catch (Throwable e) {
					RuntimeError error = new WrapperException (e);
					error.stack_trace.add(callee);
					throw error;
				}
				if (!(callee instanceof BytecodeFunction))
					for (int i = 0; i < argc; i++)
						pop();
				pop();
				push(xo);
				PC = PC_STORE;
				this.code = code;
				break;
			case Opcode.METHOD_CALL:
				argc = fetch();
				object = (shark.core.Object) stack[TOS - argc - 1];
				String method_name = (String) const_table[fetch_short()];
				if (object == null) {
					System.out.println(current_function.name + " " + PC);
					for (int i = base; i < TOS; i++) {
						System.out.println(stack[i]);
					}
				}
				callee = object.type.methods.get(method_name);
				if (callee == null)
					throw new RuntimeError (object.type.name + " object has no method " + method_name);
				if (callee.arity != argc)
					throw new RuntimeError ("arity mismatch, expected exactly " + callee.arity + " arguments but got " + argc + ".");
				PC_STORE = PC;
				try {
					xo = callee.call(this);
				} catch (RuntimeError e) {
					e.stack_trace.add(callee);
					throw e;
				} catch (Throwable e) {
					RuntimeError error = new WrapperException (e);
					error.stack_trace.add(callee);
					throw error;
				}
				if (!(callee instanceof BytecodeFunction)) {
					for (int i = 0; i < argc; i++)
						pop();
					pop();
				}
				push(xo);
				PC = PC_STORE;
				this.code = code;
				break;
			case Opcode.GET_INDEX:
				yo = pop();
				xo = pop();
				if (xo instanceof shark.core.Array) {
					push(((shark.core.Array) xo).get((int)(double) yo));
				} else if (xo instanceof shark.core.Table) {
					push(((shark.core.Table) xo).data.get(yo));
				} else {
					throw new RuntimeError("can't get index " + yo.toString() + " of object " + xo.toString() + ".");
				}
				break;
			case Opcode.SELF:
				push(stack[base]);
				break;
			case Opcode.SUPER_CALL:
				argc = fetch();
				object = (shark.core.Object) stack[TOS - argc - 1];
				callee = object.type.methods.get(const_table[fetch_short()]);
				if (callee.supermethod == null)
					throw new RuntimeError ("method " + callee.name + " has no supermethod.");
				callee = callee.supermethod;
				if (callee.arity != argc)
					throw new RuntimeError ("arity mismatch, expected exactly " + callee.arity + " arguments but got " + argc + ".");
				PC_STORE = PC;
				try {
					xo = callee.call(this);
				} catch (RuntimeError e) {
					e.stack_trace.add(callee);
					throw e;
				} catch (Throwable e) {
					RuntimeError error = new WrapperException (e);
					error.stack_trace.add(callee);
					throw error;
				}
				if (!(callee instanceof BytecodeFunction)) {
					for (int i = 0; i < argc; i++)
						pop();
					pop();
				}
				push(xo);
				PC = PC_STORE;
				this.code = code;
				break;
			case Opcode.SIZEOF:
				xo = pop();
				if (xo instanceof shark.core.Array) {
					push((double) ((shark.core.Array) xo).size());
				} else if (xo instanceof String) {
					push((double) ((String) xo).length());
				} else if (xo instanceof shark.core.Table) {
					push((double) ((shark.core.Table) xo).data.size());
				} else {
					throw new RuntimeError ("can't get the sizeof object " + xo.toString() + ".");
				}
				break;
			case Opcode.NEW:
				argc = fetch();
				object = ((shark.core.Class) stack[TOS-argc-1]).create();
				stack[TOS - argc - 1] = object;
				callee = object.type.methods.get("init");
				if (callee.arity != argc)
					throw new RuntimeError ("arity mismatch, expected exactly " + callee.arity + " arguments but got " + argc + ".");
				PC_STORE = PC;
				try {
					callee.call(this);
				} catch (RuntimeError e) {
					e.stack_trace.add(callee);
					throw e;
				} catch (Throwable e) {
					RuntimeError error = new WrapperException (e);
					error.stack_trace.add(callee);
					throw error;
				}
				if (!(callee instanceof BytecodeFunction)) {
					for (int i = 0; i < argc; i++)
						pop();
					pop();
				}
				push(object);
				PC = PC_STORE;
				this.code = code;
				break;
			case Opcode.INSTANCEOF:
				yo = pop();
				xo = pop();
				push(((shark.core.Object) xo).instanceOf((shark.core.Class) yo));
				break;
			case Opcode.ARRAY_NEW:
				push(current_array);
				current_array = new shark.core.Array();
				break;
			case Opcode.ARRAY_NEW_APPEND:
				current_array.add(pop());
				break;
			case Opcode.TABLE_NEW:
				push(current_table);
				current_table = new shark.core.Table();
				break;
			case Opcode.TABLE_NEW_INSERT:
				yo = pop();
				xo = pop();
				current_table.data.put(xo, yo);
				break;
			case Opcode.CONST:
				push(const_table[fetch_short()]);
				break;
			case Opcode.RETURN:
				xo = pop();
				for (int i = TOS; i > base; i--)
					pop();
				return xo;
			case Opcode.INSERT:
				zo = pop();
				yo = pop();
				xo = pop();
				((shark.core.Array) xo).add((int) (double) yo, zo);
				break;
			case Opcode.APPEND:
				yo = pop();
				xo = pop();
				((shark.core.Array) xo).add(yo);
				break;
			case Opcode.STORE_GLOBAL:
				globals.put((String) const_table[fetch_short()], pop());
				break;
			case Opcode.STORE:
				stack[base + fetch()] = pop();
				break;
			case Opcode.SET_STATIC:
				yo = pop();
				xo = pop();
				((shark.core.Module) xo).namespace.put((String) const_table[fetch_short()], yo);
				break;
			case Opcode.SET_FIELD:
				yo = pop();
				xo = pop();
				((shark.core.Table) xo).data.put((String) const_table[fetch_short()], yo);
				break;
			case Opcode.SET_INDEX:
				zo = pop();
				yo = pop();
				xo = pop();
				if (xo instanceof shark.core.Array) {
					((shark.core.Array) xo).set((int) (double) yo, zo);
				} else if (xo instanceof shark.core.Table) {
					((shark.core.Table) xo).data.put(yo, zo);
				} else {
					throw new RuntimeError("can't set index " + yo.toString() + " of object " + xo.toString() + ".");
				}
				break;
			case Opcode.GET_FIELD_TOP:
				push(((shark.core.Table) stack[TOS-1]).data.get(const_table[fetch_short()]));
				break;
			case Opcode.GET_INDEX_TOP:
				yo = stack[TOS-1];
				xo = stack[TOS-2];
				if (xo instanceof shark.core.Array) {
					push(((shark.core.Array) xo).get((int) (double) yo));
				} else if (xo instanceof shark.core.Table) {
					push(((shark.core.Table) xo).data.get(yo));
				} else {
					throw new RuntimeError("can't get index " + yo.toString() + " of object " + xo.toString() + ".");
				}
				break;
			case Opcode.GET_STATIC:
				push(((shark.core.Module) pop()).namespace.get((String) const_table[fetch_short()]));
				break;
			case Opcode.GET_STATIC_TOP:
				push(((shark.core.Module) stack[TOS-1]).namespace.get((String) const_table[fetch_short()]));
				break;
			case Opcode.IF:
				if ((boolean) pop() == false)
					PC = PC + fetch_short();
				else
					fetch_short();
				break;
			case Opcode.JUMP:
				PC = PC + fetch_short();
				break;
			case Opcode.LOOP:
				PC = PC - fetch_short();
				break;
			case Opcode.ZERO:
				push((double) 0);
				break;
			case Opcode.INC:
				int local = fetch();
				double value = (double) stack[base + local];
				stack[base + local] = value + 1;
				break;
			case Opcode.OR:
				xo = stack[TOS-1];
				if (xo == null || (xo instanceof Boolean && (boolean) xo == false)) {
					pop();
					fetch_short();
				} else {
					PC += fetch_short();
				}
				break;
			case Opcode.AND:
				xo = stack[TOS-1];
				if ((xo instanceof Boolean && (boolean) xo == true) || (!(xo instanceof Boolean) && xo != null)) {
					pop();
					fetch_short();
				} else {
					PC += fetch_short();
				}
				break;
			case Opcode.SET_INDEX_AU:
				zo = pop();
				yo = pop();
				xo = pop();
				if (xo instanceof shark.core.Array) {
					((shark.core.Array) xo).set((int) (double) yo, binop((double) ((shark.core.Array) xo).get((int) yo), (double) zo, fetch()));
				} else if (xo instanceof shark.core.Table) {
					((shark.core.Table) xo).data.put(yo, binop((double) ((shark.core.Table) xo).data.get(yo), (double) zo, fetch()));
				} else {
					throw new RuntimeError("can't set index " + yo.toString() + " of object " + xo.toString() + ".");
				}
				break;
			case Opcode.SET_FIELD_AU:
				yo = pop();
				xo = pop();
				int field_au_op = fetch();
				String field_au = (String) const_table[fetch_short()];
				((shark.core.Table) xo).data.put(field_au, binop((double) ((shark.core.Table) xo).data.get(field_au), (double) yo, field_au_op));
				break;
			case Opcode.SET_STATIC_AU:
				yo = pop();
				xo = pop();
				int static_au_op = fetch();
				String static_au = (String) const_table[fetch_short()];
				((shark.core.Module) xo).namespace.put(static_au, binop((double) ((shark.core.Module) xo).namespace.get(static_au), (double) yo, static_au_op));
				break;
			case Opcode.ARRAY_CLOSE:
				xo = pop();
				push(current_array);
				current_array = (shark.core.Array) xo;
				break;
			case Opcode.TABLE_CLOSE:
				xo = pop();
				push(current_table);
				current_table = (shark.core.Table) xo;
				break;
            case Opcode.BAND:
				z = (int) (double) pop();
				w = (int) (double) pop();
				push(w & z);
				break;
			case Opcode.BOR:
				z = (int) (double) pop();
				w = (int) (double) pop();
				push(w | z);
				break;
            case Opcode.BXOR:
				z = (int) (double) pop();
				w = (int) (double) pop();
				push(w | z);
				break;
            case Opcode.BSHL:
				z = (int) (double) pop();
				w = (int) (double) pop();
				push(w << z);
				break;
            case Opcode.BSHR:
				z = (int) (double) pop();
				w = (int) (double) pop();
				push(w >> z);
				break;
            case Opcode.BNOT:
				push(~((int) (double) pop()));
				break;
            default:
				throw new RuntimeError("unsupported operation: " + code[--PC]);
			}
		}
	}

	public static Object DUMP(Object object)
	{
		System.out.println(object);
		return object;
	}
}

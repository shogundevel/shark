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

package shark.core;

import java.util.ArrayList;

public class RuntimeError extends Exception
{
	static final long serialVersionUID = 1L;
	
	private String message;
	public ArrayList<Function> stack_trace;
	
	public RuntimeError() {
		this(null);
	}
	
	public RuntimeError(String message) {
		this.message = message;
		stack_trace = new ArrayList<Function> ();
	}
	
	@Override
	public String toString() {
		return message == null ? "unknown error." : message;
	}
	
	public void dump_stack_trace()
	{
		for (Function frame : stack_trace)
		{
			if (frame.owner_class != null) {
				System.err.println("in module '" + frame.owner_module.name + "', class '" + frame.owner_class.name + "', method '" + frame.name + "'.");
			} else {
				System.err.println("in module '" + frame.owner_module.name + "', function '" + frame.name + "'.");
			}
		}
	}
}

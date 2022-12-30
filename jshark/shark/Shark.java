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

package shark;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

import shark.bytecode.ModuleReader;
import shark.bytecode.VirtualMachine;
import shark.core.RuntimeError;
import shark.core.WrapperException;
import shark.core.Module;
import shark.core.Function;
import shark.core.Array;
import shark.lib.ModuleLoader;
import shark.lib.Standard;
import shark.lib.Standard.Exit;

public class Shark
{
	public static void main(String[] args)
	{
		if (args.length >= 1) {
			Standard.load();
			VirtualMachine vm = new VirtualMachine ();
			try {
				ModuleLoader.add_import_path(Standard.path_get_base(args[0]));
				Module main_module = new ModuleReader(new FileInputStream(args[0]), Standard.path_get_tail(args[0])).read().execute(vm);
				Object main = main_module.namespace.get("main");
				if (main instanceof Function) {
					if (((Function) main).arity != 1) {
						System.err.println("Expected 'main' function to take exactly one argument, not " + ((Function) main).arity + ". Execution aborted.");
						return;
					}
					Array args_list = new Array ();
					args_list.add(args[0]);
					for (int i = 1; i < args.length; i++)
						args_list.add(args[i]);
					vm.push(args_list);
					((Function) main).call(vm);
				}
			} catch (FileNotFoundException e) {
				System.err.println("Can't find input file '" + args[0] + "', execution aborted.");
				return;
			} catch (Exit e) {
				// do nothing
			} catch (RuntimeError e) {
				System.err.println("A shark.core.RuntimeError happened, here are the details:");
				e.dump_stack_trace();
				System.err.println("And this is the root exception that was thrown:");
				if (e instanceof WrapperException) {
					((WrapperException) e).error.printStackTrace();
				} else {
					e.printStackTrace();
				}
				return;
			} catch (IOException e) {
				System.err.println("A java.io.IOException happened during module reading (possibly because of corrupted/malformed binary data): execution aborted.");
				e.printStackTrace();
				return;
			}
		} else {
			System.out.println("Usage: jshark <path/to/bytecode_file.sho> <arg1> <arg2> ... <argN>");
		}
	}
}

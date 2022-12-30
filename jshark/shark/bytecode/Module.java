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

import java.util.*;
import java.io.IOException;

import shark.core.RuntimeError;
import shark.lib.ModuleLoader;

public final class Module extends shark.core.Module
{
	public ArrayList<Import> imports;
	public Object[] const_table;
	public byte[] bytecode;
	
	public Module(String name)
	{
		super(name);
		imports = null;
		const_table = null;
		bytecode = null;
	}
	
	@SuppressWarnings("unchecked")
	public Module execute(VirtualMachine loader) throws RuntimeError, IOException
	{
		for (Import import_decl : imports)
		{
			shark.core.Module imported = ModuleLoader.load_module(import_decl.import_path, loader);
			Object target = import_decl.import_target;
			if (target instanceof String) {
				namespace.put((String) target, imported);
			} else {
				for (String name : (ArrayList<String>) target)
					namespace.put(name, imported.namespace.get(name));
			}
		}
		
		loader.execute(this, null, bytecode, loader.TOS);
		
		imports = null;
		bytecode = null;
		
		return this;
	}
}

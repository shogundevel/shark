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

package shark.lib;

import shark.core.Module;
import shark.core.RuntimeError;

import shark.bytecode.VirtualMachine;
import shark.bytecode.ModuleReader;

import java.util.*;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

public class ModuleLoader
{
	private static Object sync = new Object ();
	
	public static ArrayList<String> module_path = new ArrayList<String> ();
	
	public static void add_import_path(String import_path)
	{
		synchronized(sync)
		{
			if (import_path.endsWith("\\") || import_path.endsWith("/"))
				module_path.add(import_path);
			else
				module_path.add(import_path + "/");
		}
	}
	
	public static HashMap<String, Module> archive_record = new HashMap<String, Module> ();
	public static HashMap<String, Module> module_record = new HashMap<String, Module> ();
	public static HashMap<String, Module> import_record = new HashMap<String, Module> ();
	
	public static Module load_archive(String path) throws RuntimeError, IOException
	{
		synchronized(sync)
		{
			if (archive_record.containsKey(path))
				return archive_record.get(path);
			else {
				for (String import_path : module_path)
				{
					try {
						return new ModuleReader(new FileInputStream(Standard.path_join(import_path, path)), path).read();
					} catch (FileNotFoundException e) {
						continue;
					}
				}
				
				throw new RuntimeError("Can't find archive '" + path + "'.");
			}
		}
	}
	
	public static Module load_module(String path, VirtualMachine loader) throws RuntimeError, IOException
	{
		synchronized(sync)
		{
			if (import_record.containsKey(path)) {
				return import_record.get(path);
			} else if (module_record.containsKey(path)) {
				shark.bytecode.Module module = (shark.bytecode.Module) module_record.get(path);
				import_record.put(path, module);
				return module.execute(loader);
			} else {
				throw new RuntimeError("Can't find module '" + path + "'.");
			}
		}
	}
	
	public static void add_module(Module module, String path)
	{
		synchronized(sync)
		{
			if (module instanceof shark.bytecode.Module)
				module_record.put(path, module);
			else
				import_record.put(path, module);
		}
	}
}

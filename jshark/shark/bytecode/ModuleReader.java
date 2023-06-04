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

import java.io.FileInputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Arrays;

import shark.core.RuntimeError;
import shark.lib.ModuleLoader;

public class ModuleReader
{
	private static final int CONST_INT = 0, CONST_FLOAT = 1, CONST_CHAR = 2, CONST_STR = 3, CONST_SYMBOL = 4;
	private FileInputStream source;
	private String name;
	private shark.bytecode.Module module;
	
	public ModuleReader(FileInputStream source, String name)
	{
		this.source = source;
		this.name = name;
	}
	private int fetch() throws IOException {
		return ((int) source.read()) & 0xFF;
	}
	
	private int fetch_short() throws IOException {
		return source.read() | (source.read() << 8);
	}
	
	private int fetch_int() throws IOException {
		return source.read() | (source.read() << 8) | (source.read() << 16) | (source.read() << 24);
	}
	
	private String fetch_str() throws IOException
	{
        byte[] data = new byte[fetch_int()];
        source.read(data);
		return new String(data);
	}
	
	public Module read_module(String name) throws IOException, RuntimeError
	{
		module = new Module (name);
		
		int import_section_size = fetch_short();
		module.imports = new ArrayList<Import> ();
		
		for (int i = 0; i < import_section_size; i++)
		{
			String import_path = fetch_str();
			if (source.read() == 0)
				module.imports.add(new Import(import_path, fetch_str()));
			else {
				int target_count = fetch_short();
				ArrayList<String> target = new ArrayList<String> ();
				for (int j = 0; j < target_count; j++)
					target.add(fetch_str());
				module.imports.add(new Import(import_path, target));
			}
		}
		
		int const_table_size = fetch_int();
		module.const_table = new Object[const_table_size];
		
		for (int i = 0; i < const_table_size; i++)
		{
			Object value = null;
			
			switch (source.read())
			{
			case CONST_INT:
				value = (double) fetch_int();
				break;
			case CONST_FLOAT:
				value = Double.parseDouble(fetch_str());
				break;
			case CONST_CHAR:
                int size = fetch_int();
                if (size == 1)
                    value = (char) source.read();
                else
                    throw new RuntimeError ("can't decode non-ascii character.");
				break;
			case CONST_STR:
			case CONST_SYMBOL:
				value = fetch_str();
				break;
			default:
				throw new RuntimeError ("invalid constant type.");
			}
			
			module.const_table[i] = value;
		}
		
        module.bytecode = new byte[fetch_int()];
        source.read(module.bytecode);
		
		return module;
	}
	
	public Module read() throws IOException, RuntimeError
	{
		ModuleLoader.archive_record.put(name, null);
		
		int main_name_size = fetch();
        byte[] name_data = new byte[256];
        source.read(name_data, 0, main_name_size);
		String main_name = new String(Arrays.copyOf(name_data, main_name_size));
		
		int import_table_size = fetch();
		
		for (int i = 0; i < import_table_size; i++)
		{
			int archive_name_size = fetch();
            source.read(name_data, 0, archive_name_size);
			String archive_name = new String(Arrays.copyOf(name_data, archive_name_size));
			ModuleLoader.load_archive(archive_name);
		}
		
		while (true)
		{
			int c = source.read();
			if (c == -1) break;
			c |= (source.read() << 8) | (source.read() << 16) | (source.read() << 24);
			byte[] c_data = new byte[c];
            source.read(c_data);
            String module_name = new String (c_data);
			Module module = read_module(module_name);
			ModuleLoader.module_record.put(module_name, module);
		}
		
		return (Module) ModuleLoader.module_record.get(main_name);
	}
}

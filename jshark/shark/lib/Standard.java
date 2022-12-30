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

import shark.bytecode.VirtualMachine;
import shark.core.Array;
import shark.core.Class;
import shark.core.Function;
import shark.core.Module;
import shark.core.RuntimeError;
import shark.core.WrapperException;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.InputStreamReader;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Arrays;
import java.util.List;
import java.util.Objects;
import java.util.Scanner;
import java.util.Iterator;

public class Standard
{
	public static class Exit extends RuntimeError
	{
		private static final long serialVersionUID = 1L;
		
		int exit_code;
		
		public Exit(int code)
		{
			super("");
			exit_code = code;
		}
	}
	
	public static int EXIT_FAILURE = -1, EXIT_SUCCESS = 0;
	
	public static void exit(int exit_code) throws RuntimeError {
		throw new Exit(exit_code);
	}
	
	public static int errno = 0;
	
	public static final int ERR_NONE = 0, ERR_UNKNOWN = 1, ERR_INVALID_LITERAL = 2, ERR_FILE_NOT_FOUND = 3;
	
	public static int get_err() {
		return errno;
	}
	
	public static boolean has_err() {
		return errno != ERR_NONE;
	}
	
	public static void set_err(int value) {
		errno = value;
	}
	
	public static void clear_err() {
		errno = ERR_NONE;
	}
	
	public static class Error extends RuntimeError
	{
		private static final long serialVersionUID = 1L;
		
		public Error(String message) {
			super(message);
		}
	}
	
	public static String path_get_base(String path)
	{
		int slash_index = path.lastIndexOf((int) '/');
		int backslash_index = path.lastIndexOf((int) '\\');
		int split_index = slash_index > backslash_index ? slash_index : backslash_index;
		if (split_index != -1)
			return path.substring(0, split_index);
		else
			return "";
	}
	
	public static String path_get_tail(String path)
	{
		int slash_index = path.lastIndexOf((int) '/');
		int backslash_index = path.lastIndexOf((int) '\\');
		int split_index = slash_index > backslash_index ? slash_index : backslash_index;
		if (split_index != -1)
			return path.substring(split_index + 1, path.length());
		else
			return path;
	}
	
	public static String path_get_ext(String path)
	{
		int slash_index = path.lastIndexOf((int) '/');
		int backslash_index = path.lastIndexOf((int) '\\');
		slash_index = slash_index > backslash_index ? slash_index : backslash_index;
		int dot_index = path.lastIndexOf((int) '.');
		int split_index = slash_index > dot_index ? slash_index : dot_index;
		if (split_index != -1)
			return path.substring(split_index + 1, path.length());
		else
			return "";
	}
	
	public static String path_remove_ext(String path)
	{
		int slash_index = path.lastIndexOf((int) '/');
		int backslash_index = path.lastIndexOf((int) '\\');
		slash_index = slash_index > backslash_index ? slash_index : backslash_index;
		int dot_index = path.lastIndexOf((int) '.');
		int split_index = slash_index > dot_index ? slash_index : dot_index;
		if (split_index != -1)
			return path.substring(0, split_index);
		else
			return path;
	}
	
	public static String path_join(String xpath, String ypath)
	{
        boolean xslash = false;
        boolean yslash = false;
        if (xpath.length() != 0) {
            int xchar = xpath.codePointAt(xpath.length() - 1);
            xslash = xchar == (int) '/' || xchar == (int) '\\';
        }
        if (ypath.length() != 0) {
            int ychar = ypath.codePointAt(0);
            yslash = ychar == (int) '/' || ychar == (int) '\\';
        }
		if (xslash) {
			if (yslash) {
				return xpath + ypath.substring(1);
			} else {
				return xpath + ypath;
			}
		} else if (yslash) {
			return xpath + ypath;
		} else if (xpath.length() == 0) {
            return ypath;
        } else {
			return xpath + '/' + ypath;
		}
	}
	
	public static String normalize(Object value)
	{
		if (value instanceof Double && ((double) (long) (double) value) == (double) value) {
			return Long.toString((long) (double) value);
		} else {
			return Objects.toString(value);
		}
	}
	
	public static String format(String format, Array args)
	{
		String[] parts = format.split("%");
		StringBuilder builder = new StringBuilder();
		int format_length = Integer.min(args.size(), parts.length);
		for (int i = 0; i < format_length; i++)
		{
			builder.append(parts[i]);
			builder.append(normalize(args.get(i)));
		}
		if (args.size() > format_length) {
			for (int i = format_length; i < args.size(); i++)
				builder.append(args.get(i));
		} else {
			for (int i = format_length; i < parts.length; i++)
				builder.append(normalize(parts[i]));
		}
		return builder.toString();
	}
	
	public static String normal(String data)
	{
		StringBuilder buffer = new StringBuilder();
		for (int i = 1; i < data.length() - 1; i++)
		{
			int c = data.codePointAt(i);
			if (c == '\\')
			{
				switch (data.codePointAt(++i))
				{
				case '0': 	c = '\0'; break;
				case 'r': 	c = '\r'; break;
				case 'n': 	c = '\n'; break;
				case 't': 	c = '\t'; break;
				case '\\': 	c = '\\'; break;
				case '"': 	c = '"'; break;
				case '\'': 	c = '\''; break;
				}
			}
			buffer.appendCodePoint(c);
		}
		return buffer.toString();
	}
	
	public static String quote(String data)
	{
		StringBuilder buffer = new StringBuilder();
		buffer.append("\"");
		for (int i = 0; i < data.length(); i++)
		{
			int c = data.codePointAt(i);
			switch (c)
			{
			case '\0':
				buffer.append("\\0");
				continue;
			case '\r':
				buffer.append("\\r");
				continue;
			case '\n':
				buffer.append("\\n");
				continue;
			case '\t':
				buffer.append("\\t");
				continue;
			case '\\':
				buffer.append("\\\\");
				continue;
			case '"':
				buffer.append("\\\"");
				continue;
			case '\'':
				buffer.append("\\'");
				continue;
			default:
				buffer.appendCodePoint(c);
				continue;
			}
		}
		buffer.append("\"");
		return buffer.toString();
	}
	
	public static int BUFFER_SIZE_UNKNOWN = -1;
	
	public static class AbstractClass extends Class
	{
		public AbstractClass(String name, Class parent) {
			super(name, parent);
		}

		@Override
		public shark.core.Object create() throws RuntimeError {
			throw new RuntimeError ("Can't instantiate an abstract class.");
		}
	}
	
	public static class AbstractFunction extends Function
	{
		@Override
		public Object call(VirtualMachine caller) throws RuntimeError {
			return new RuntimeError ("Function '" + name + "' is not implemented.");
		}
	}
	
	public static class strbuf extends shark.core.Object
	{
		public StringBuilder buffer;
		public int start;
		
		strbuf(Class type)
		{
			this.type = type;
			buffer = new StringBuilder ();
		}
		
		public int size() {
			return buffer.length();
		}
		
		public void put(char value) {
			buffer.append(value);
		}
		
		public void puts(String data) {
			buffer.append(data);
		}
		
		public void printf(String format, Array args) {
			buffer.append(format(format, args));
		}
		
		public void maybe_shrink()
		{
			if (start > buffer.length() / 2)
			{
				buffer = new StringBuilder(buffer.substring(start));
				start = 0;
			}
		}
		
		public int fetch()
		{
			maybe_shrink();
			if (start < buffer.length())
				return buffer.codePointAt(start++);
			else
				return '\0';
		}
		
		public String read(int count)
		{
			int size = Integer.min(start + count, buffer.length()) - start;
			String read = buffer.substring(start, start + size);
			start += size;
			maybe_shrink();
			return read;
		}
		
		public String read_all()
		{
			String data = buffer.toString();
			buffer = new StringBuilder();
			start = 0;
			return data;
		}
		
		public boolean at_end() {
			return start == buffer.length();
		}
	}
	
	public static class bytes extends shark.core.Object
	{
		public static Class BYTES_CLASS = null;
		
		private byte[] data;
		private int length;
		
		public bytes()
		{
			this.type = BYTES_CLASS;
			this.data = new byte[32];
			this.length = 0;
		}
		
		public bytes(byte[] data)
		{
			this.type = BYTES_CLASS;
			this.data = data;
			this.length = data.length;
		}
		
		public void maybe_grow()
		{
			if (length >= data.length)
				data = Arrays.copyOf(data, length << 1);
		}
		
		public void put(byte value)
		{
			data[length++] = value;
			maybe_grow();
		}
		
		public void put_short(short value)
		{
			put((byte) (value & 0xFF));
			put((byte) ((value >> 8) & 0xFF));
		}
		
		public void put_int(int value)
		{
			put((byte) (value & 0xFF));
			put((byte) ((value >> 8) & 0xFF));
			put((byte) ((value >> 16) & 0xFF));
			put((byte) ((value >> 24) & 0xFF));
		}
		
		public void puts(bytes data)
		{
			for (int i = 0; i < data.length; i++) {
				put(data.data[i]);
			}
		}
		
		public int tell() {
			return length;
		}
		
		public void patch(int pos, byte value) {
			data[pos] = value;
		}
		
		public void patch_short(int pos, short value)
		{
			data[pos] = (byte) (value & 0xFF);
			data[pos+1] = (byte) ((value >> 8) & 0xFF);
		}
		
		public void patch_int(int pos, int value)
		{
			data[pos] = (byte) (value & 0xFF);
			data[pos+1] = (byte) ((value >> 8) & 0xFF);
			data[pos+2] = (byte) ((value >> 16) & 0xFF);
			data[pos+3] = (byte) ((value >> 24) & 0xFF);
		}
		
		public int get(int index) {
			return ((int) data[index]) & 0xFF;
		}
	}
	
	public static bytes encode(String data) {
		return new bytes(data.getBytes(StandardCharsets.UTF_8));
	}
	
	public static String decode(bytes data) {
		return new String (data.data, StandardCharsets.UTF_8);
	}
	
	public static class text_file extends shark.core.Object
	{
		public FileReader in;
		public FileWriter out;
		
		public static Class FILE_CLASS = null;
		
		text_file()
		{
			type = FILE_CLASS;
			in = null;
			out = null;
		}
		
		public int size() {
			return BUFFER_SIZE_UNKNOWN;
		}
		
		public void put(char value) throws RuntimeError
		{
			try {
				out.append(value);
			} catch (IOException e) {
				throw new WrapperException (e);
			}
		}
		
		public void puts(String data) throws RuntimeError
		{
			try {
				out.append(data);
			} catch (IOException e) {
				throw new WrapperException (e);
			}
		}
		
		public void printf(String format, Array args) throws RuntimeError
		{
			try {
				out.append(format(format, args));
			} catch (IOException e) {
				throw new WrapperException (e);
			}
		}
		
		public char fetch() throws RuntimeError
		{
			try {
				int c = in.read();
				if (c != -1)
					return (char) c;
				else
					return '\0';
			} catch (IOException e) {
				throw new WrapperException (e);
			}
		}
		
		public String read(int count) throws RuntimeError
		{
			try {
				char[] buffer = new char[count * 2];
				int size = in.read(buffer);
				return String.valueOf(buffer, 0, size);
			} catch (IOException e) {
				throw new WrapperException (e);
			}
		}
		
		public boolean at_end() throws RuntimeError
		{
			try {
				return !in.ready();
			} catch (IOException e) {
				throw new WrapperException (e);
			}
		}
		
		public void close() throws RuntimeError
		{
			try {
				if (in != null)
					in.close();
				else
					out.close();
			} catch (IOException e) {
				throw new WrapperException (e);
			}
		}
	}
	
	public static class binary_file extends shark.core.Object
	{
		public FileInputStream in;
		public FileOutputStream out;
		
		public static Class FILE_CLASS = null;
		
		binary_file()
		{
			type = FILE_CLASS;
			in = null;
			out = null;
		}
		
		public int size() {
			return BUFFER_SIZE_UNKNOWN;
		}
		
		public void put(byte value) throws RuntimeError
		{
			try {
				out.write(value);
			} catch (IOException e) {
				throw new WrapperException (e);
			}
		}
		
		public void puts(bytes data) throws RuntimeError
		{
			try {
				out.write(data.data, 0, data.length);
			} catch (IOException e) {
				throw new WrapperException (e);
			}
		}
		
		public int fetch() throws RuntimeError
		{
			try {
				int c = in.read();
				if (c != -1)
					return c;
				else
					return 0;
			} catch (IOException e) {
				throw new WrapperException (e);
			}
		}
		
		public bytes read(int count) throws RuntimeError
		{
			try {
                byte[] data = new byte[count];
                int size = in.read(data);
				return new bytes (Arrays.copyOf(data, size));
			} catch (IOException e) {
				throw new WrapperException (e);
			}
		}
		
		public boolean at_end() throws RuntimeError
		{
			try {
				return in.available() == 0;
			} catch (IOException e) {
				throw new WrapperException (e);
			}
		}
		
		public void close() throws RuntimeError
		{
			try {
				if (in != null)
					in.close();
				else
					out.close();
			} catch (IOException e) {
				throw new WrapperException (e);
			}
		}
	}
	
	public static shark.core.Object open(String filename, String mode) throws RuntimeError
	{
		if (mode.length() == 1)
		{
			text_file file = new text_file ();
			try {
				if (mode.equals("r")) {
					try {
						file.in = new FileReader(filename);
					} catch (FileNotFoundException e) {
						set_err(ERR_FILE_NOT_FOUND);
						return null;
					}
				} else if (mode.equals("w")) {
					try {
						file.out = new FileWriter(filename);
					} catch (FileNotFoundException e) {
						set_err(ERR_FILE_NOT_FOUND);
						return null;
					}
				} else {
					throw new RuntimeError ("Invalid open mode in call to 'open': '" + mode + "'.");
				}
			} catch (IOException e) {
				throw new WrapperException (e);
			}
			return file;
		}
		else if (mode.length() == 2)
		{
			binary_file file = new binary_file ();
			if (mode.equals("rb")) {
				try {
					file.in = new FileInputStream(filename);
				} catch (FileNotFoundException e) {
					set_err(ERR_FILE_NOT_FOUND);
					return null;
				}
			} else if (mode.equals("wb")) {
				try {
					file.out = new FileOutputStream(filename);
				} catch (FileNotFoundException e) {
					set_err(ERR_FILE_NOT_FOUND);
					return null;
				}
			} else {
				throw new RuntimeError ("Invalid open mode in call to 'open': '" + mode + "'.");
			}
			return file;
		} else {
			throw new RuntimeError ("Invalid open mode in call to 'open': '" + mode + "'.");
		}
	}
	
	public static void put(char value) {
		System.out.append(value);
	}
	
	public static void puts(String value) {
		System.out.print(value);
	}
	
	public static void printf(String format, Array args) {
		System.out.print(format(format, args));
	}
	
	private static InputStreamReader input_reader = new InputStreamReader(System.in);
	
	public static char fetch() throws RuntimeError
	{
		try {
			int c = input_reader.read();
			if (c == -1) {
				return '\0';
			} else {
				return (char) c;
			}
		} catch (IOException e) {
			throw new WrapperException (e);
		}
	}
	
	public static String read_line()
	{
		Scanner scanner = new Scanner (System.in);
		String line = scanner.nextLine();
		scanner.close();
		return line;
	}
	
	public static Object util_copy(Object source) throws RuntimeError
	{
		if (source instanceof shark.core.Table) {
			shark.core.Table copy = new shark.core.Table(((shark.core.Table) source).type);
			copy.data.putAll(((shark.core.Table) source).data);
			return copy;
		} else if (source instanceof Array) {
			Array copy = new Array ();
			copy.addAll((Array) source);
			return copy;
		} else {
			throw new RuntimeError ("Can't copy object " + source.toString() + ".");
		}
	}
	
	public static Array slice(Array source, int start, int end)
	{
		Array slice = new Array ();
		for (int i = start; i < end && i < source.size(); i++)
			slice.add(source.get(i));
		return slice;
	}
	
	private static boolean loaded = false;
	
	public static void load()
	{
		if (!loaded)
		{
			loaded = true;
			
			Module module = null;
			Class current_class = null;
			Function function = null;
			
			/* system.exit */
			module = new Module("system.exit");
			module.namespace.put("EXIT_FAILURE", (double) EXIT_FAILURE);
			module.namespace.put("EXIT_SUCCESS", (double) EXIT_SUCCESS);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					exit((int) (double) caller.stack[caller.TOS-1]);
					return null;
				}
			};
			
			function.name = "exit";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			ModuleLoader.add_module(module, "system.exit");
			
			/* system.error */
			module = new Module("system.error");
			module.namespace.put("ERR_NONE", (double) ERR_NONE);
			module.namespace.put("ERR_UNKNOWN", (double) ERR_UNKNOWN);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return (double) get_err();
				}
			};
			
			function.name = "get_err";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 0;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return has_err();
				}
			};
			
			function.name = "has_err";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 0;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					set_err((int) caller.stack[caller.TOS - 1]);
					return null;
				}
			};
			
			function.name = "set_err";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					clear_err();
					return null;
				}
			};
			
			function.name = "clear_err";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 0;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					throw new Error ((String) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "error";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					Function function = (Function) caller.stack[caller.TOS-2];
					Array args = (Array) caller.stack[caller.TOS-1];
					if (function.arity != args.size())
						throw new RuntimeError ("arity mismatch in pcall (expected " + function.arity + " arguments but got " + args.size() + ".");
					for (Object arg : args) caller.push(arg);
					try {
						return function.call(caller);
					} catch (Error e) {
						return e.toString();
					}
				}
			};
			
			function.name = "pcall";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 2;
			
			module.namespace.put(function.name, function);
			
			ModuleLoader.add_module(module, "system.error");
			
			/* system.path */
			module = new Module("system.math");
			module.namespace.put("pi", Math.PI);
			module.namespace.put("e", Math.E);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return Math.abs((double) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "abs";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);

			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return Math.acos((double) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "acos";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);

			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return Math.asin((double) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "asin";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);

			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return Math.atan((double) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "atan";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);

			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return Math.atan2((double) caller.stack[caller.TOS-2], (double) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "atan2";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 2;
			
			module.namespace.put(function.name, function);

			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return Math.cos((double) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "cos";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);

			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return Math.cosh((double) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "cosh";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);

			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return Math.sin((double) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "sin";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);

			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return Math.sinh((double) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "sinh";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);

			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return Math.tan((double) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "tan";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);

			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return Math.tanh((double) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "tanh";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);

			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return Math.exp((double) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "exp";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);

			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return Math.log((double) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "log";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);

			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return Math.log10((double) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "log10";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);

			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return Math.pow((double) caller.stack[caller.TOS-2], (double) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "pow";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 2;
			
			module.namespace.put(function.name, function);

			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return Math.sqrt((double) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "sqrt";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);

			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return Math.ceil((double) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "ceil";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);

			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return Math.floor((double) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "floor";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);

			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return Math.min((double) caller.stack[caller.TOS-2], (double) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "min";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 2;
			
			module.namespace.put(function.name, function);

			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return Math.max((double) caller.stack[caller.TOS-2], (double) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "max";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 2;
			
			module.namespace.put(function.name, function);

			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return Math.floor(((double) caller.stack[caller.TOS-1]) * Math.random());
				}
			};
			
			function.name = "random";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			ModuleLoader.add_module(module, "system.math");
			
			/* system.path */
			module = new Module("system.path");
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return path_get_base((String) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "get_base";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return path_get_tail((String) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "get_tail";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return path_get_ext((String) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "get_ext";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return path_remove_ext((String) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "remove_ext";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return path_join((String) caller.stack[caller.TOS-2], (String) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "join";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 2;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					try {
						Array list = new Array ();
						Iterator<Path> stream = Files.list(Paths.get((String) caller.stack[caller.TOS-1])).iterator();
						if (stream.hasNext()) {
							for (Path path = stream.next(); stream.hasNext(); path = stream.next()) {
								list.add(path.toString());
							}
						}
						return list;
					} catch (IOException err) {
						return null;
					}
				}
			};
			
			function.name = "listdir";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					try {
						Files.createDirectory(Paths.get((String) caller.stack[caller.TOS-1]));
						return true;
					} catch (IOException e) {
						return false;
					}
				}
			};
			
			function.name = "mkdir";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					try {
						Files.delete(Paths.get((String) caller.stack[caller.TOS-1]));
						return true;
					} catch (IOException e) {
						return false;
					}
				}
			};
			
			function.name = "rmdir";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					try {
						Files.delete(Paths.get((String) caller.stack[caller.TOS-1]));
						return true;
					} catch (IOException e) {
						return false;
					}
				}
			};
			
			function.name = "unlink";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			ModuleLoader.add_module(module, "system.path");
			
			/* system.string */
			module = new Module("system.string");
			module.namespace.put("ERR_INVALID_LITERAL", (double) ERR_INVALID_LITERAL);
			module.namespace.put("CHAR_SIZE", (double) 2);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return "" + (long) (double) caller.stack[caller.TOS-1];
				}
			};
			
			function.name = "itos";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return "" + (double) caller.stack[caller.TOS-1];
				}
			};
			
			function.name = "ftos";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return String.valueOf((char) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "ctos";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					try {
						return (double) Long.parseLong((String) caller.stack[caller.TOS-1]);
					} catch (NumberFormatException e) {
						set_err(ERR_INVALID_LITERAL);
						return 0D;
					}
				}
			};
			
			function.name = "stoi";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					try {
						return Double.parseDouble((String) caller.stack[caller.TOS-1]);
					} catch (NumberFormatException e) {
						set_err(ERR_INVALID_LITERAL);
						return 0D;
					}
				}
			};
			
			function.name = "stof";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					char c = (char) caller.stack[caller.TOS-1];
					return c >= 'a' && c <= 'z';
				}
			};
			
			function.name = "islower";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					char c = (char) caller.stack[caller.TOS-1];
					return c >= 'A' && c <= 'Z';
				}
			};
			
			function.name = "isupper";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					char c = (char) caller.stack[caller.TOS-1];
					return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
				}
			};
			
			function.name = "isalpha";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					char c = (char) caller.stack[caller.TOS-1];
					return c >= '0' && c <= '9';
				}
			};
			
			function.name = "isdigit";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					char c = (char) caller.stack[caller.TOS-1];
					return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
				}
			};
			
			function.name = "isalnum";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					char c = (char) caller.stack[caller.TOS-1];
					return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || (c >= '0' && c <= '9');
				}
			};
			
			function.name = "isident";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					char c = (char) caller.stack[caller.TOS-1];
					return (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9');
				}
			};
			
			function.name = "ishex";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					char c = (char) caller.stack[caller.TOS-1];
					return ((int) c) <= 127;
				}
			};
			
			function.name = "isascii";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return Character.isSurrogate((char) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "issurrogate";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					char c = (char) caller.stack[caller.TOS-1];
					if (c >= 'A' && c <= 'Z')
						return c - 'A' + 'a';
					else
						return c;
				}
			};
			
			function.name = "tolower";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					char c = (char) caller.stack[caller.TOS-1];
					if (c >= 'a' && c <= 'z')
						return c - 'a' + 'A';
					else
						return c;
				}
			};
			
			function.name = "toupper";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return ((String) caller.stack[caller.TOS-2]).length();
				}
			};
			
			function.name = "len";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return ((String) caller.stack[caller.TOS-2]).codePointAt((int) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "index";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 2;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return ((String) caller.stack[caller.TOS-3]).substring((int) caller.stack[caller.TOS-2], (int) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "slice";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 3;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return ((String) caller.stack[caller.TOS-2]).indexOf((String) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "find";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 2;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return (String) caller.stack[caller.TOS-2] + (String) caller.stack[caller.TOS-1];
				}
			};
			
			function.name = "concat";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 2;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@SuppressWarnings("unchecked")
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
                    String sep = (String) caller.stack[caller.TOS-2];
                    shark.core.Array data = (shark.core.Array) caller.stack[caller.TOS-1];
                    StringBuilder buf = new StringBuilder ();
                    for (int i = 0; i < data.size(); i++)
                    {
                        if (i != 0) buf.append(sep);
                        buf.append((String) data.get(i));
                    }
                    return buf.toString();
				}
			};
			
			function.name = "join";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 2;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return ((String) caller.stack[caller.TOS-2]).split(String.valueOf((char) caller.stack[caller.TOS-1]));
				}
			};
			
			function.name = "split";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 2;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return format((String) caller.stack[caller.TOS-2], (Array) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "format";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 2;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return normal((String) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "normal";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return quote((String) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "quote";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			current_class = new Class("strbuf", null) {
				@Override
				public shark.core.Object create() throws RuntimeError {
					return new strbuf (this);
				}
			};
			
			module.namespace.put(current_class.name, current_class);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return null;
				}
			};
			
			function.name = "init";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 0;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					((strbuf) caller.stack[caller.TOS-2]).put((char) caller.stack[caller.TOS-1]);
					return null;
				}
			};
			
			function.name = "put";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 1;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					((strbuf) caller.stack[caller.TOS-2]).puts((String) caller.stack[caller.TOS-1]);
					return null;
				}
			};
			
			function.name = "puts";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 1;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					((strbuf) caller.stack[caller.TOS-3]).printf((String) caller.stack[caller.TOS-2], (Array) caller.stack[caller.TOS-1]);
					return null;
				}
			};
			
			function.name = "printf";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 2;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return ((strbuf) caller.stack[caller.TOS-1]).read_all();
				}
			};
			
			function.name = "read_all";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 0;
			
			current_class.methods.put(function.name, function);
			
			current_class = new Class("bytes", null) {
				@Override
				public shark.core.Object create() throws RuntimeError {
					return new bytes ();
				}
			};
			
			bytes.BYTES_CLASS = current_class;
			module.namespace.put(current_class.name, current_class);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return null;
				}
			};
			
			function.name = "init";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 0;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					((bytes) caller.stack[caller.TOS-2]).put((byte) (double) caller.stack[caller.TOS-1]);
					return null;
				}
			};
			
			function.name = "put";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 1;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					((bytes) caller.stack[caller.TOS-2]).put_short((short) (double) caller.stack[caller.TOS-1]);
					return null;
				}
			};
			
			function.name = "put_short";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 1;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					((bytes) caller.stack[caller.TOS-2]).put_int((int) (double) caller.stack[caller.TOS-1]);
					return null;
				}
			};
			
			function.name = "put_int";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 1;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					((bytes) caller.stack[caller.TOS-2]).puts((bytes) caller.stack[caller.TOS-1]);
					return null;
				}
			};
			
			function.name = "puts";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 1;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return (double) ((bytes) caller.stack[caller.TOS-1]).tell();
				}
			};
			
			function.name = "tell";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 0;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					((bytes) caller.stack[caller.TOS-3]).patch((int) (double) caller.stack[caller.TOS-2], (byte) (double) caller.stack[caller.TOS-1]);
					return null;
				}
			};
			
			function.name = "patch";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 2;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					((bytes) caller.stack[caller.TOS-3]).patch_short((int) (double) caller.stack[caller.TOS-2], (short) (double) caller.stack[caller.TOS-1]);
					return null;
				}
			};
			
			function.name = "patch_short";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 2;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					((bytes) caller.stack[caller.TOS-3]).patch_int((int) (double) caller.stack[caller.TOS-2], (int) (double) caller.stack[caller.TOS-1]);
					return null;
				}
			};
			
			function.name = "patch_int";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 2;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					((bytes) caller.stack[caller.TOS-2]).get((int) (double) caller.stack[caller.TOS-2]);
					return null;
				}
			};
			
			function.name = "get";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 1;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return encode((String) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "encode";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return decode((bytes) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "decode";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			ModuleLoader.add_module(module, "system.string");
			
			/* system.io */
			module = new Module("system.io");
			module.namespace.put("ERR_FILE_NOT_FOUND", (double) ERR_FILE_NOT_FOUND);
			
			current_class = new Class("text_file", null) {
				@Override
				public shark.core.Object create() throws RuntimeError {
					throw new RuntimeError("Can't create a text_file directly (use 'open' instead).");
				}
			};
			
			text_file.FILE_CLASS = current_class;
			module.namespace.put(current_class.name, current_class);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					((text_file) caller.stack[caller.TOS-2]).put((char) caller.stack[caller.TOS-1]);
					return null;
				}
			};
			
			function.name = "put";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 1;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					((text_file) caller.stack[caller.TOS-2]).puts((String) caller.stack[caller.TOS-1]);
					return null;
				}
			};
			
			function.name = "puts";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 1;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					((text_file) caller.stack[caller.TOS-3]).printf((String) caller.stack[caller.TOS-2], (Array) caller.stack[caller.TOS-1]);
					return null;
				}
			};
			
			function.name = "printf";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 2;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return ((text_file) caller.stack[caller.TOS-1]).fetch();
				}
			};
			
			function.name = "fetch";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 0;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return ((text_file) caller.stack[caller.TOS-2]).read((int) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "read";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 1;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return ((text_file) caller.stack[caller.TOS-1]).at_end();
				}
			};
			
			function.name = "at_end";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 0;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					((text_file) caller.stack[caller.TOS-1]).close();
					return null;
				}
			};
			
			function.name = "close";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 0;
			
			current_class.methods.put(function.name, function);
			
			current_class = new Class("binary_file", null) {
				@Override
				public shark.core.Object create() throws RuntimeError {
					throw new RuntimeError("Can't create a binary_file directly (use 'open' instead).");
				}
			};
			
			binary_file.FILE_CLASS = current_class;
			module.namespace.put(current_class.name, current_class);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					((binary_file) caller.stack[caller.TOS-2]).put((byte) (double) caller.stack[caller.TOS-1]);
					return null;
				}
			};
			
			function.name = "put";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 1;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					((binary_file) caller.stack[caller.TOS-2]).puts((bytes) caller.stack[caller.TOS-1]);
					return null;
				}
			};
			
			function.name = "puts";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 1;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return (double) (((int) ((binary_file) caller.stack[caller.TOS-1]).fetch()) & 0xFF);
				}
			};
			
			function.name = "fetch";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 0;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return ((binary_file) caller.stack[caller.TOS-2]).read((int) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "read";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 1;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return ((binary_file) caller.stack[caller.TOS-1]).at_end();
				}
			};
			
			function.name = "at_end";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 0;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					((binary_file) caller.stack[caller.TOS-1]).close();
					return null;
				}
			};
			
			function.name = "close";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 0;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return open((String) caller.stack[caller.TOS-2], (String) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "open";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 2;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					put((char) caller.stack[caller.TOS-1]);
					return null;
				}
			};
			
			function.name = "put";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					puts((String) caller.stack[caller.TOS-1]);
					return null;
				}
			};
			
			function.name = "puts";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					printf((String) caller.stack[caller.TOS-2], (Array) caller.stack[caller.TOS-1]);
					return null;
				}
			};
			
			function.name = "printf";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 2;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return read_line();
				}
			};
			
			function.name = "read_line";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 0;
			
			module.namespace.put(function.name, function);
			
			ModuleLoader.add_module(module, "system.io");
			
			/* system.io */
			module = new Module("system.util");
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return util_copy(caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "copy";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return slice((Array) caller.stack[caller.TOS-3], (int) (double) caller.stack[caller.TOS-2], (int) (double) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "slice";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 3;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					Array list = (Array) caller.stack[caller.TOS-1];
					return list.remove(list.size() - 1);
				}
			};
			
			function.name = "pop";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return ((Array) caller.stack[caller.TOS-2]).remove((int) (double) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "popindex";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 2;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return (double) ((Array) caller.stack[caller.TOS-2]).indexOf(caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "find";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 2;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					Array list = (Array) caller.stack[caller.TOS-2];
					Array other = (Array) caller.stack[caller.TOS-1];
					Array concat = new Array ();
					concat.addAll(list);
					concat.addAll(other);
					return concat;
				}
			};
			
			function.name = "concat";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 2;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					((Array) caller.stack[caller.TOS-2]).addAll((Array) caller.stack[caller.TOS-1]);
					return null;
				}
			};
			
			function.name = "extend";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 2;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return ((shark.core.Table) caller.stack[caller.TOS-2]).data.remove(caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "remove";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 2;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					((shark.core.Table) caller.stack[caller.TOS-2]).data.putAll(((shark.core.Table) caller.stack[caller.TOS-1]).data);
					return null;
				}
			};
			
			function.name = "update";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 2;
			
			module.namespace.put(function.name, function);
			
			ModuleLoader.add_module(module, "system.util");
		}
	}
}

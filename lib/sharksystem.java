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

/*
import java.io.FileReader;
import java.io.FileWriter;
import java.io.FileNotFoundException;
import java.io.IOException;

import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Scanner;
import java.util.Iterator;
*/

public static double abs(double x) {
    return Math.abs(x);
}

public static double acos(double x) {
    return Math.acos(x);
}

public static double asin(double x) {
    return Math.asin(x);
}

public static double atan(double x) {
    return Math.atan(x);
}

public static double atan2(double y, double x) {
    return Math.atan2(y, x);
}

public static long ceil(double x) {
    return (long) Math.ceil(x);
}

public static void clear_err() {
    ERR = 0;
}

public static double cos(double x) {
    return Math.cos(x);
}

public static double cosh(double x) {
    return Math.cosh(x);
}

public static long ctoi(char x) {
    return (long) x;
}

public static double e = Math.E;

public static long ERR = 0;

public static long ERR_FILE_NOT_FOUND = 3;

public static long ERR_INVALID_LITERAL = 2;

public static long ERR_NONE = 0;

public static long ERR_UNKNOWN = 1;

public static void exit(long code) {
    System.exit((int) code);
}

public static long EXIT_FAILURE = -1;

public static long EXIT_SUCCESS = 0;

public static double exp(double x) {
    return Math.exp(x);
}

public static class FILE
{
    FileReader in;
    FileWriter out;
    
    public FILE(FileReader in, FileWriter out) {
        this.in = in;
        this.out = out;
    }
    
    public void close() {
        try {
            if (in != null) in.close();
            else out.close();
        } catch (IOException e) {
            // do nothing
        }
    }
    
    public boolean at_end() {
        try {
            return !in.ready();
        } catch (IOException e) {
            return false;
        }
    }
    
    public void put(char value) {
        try {
            out.append(value);
        } catch (IOException e) {
            // do nothing
        }
    }
    
    public void write(String value) {
        try {
            out.append(value);
        } catch (IOException e) {
            // do nothing
            e.printStackTrace();
        }
    }
    
    public char fetch() {
        try {
            int c = in.read();
            if (c != -1)
                return (char) c;
            else
                return '\0';
        } catch (IOException e) {
            return '\0';
        }
    }
}

public static long floor(double x) {
    return (long) Math.floor(x);
}

public static void free(Object x) {
    // do nothing
}

public static void free_list(Object x) {
    // do nothing
}

public static void free_strbuf(Object x) {
    // do nothing
}

public static void free_striter(Object x) {
    // do nothing
}

public static void free_strmap(Object x) {
    // do nothing
}

public static String ftos(double x) {
    return Double.toString(x);
}

public static long get_err() {
    return ERR;
}

public static long idiv(long x, long y) {
    return (long) (x / y);
}

public static boolean isalnum(char x) {
    return isalpha(x) || isdigit(x);
}

public static boolean isalpha(char x) {
    return islower(x) || isupper(x);
}

public static boolean isdigit(char x) {
    return x >= '0' && x <= '9';
}

public static boolean isident(char x) {
    return isalnum(x) || x == '_';
}

public static boolean islower(char x) {
    return x >= 'a' && x <= 'z';
}

public static boolean isupper(char x) {
    return x >= 'A' && x <= 'Z';
}

public static char itoc(long x) {
    return (char) x;
}

public static String itos(long x) {
    return Long.toString(x);
}

public static class list
{
    ArrayList<Object> data;
    
    public list() {
        data = new ArrayList<Object> ();
    }
    
    public long size() {
        return data.size();
    }
    
    public void push(Object value) {
        data.add(value);
    }
    
    public Object pop() {
        return data.remove(data.size() - 1);
    }
    
    public Object get(long index) {
        return data.get((int) index);
    }
    
    public void set(long index, Object value) {
        data.set((int) index, value);
    }
}

public static list listdir(String dir) {
    try {
        list result = new list ();
        Iterator<Path> stream = Files.list(Paths.get(dir)).iterator();
        if (stream.hasNext()) {
            for (Path path = stream.next(); stream.hasNext(); path = stream.next()) {
                result.push(path.toString());
            }
        }
        return result;
    } catch (IOException err) {
        return null;
    }
}

public static double log(double x) {
    return Math.log(x);
}

public static double log10(double x) {
    return Math.log10(x);
}

public static double max(double x, double y) {
    return Math.max(x, y);
}

public static double min(double x, double y) {
    return Math.min(x, y);
}

public static boolean mkdir(String dir) {
    try {
        Files.createDirectory(Paths.get(dir));
        return true;
    } catch (IOException e) {
        return false;
    }
}

public static FILE open(String filename, char mode) {
    try {
        if (mode == 'r') {
            return new FILE (new FileReader (filename), null);
        } else {
            return new FILE (null, new FileWriter (filename));
        }
    } catch (FileNotFoundException e) {
        set_err(ERR_FILE_NOT_FOUND);
        return null;
    } catch (Exception e) {
        return null;
    }
}

public static double pi = Math.PI;

public static double pow(double x, double y) {
    return Math.pow(x, y);
}

public static long random(long x) {
    return (long) Math.floor(((double) x) * Math.random());
}

public static String read() {
    Scanner scanner = new Scanner (System.in);
    String line = scanner.nextLine();
    scanner.close();
    return line;
}

public static double real(long x) {
    return (double) x;
}

public static boolean rmdir(String dir) {
    try {
        Files.delete(Paths.get(dir));
        return true;
    } catch (IOException e) {
        return false;
    }
}

public static void set_err(long value) {
    ERR = value;
}

public static double sin(double x) {
    return Math.sin(x);
}

public static double sinh(double x) {
    return Math.sinh(x);
}

public static double sqrt(double x) {
    return Math.sqrt(x);
}

public static double stof(String x) {
    try {
        return Double.parseDouble(x);
    } catch (NumberFormatException e) {
        set_err(ERR_INVALID_LITERAL);
        return 0.0D;
    }
}

public static long stoi(String x) {
    try {
        return Long.parseLong(x);
    } catch (NumberFormatException e) {
        set_err(ERR_INVALID_LITERAL);
        return 0;
    }
}

public static class strbuf
{
    StringBuilder buffer;
    
    public strbuf() {
        buffer = new StringBuilder ();
    }
    
    public void put(char value) {
        buffer.append(value);
    }
    
    public void write(String data) {
        buffer.append(data);
    }
    
    public String get() {
        String data = buffer.toString();
        buffer = new StringBuilder ();
        return data;
    }
}

public static class striter
{
    String data;
    long pos;
    
    public striter(String data) {
        this.data = data;
        pos = 0;
    }
    
    public long tell() {
        return pos;
    }
    
    public void seek(long pos) {
        this.pos = pos;
    }
    
    public char next() {
        return (char) data.codePointAt((int) pos++);
    }
    
    public boolean empty() {
        return pos >= data.length();
    }
}

public static class strmap
{
    HashMap<String, Long> data;
    
    public strmap() {
        data = new HashMap<String, Long> ();
    }
    
    public long get(String index) {
        return (long) data.get(index);
    }
    
    public void set(String index, long value) {
        data.put(index, (Long) value);
    }
    
    public boolean has(String index) {
        return data.containsKey(index);
    }
    
    public long pop(String index) {
        return (long) data.remove(index);
    }
}

public static double tan(double x) {
    return Math.tan(x);
}

public static double tanh(double x) {
    return Math.tanh(x);
}

public static char tolower(char x) {
    return Character.toLowerCase(x);
}

public static char toupper(char x) {
    return Character.toUpperCase(x);
}

public static boolean unlink(String dir) {
    try {
        Files.delete(Paths.get(dir));
        return true;
    } catch (IOException e) {
        return false;
    }
}

public static void write(String x) {
    System.out.print(x);
}

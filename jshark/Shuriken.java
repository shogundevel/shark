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

import shark.bytecode.ModuleReader;
import shark.bytecode.VirtualMachine;
import shark.core.Class;
import shark.core.Function;
import shark.core.Module;
import shark.core.Function;
import shark.core.RuntimeError;
import shark.core.WrapperException;
import shark.lib.ModuleLoader;
import shark.lib.Standard;
import shark.lib.Standard.Exit;

import java.awt.*;
import java.awt.event.*;
import java.awt.geom.AffineTransform;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.swing.Timer;

public class Shuriken {
    public static int SIZE_X = 640;
    public static int SIZE_Y = 480;
    
    public static int SCALE_FACTOR = 1;
    
	public static Class ACTIVITY_CLASS = null;
	
	public static class Activity extends shark.core.Table
	{
		public static BufferedImage screen_image;
		public static Graphics2D screen;
		
		public Activity(Class type)
		{
			this.type = type;
		}
		
        public static void init()
        {
            screen_image = new BufferedImage(SIZE_X, SIZE_Y, BufferedImage.TYPE_INT_ARGB);
			screen = screen_image.createGraphics();
        }
        
		public void draw(Render texture, int x, int y) {
			screen.drawImage(texture.texture, x, y, null);
		}
		
		public void draw_text(String text, SharkFont font, int x, int y)
		{
			screen.setFont(font.font);
			screen.drawString(text, x, y);
		}
	}
	
    public static Activity main_activity = null;
    
	public static Class RENDER_CLASS = null;
	
	public static class Render extends shark.core.Object
	{
		int size_x;
        int size_y;
		BufferedImage texture;
        Graphics2D canvas;
		
		public Render () {
			type = RENDER_CLASS;
		}
		
		public void init(BufferedImage texture)
		{
			size_x = texture.getWidth(null);
			size_y = texture.getHeight(null);
			this.texture = texture;
		}
        
        public void draw(Render render, int x, int y)
        {
            if (canvas == null) canvas = texture.createGraphics();
            canvas.drawImage(render.texture, x, y, null);
        }
	}
	
	public static Class FONT_CLASS = null;
	
	public static class SharkFont extends shark.core.Object
	{
		int height;
		Font font;
        FontMetrics metrics;
        int ascent;
		
		public SharkFont() {
			type = FONT_CLASS;
		}
		
		public void init(String name, int height) throws Exception
		{
			this.height = height;
			font = Font.createFont(Font.TRUETYPE_FONT, new File (Standard.path_join(asset_path, name))).deriveFont((float) height);
            metrics = Activity.screen.getFontMetrics(font);
            ascent = metrics.getAscent();
		}
	}
	
	public static String asset_path = null;
	
	private static boolean loaded = false;
	
	public static void load()
	{
		if (!loaded)
		{
			loaded = true;
			
			Module module = null;
			Class current_class = null;
			Function function = null;
			
			/* shuriken.render */
			module = new Module("shuriken.render");
			
			current_class = RENDER_CLASS = new Class("render", null) {
				@Override
				public shark.core.Object create() throws RuntimeError {
					return new Render ();
				}
			};
            
			module.namespace.put(current_class.name, current_class);
            
            function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					((Render) caller.stack[caller.TOS-4]).draw((Render) caller.stack[caller.TOS-3], (int) (double) caller.stack[caller.TOS-2], (int) (double) caller.stack[caller.TOS-1]);
                    return null;
				}
			};
			
			function.name = "draw";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 3;
			
			current_class.methods.put(function.name, function);
            
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return (double) ((Render) caller.stack[caller.TOS-1]).size_x;
				}
			};
			
			function.name = "get_size_x";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 0;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return (double) ((Render) caller.stack[caller.TOS-1]).size_y;
				}
			};
			
			function.name = "get_size_y";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 0;
			
			current_class.methods.put(function.name, function);
			
            current_class = FONT_CLASS = new Class("font", null) {
				@Override
				public shark.core.Object create() throws RuntimeError {
					return new SharkFont ();
				}
			};
			
			module.namespace.put(current_class.name, current_class);
            
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return (double) ((SharkFont) caller.stack[caller.TOS-1]).height;
				}
			};
			
			function.name = "get_height";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 0;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return (double) ((SharkFont) caller.stack[caller.TOS-2]).metrics.stringWidth((String) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "get_width";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 1;
			
			current_class.methods.put(function.name, function);
			
            function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return (double) (((int) (double) caller.stack[caller.TOS-4]) << 24 | ((int) (double) caller.stack[caller.TOS-3]) << 16 | ((int) (double) caller.stack[caller.TOS-2]) << 8 | ((int) (double) caller.stack[caller.TOS-1]));
				}
			};
			
			function.name = "color";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 4;
			
			module.namespace.put(function.name, function);
            
            function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
                    Render render = new Render ();
                    render.texture = new BufferedImage((int) (double) caller.stack[caller.TOS-2], (int) (double) caller.stack[caller.TOS-1], BufferedImage.TYPE_INT_ARGB);
                    render.canvas = render.texture.createGraphics();
                    render.canvas.setColor(new Color (
                        (((int) (double) caller.stack[caller.TOS-3]) >> 24) & 0xFF,
                        (((int) (double) caller.stack[caller.TOS-3]) >> 16) & 0xFF,
                        (((int) (double) caller.stack[caller.TOS-3]) >> 8) & 0xFF,
                        (((int) (double) caller.stack[caller.TOS-3])) & 0xFF));
                    render.canvas.fillRect(0, 0, render.texture.getWidth(), render.texture.getHeight());
                    return render;
				}
			};
			
			function.name = "render_solid";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 3;
			
			module.namespace.put(function.name, function);
            
            function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
                    SharkFont font = (SharkFont) caller.stack[caller.TOS-2];
                    int width = font.metrics.stringWidth((String) caller.stack[caller.TOS-3]);
                    int height = font.height;
                    Render render = new Render ();
                    render.texture = new BufferedImage(width > 0 ? width : 1, height, BufferedImage.TYPE_INT_ARGB);
                    render.canvas = render.texture.createGraphics();
                    render.canvas.setFont(font.font);
                    render.canvas.setColor(new Color (
                        (((int) (double) caller.stack[caller.TOS-1]) >> 24) & 0xFF,
                        (((int) (double) caller.stack[caller.TOS-1]) >> 16) & 0xFF,
                        (((int) (double) caller.stack[caller.TOS-1]) >> 8) & 0xFF,
                        (((int) (double) caller.stack[caller.TOS-1])) & 0xFF));
                    render.canvas.drawString((String) caller.stack[caller.TOS-3], 0, font.ascent);
                    return render;
				}
			};
			
			function.name = "render_text";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 3;
			
			module.namespace.put(function.name, function);
            
			ModuleLoader.add_module(module, "shuriken.render");
			
			/* shark.event */
			module = new Module("shuriken.event");
			
			module.namespace.put("E_NONE", (double) 0);
			module.namespace.put("E_PRESS", (double) 1);
			module.namespace.put("E_RELEASE", (double) 2);
            module.namespace.put("E_MOVE", (double) 3);
            module.namespace.put("E_ROLL_UP", (double) 4);
            module.namespace.put("E_ROLL_DOWN", (double) 5);
			module.namespace.put("E_PRESS_UP", (double) 6);
			module.namespace.put("E_PRESS_DOWN", (double) 7);
			module.namespace.put("E_PRESS_LEFT", (double) 8);
			module.namespace.put("E_PRESS_RIGHT", (double) 9);
			module.namespace.put("E_PRESS_ENTER", (double) 10);
			module.namespace.put("E_PRESS_BACK", (double) 11);
			module.namespace.put("E_PRESS_CONTROL", (double) 12);
			module.namespace.put("E_REL_UP", (double) 13);
			module.namespace.put("E_REL_DOWN", (double) 14);
			module.namespace.put("E_REL_LEFT", (double) 15);
			module.namespace.put("E_REL_RIGHT", (double) 16);
			module.namespace.put("E_REL_ENTER", (double) 17);
			module.namespace.put("E_REL_BACK", (double) 18);
			module.namespace.put("E_REL_CONTROL", (double) 19);
			module.namespace.put("E_REL_TEXT_DOWN", (double) 20);
			module.namespace.put("E_REL_TEXT_UP", (double) 21);
			
			ModuleLoader.add_module(module, "shuriken.event");
			
			/* shark.asset */
			module = new Module("shuriken.asset");
			
			module.namespace.put("ERR_ASSET_NOT_FOUND", (double) 1000);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					try {
						File source = new File (Standard.path_join(asset_path, (String) caller.stack[caller.TOS-1]));
						BufferedImage image = ImageIO.read(source);
						Render render = new Render ();
						render.init(image);
						return render;
					} catch (IOException err) {
						Standard.set_err(1000);
						return null;
					}
				}
			};
			
			function.name = "load_image";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					SharkFont font = new SharkFont ();
                    try {
                        font.init((String) caller.stack[caller.TOS-2], (int) (double) caller.stack[caller.TOS-1]);
                        return font;
                    } catch (Exception e) {
                        Standard.set_err(1000);
                        throw new WrapperException (e);
                    }
				}
			};
			
			function.name = "load_font";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 2;
			
			module.namespace.put(function.name, function);
			
			ModuleLoader.add_module(module, "shuriken.asset");
			
            /* shark.activity */
			module = new Module("shuriken.activity");
			
			current_class = ACTIVITY_CLASS = new Class("activity", null) {
				@Override
				public shark.core.Object create() throws RuntimeError {
					return null;
				}
			};
			
			module.namespace.put(current_class.name, current_class);
            
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
                    ((Activity) caller.stack[caller.TOS-4]).draw(
                        (Render) caller.stack[caller.TOS-3],
                        (int) (double) caller.stack[caller.TOS-2],
                        (int) (double) caller.stack[caller.TOS-1]);
					return null;
				}
			};
			
			function.name = "draw";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 3;
			
			current_class.methods.put(function.name, function);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return null;
				}
			};
			
			function.name = "launch";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 0;
			
			current_class.methods.put(function.name, function);
			
            function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return null;
				}
			};
			
			function.name = "event";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 4;
			
			current_class.methods.put(function.name, function);
			
            function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return null;
				}
			};
			
			function.name = "update";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 0;
			
			current_class.methods.put(function.name, function);
			
            function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return null;
				}
			};
			
			function.name = "render";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 0;
			
			current_class.methods.put(function.name, function);
			
			ModuleLoader.add_module(module, "shuriken.activity");
		}
	}
    
    public static Object sync = new Object ();
    
    public static class Main extends Frame implements ActionListener
    {
        VirtualMachine machine;
        Activity child;
        
        public Main(VirtualMachine vm, Activity main)
        {
            super("Shuriken");
            
            machine = vm;
            child = main_activity = main;
            
            setSize(SIZE_X * SCALE_FACTOR, SIZE_Y * SCALE_FACTOR);
            setVisible(true);
            
            Timer timer = new Timer ((int) 1000 / 24, this);
            timer.start();
            
            addWindowListener(new WindowAdapter () {
                @Override
                public void windowClosing(WindowEvent event) {
                    dispose();
                    System.exit(0);
                }
            });
            
            addKeyListener(new KeyAdapter () {
                @Override
                public void keyPressed(KeyEvent event)
                {
                    switch (event.getKeyCode())
                    {
                    case KeyEvent.VK_UP:
                        dispatchKeyEvent(6, '\0');
                        break;
                    case KeyEvent.VK_DOWN:
                        dispatchKeyEvent(7, '\0');
                        break;
                    case KeyEvent.VK_LEFT:
                        dispatchKeyEvent(8, '\0');
                        break;
                    case KeyEvent.VK_RIGHT:
                        dispatchKeyEvent(9, '\0');
                        break;
                    case KeyEvent.VK_ENTER:
                        dispatchKeyEvent(10, '\0');
                        break;
                    case KeyEvent.VK_BACK_SPACE:
                        dispatchKeyEvent(11, '\0');
                        break;
                    case KeyEvent.VK_CONTROL:
                        dispatchKeyEvent(12, '\0');
                        break;
                    default:
                        if (event.getKeyCode() >= 32 &&  event.getKeyCode() < 128)
                            dispatchKeyEvent(20, (char) event.getKeyCode());
                        break;
                    }
                }
                
                @Override
                public void keyReleased(KeyEvent event)
                {
                    switch (event.getKeyCode())
                    {
                    case KeyEvent.VK_UP:
                        dispatchKeyEvent(13, '\0');
                        break;
                    case KeyEvent.VK_DOWN:
                        dispatchKeyEvent(14, '\0');
                        break;
                    case KeyEvent.VK_LEFT:
                        dispatchKeyEvent(15, '\0');
                        break;
                    case KeyEvent.VK_RIGHT:
                        dispatchKeyEvent(16, '\0');
                        break;
                    case KeyEvent.VK_ENTER:
                        dispatchKeyEvent(17, '\0');
                        break;
                    case KeyEvent.VK_BACK_SPACE:
                        dispatchKeyEvent(18, '\0');
                        break;
                    case KeyEvent.VK_CONTROL:
                        dispatchKeyEvent(19, '\0');
                        break;
                    default:
                        if (event.getKeyCode() >= 32 &&  event.getKeyCode() < 128)
                            dispatchKeyEvent(21, (char) event.getKeyCode());
                        break;
                    }
                }
            });
        }
        
        public void dispatchKeyEvent(int event, char value)
        {
            synchronized(sync)
            {
                try {
                    machine.push(child);
                    machine.push((double) event);
                    machine.push((double) 0);
                    machine.push((double) 0);
                    machine.push(value);
                    ((Function) child.type.methods.get("event")).call(machine);
                    while (machine.TOS > 0) machine.pop();
                } catch (RuntimeError e) {
                    System.err.println("A shark.core.RuntimeError happened, here are the details:");
                    e.dump_stack_trace();
                    System.err.println("And this is the root exception that was thrown:");
                    if (e instanceof WrapperException) {
                        ((WrapperException) e).error.printStackTrace();
                    } else {
                        e.printStackTrace();
                    }
                    System.exit(-1);
                }
            }
        }
        
        @Override
        public void actionPerformed(ActionEvent event)
        {
            synchronized(sync)
            {
                try {
                    machine.push(child);
                    ((Function) child.type.methods.get("update")).call(machine);
                    machine.push(child);
                    ((Function) child.type.methods.get("render")).call(machine);
                } catch (RuntimeError e) {
                    System.err.println("A shark.core.RuntimeError happened, here are the details:");
                    e.dump_stack_trace();
                    System.err.println("And this is the root exception that was thrown:");
                    if (e instanceof WrapperException) {
                        ((WrapperException) e).error.printStackTrace();
                    } else {
                        e.printStackTrace();
                    }
                    System.exit(-1);
                }
            }
            repaint();
        }
        
        public void update(Graphics g)
        {
            paint(g);
        }
        
        public void paint(Graphics g)
        {
            ((Graphics2D) g).scale(SCALE_FACTOR, SCALE_FACTOR);
            ((Graphics2D) g).drawImage(child.screen_image, 0, 0, this);
        }
    }
    
    public static void main(String[] args)
	{
		if (args.length >= 1) {
			Standard.load();
            Shuriken.load();
			VirtualMachine vm = new VirtualMachine ();
			try {
                asset_path = Standard.path_get_base(args[0]);
				ModuleLoader.add_import_path(Standard.path_get_base(args[0]));
                Activity.init();
				Module main_module = new ModuleReader(new FileInputStream(args[0]), Standard.path_get_tail(args[0])).read().execute(vm);
				Object main = main_module.namespace.get("main_activity");
				if (!(main instanceof Class)) {
                    System.err.println("Expected 'main_activity' to be a class.");
                    return;
                } else {
                    Activity main_activity = new Activity ((Class) main);
                    vm.push(main_activity);
                    vm.push((double) SIZE_X * SCALE_FACTOR);
                    vm.push((double) SIZE_Y * SCALE_FACTOR);
                    ((Function) main_activity.type.methods.get("launch")).call(vm);
                    new Main (vm, main_activity);
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
			System.out.println("Usage: shuriken <filename> [ <size_x> <size_y> [ <scale> ]]");
		}
	}
}

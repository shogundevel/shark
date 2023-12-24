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

public class SharkGame {
    public static int SIZE_X = 320;
    public static int SIZE_Y = 192;
    
    public static int SCALE_FACTOR = 3;
    
	public static Class ACTIVITY_CLASS = null;
	
	public static class Activity extends shark.core.Table
	{
		BufferedImage screen_image;
		Graphics2D screen;
		
		public Activity(Class type)
		{
			this.type = type;
			screen_image = new BufferedImage(SIZE_X, SIZE_Y, BufferedImage.TYPE_INT_ARGB);
			screen = screen_image.createGraphics();
		}
		
		public void draw(Texture texture, int x, int y) {
			screen.drawImage(texture.texture, x, y, null);
		}
		
        public void draw_ex(Texture texture, int x, int y, int origin_x, int origin_y, float rotation, float scale_x, float scale_y)
		{
			AffineTransform transform = screen.getTransform();
			screen.translate(x, y);
			screen.rotate(-Math.toRadians(rotation), origin_x, origin_y);
			screen.scale(scale_x, scale_y);
			screen.drawImage(texture.texture, -texture.size_x / 2, -texture.size_y / 2, null);
			screen.setTransform(transform);
		}
        
		public void draw_text(String text, SharkFont font, int x, int y)
		{
			screen.setFont(font.font);
            screen.setColor(font.color);
			screen.drawString(text, x, y + screen.getFontMetrics(font.font).getAscent());
		}
	}
	
	public static Class TEXTURE_CLASS = null;
	
	public static class Texture extends shark.core.Object
	{
		int size_x;
        int size_y;
		Image texture;
		
		public Texture () {
			type = TEXTURE_CLASS;
		}
		
		public void init(Image texture)
		{
			size_x = texture.getWidth(null);
			size_y = texture.getHeight(null);
			this.texture = texture;
		}
	}
	
	public static Class FONT_CLASS = null;
	
	public static class SharkFont extends shark.core.Object
	{
		int height;
		Font font;
        Color color;
		
		public SharkFont() {
			type = FONT_CLASS;
		}
		
		public void init(String name, int height, int color_code) throws Exception
		{
			this.height = height;
			font = Font.createFont(Font.TRUETYPE_FONT, new File (Standard.path_join(asset_path, name))).deriveFont((float) height);
            color = new Color ((color_code >> 24) & 0xFF,
                        (color_code >> 16) & 0xFF,
                        (color_code >> 8) & 0xFF,
                        (color_code) & 0xFF);
		}
	}
	
	public static String asset_path = null;
	public static String save_path = null;
	
	private static boolean loaded = false;
	
	public static void load()
	{
		if (!loaded)
		{
			loaded = true;
			
			Module module = null;
			Class current_class = null;
			Function function = null;
			
			/* shark.texture */
			module = new Module("shark.texture");
			
			current_class = TEXTURE_CLASS = new Class("texture", null) {
				@Override
				public shark.core.Object create() throws RuntimeError {
					return new Texture ();
				}
			};
            
			module.namespace.put(current_class.name, current_class);
            
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return (double) ((Texture) caller.stack[caller.TOS-1]).size_x;
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
					return (double) ((Texture) caller.stack[caller.TOS-1]).size_y;
				}
			};
			
			function.name = "get_size_y";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 0;
			
			current_class.methods.put(function.name, function);
			
			ModuleLoader.add_module(module, "shark.texture");
			
			/* shark.text */
			module = new Module("shark.text");
			
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
					return (double) 0;
				}
			};
			
			function.name = "get_width";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 1;
			
			current_class.methods.put(function.name, function);
			
			ModuleLoader.add_module(module, "shark.text");
			
            /* shark.persistent */
            module = new Module("shark.persistent");
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					return Standard.open(save_path, (String) caller.stack[caller.TOS-1]);
				}
			};
			
			function.name = "get_save_file";
			function.is_method = false;
			function.supermethod = null;
			function.owner_class = null;
			function.owner_module = module;
			function.arity = 1;
			
			module.namespace.put(function.name, function);
			
			ModuleLoader.add_module(module, "shark.persistent");
            
			/* shark.event */
			module = new Module("shark.event");
			
			module.namespace.put("E_NONE", (double) 0);
			module.namespace.put("E_PRESS", (double) 1);
			module.namespace.put("E_MOVE", (double) 2);
			module.namespace.put("E_RELEASE", (double) 3);
			module.namespace.put("E_PRESS_UP", (double) 4);
			module.namespace.put("E_PRESS_DOWN", (double) 5);
			module.namespace.put("E_PRESS_LEFT", (double) 6);
			module.namespace.put("E_PRESS_RIGHT", (double) 7);
			module.namespace.put("E_PRESS_X", (double) 8);
			module.namespace.put("E_PRESS_Y", (double) 9);
			module.namespace.put("E_REL_UP", (double) 10);
			module.namespace.put("E_REL_DOWN", (double) 11);
			module.namespace.put("E_REL_LEFT", (double) 12);
			module.namespace.put("E_REL_RIGHT", (double) 13);
			module.namespace.put("E_REL_X", (double) 14);
			module.namespace.put("E_REL_Y", (double) 15);
			
			ModuleLoader.add_module(module, "shark.event");
			
			/* shark.asset */
			module = new Module("shark.asset");
			
			module.namespace.put("ERR_ASSET_NOT_FOUND", (double) 1000);
			
			function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
					try {
						File source = new File (Standard.path_join(asset_path, (String) caller.stack[caller.TOS-1]));
						Image image = ImageIO.read(source);
						Texture texture = new Texture ();
						texture.init(image);
						return texture;
					} catch (IOException err) {
						Standard.set_err(1000);
						return null;
					}
				}
			};
			
			function.name = "load_texture";
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
                        font.init((String) caller.stack[caller.TOS-3], (int) (double) caller.stack[caller.TOS-2], (int) (double) caller.stack[caller.TOS-1]);
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
			function.arity = 3;
			
			module.namespace.put(function.name, function);
			
			ModuleLoader.add_module(module, "shark.asset");
			
            /* shark.activity */
			module = new Module("shark.activity");
			
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
                        (Texture) caller.stack[caller.TOS-3],
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
                    ((Activity) caller.stack[caller.TOS-9]).draw_ex(
                        (Texture) caller.stack[caller.TOS-8],
                        (int) (double) caller.stack[caller.TOS-7],
                        (int) (double) caller.stack[caller.TOS-6],
                        (int) (double) caller.stack[caller.TOS-5],
                        (int) (double) caller.stack[caller.TOS-4],
                        (float) (double) caller.stack[caller.TOS-3],
                        (float) (double) caller.stack[caller.TOS-2],
                        (float) (double) caller.stack[caller.TOS-1]);
					return null;
				}
			};
			
			function.name = "draw_ex";
			function.is_method = true;
			function.supermethod = null;
			function.owner_class = current_class;
			function.owner_module = module;
			function.arity = 8;
			
			current_class.methods.put(function.name, function);
            
            function = new Function () {
				@Override
				public Object call(VirtualMachine caller) throws RuntimeError {
                    ((Activity) caller.stack[caller.TOS-5]).draw_text(
                        (String) caller.stack[caller.TOS-4],
                        (SharkFont) caller.stack[caller.TOS-3],
                        (int) (double) caller.stack[caller.TOS-2],
                        (int) (double) caller.stack[caller.TOS-1]);
					return null;
				}
			};
			
			function.name = "draw_text";
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
			function.arity = 3;
			
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
			
			ModuleLoader.add_module(module, "shark.activity");
		}
	}
    
    public static Object sync = new Object ();
    
    public static class Main extends Frame implements ActionListener
    {
        VirtualMachine machine;
        Activity child;
        
        public Main(VirtualMachine vm, Activity main)
        {
            super("SharkGame");
            
            machine = vm;
            child = main;
            
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
                        dispatchKeyEvent(4);
                        break;
                    case KeyEvent.VK_DOWN:
                        dispatchKeyEvent(5);
                        break;
                    case KeyEvent.VK_LEFT:
                        dispatchKeyEvent(6);
                        break;
                    case KeyEvent.VK_RIGHT:
                        dispatchKeyEvent(7);
                        break;
                    case 'z':
                    case 'Z':
                        dispatchKeyEvent(8);
                        break;
                    case 'x':
                    case 'X':
                        dispatchKeyEvent(9);
                        break;
                    }
                }
                
                @Override
                public void keyReleased(KeyEvent event)
                {
                    switch (event.getKeyCode())
                    {
                    case KeyEvent.VK_UP:
                        dispatchKeyEvent(10);
                        break;
                    case KeyEvent.VK_DOWN:
                        dispatchKeyEvent(11);
                        break;
                    case KeyEvent.VK_LEFT:
                        dispatchKeyEvent(12);
                        break;
                    case KeyEvent.VK_RIGHT:
                        dispatchKeyEvent(13);
                        break;
                    case 'z':
                    case 'Z':
                        dispatchKeyEvent(14);
                        break;
                    case 'x':
                    case 'X':
                        dispatchKeyEvent(15);
                        break;
                    }
                }
            });
        }
        
        public void dispatchKeyEvent(int event)
        {
            synchronized(sync)
            {
                try {
                    machine.push(child);
                    machine.push((double) event);
                    machine.push((double) 0);
                    machine.push((double) 0);
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
		if (args.length == 1) {
			Standard.load();
            SharkGame.load();
			VirtualMachine vm = new VirtualMachine ();
			try {
                asset_path = Standard.path_join(args[0], "asset");
                save_path = Standard.path_join(args[0], "save");
				ModuleLoader.add_import_path(Standard.path_join(args[0], "bin"));
				Module main_module = new ModuleReader(new FileInputStream(Standard.path_join(args[0], "bin/game.shar")), "game.shar").read().execute(vm);
				Object main = main_module.namespace.get("main_activity");
				if (!(main instanceof Class)) {
                    System.err.println("Expected 'main_activity' to be a class.");
                    return;
                } else {
                    Activity main_activity = new Activity ((Class) main);
                    vm.push(main_activity);
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
			System.out.println("Usage: sharkgame <path/to/game>");
		}
	}
}

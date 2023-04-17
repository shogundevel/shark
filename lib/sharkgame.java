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
import java.awt.*;
import java.awt.event.*;
import java.awt.geom.AffineTransform;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.swing.Timer;
*/

public static int SIZE_X = 320;
public static int SIZE_Y = 192;

public static int SCALE_FACTOR = 3;

public static long ERR_ASSET_NOT_FOUND = 1000;

public static long E_NONE = 0;
public static long E_PRESS = 1;
public static long E_MOVE = 2;
public static long E_RELEASE = 3;
public static long E_PRESS_UP = 4;
public static long E_PRESS_DOWN = 5;
public static long E_PRESS_LEFT = 6;
public static long E_PRESS_RIGHT = 7;
public static long E_PRESS_X = 8;
public static long E_PRESS_Y = 9;
public static long E_REL_UP = 10;
public static long E_REL_DOWN = 11;
public static long E_REL_LEFT = 12;
public static long E_REL_RIGHT = 13;
public static long E_REL_X = 14;
public static long E_REL_Y = 15;

public static BufferedImage screen_image = null;
public static Graphics2D screen = null;

public static String asset_path = "asset/";
public static String save_path = "save";

public static Object sync = new Object ();

public static class MainFrame extends Frame implements ActionListener
{
    public MainFrame()
    {
        super("SharkGame");
        
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
            public void keyPressed(KeyEvent ev)
            {
                switch (ev.getKeyCode())
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
            public void keyReleased(KeyEvent ev)
            {
                switch (ev.getKeyCode())
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
    
    public void dispatchKeyEvent(int evcode)
    {
        synchronized(sync) {
            event(evcode, 0, 0);
        }
    }
    
    @Override
    public void actionPerformed(ActionEvent event)
    {
        synchronized(sync)
        {
            __update_game();
            render();
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
        ((Graphics2D) g).drawImage(screen_image, 0, 0, this);
    }
}

public static void __update_game() {
    update();
}

public static class texture
{
    long size_x;
    long size_y;
    Image text;
    
    public texture(Image text)
    {
        size_x = text.getWidth(null);
        size_y = text.getHeight(null);
        this.text = text;
    }
    
    public long get_size_x() {
        return size_x;
    }
    
    public long get_size_y() {
        return size_y;
    }
}

public static class font
{
    long height;
    Font font_object;
    
    public font(String name, int height) throws Exception
    {
        this.height = height;
        this.font_object = Font.createFont(Font.TRUETYPE_FONT,
            new File (asset_path + name)).deriveFont((float) height);
    }
    
    public long get_height() {
        return height;
    }
}

public static texture load_texture(String name)
{
    try {
        return new texture (ImageIO.read(new File (asset_path + name)));
    } catch (IOException err) {
        return null;
    }
}

public static font load_font(String name, long size, long color)
{
    try {
        return new font (name, (int) size);
    } catch (Exception e) {
        return null;
    }
}

public static FILE get_save_file(char mode)
{
    return open(save_path, mode);
}

public static void draw(texture text, long x, long y)
{
    screen.drawImage(text.text, (int) x, (int) y, null);
}

public static void draw_ex(texture text, long x, long y,
    long origin_x, long origin_y, double rotation,
    double scale_x, double scale_y)
{
    AffineTransform transform = screen.getTransform();
    screen.translate(x, y);
    screen.rotate(-Math.toRadians(rotation), origin_x, origin_y);
    screen.scale(scale_x, scale_y);
    screen.drawImage(text.text, (int) -text.size_x / 2, (int) -text.size_y / 2, null);
    screen.setTransform(transform);
}

public static void draw_text(String text, font font_object, long x, long y)
{
    screen.setFont(font_object.font_object);
    screen.drawString(text, (int) x, (int) y);
}

public static void main(String[] args)
{
    launch();
    
    screen_image = new BufferedImage(320, 192, BufferedImage.TYPE_INT_ARGB);
    screen = screen_image.createGraphics();
    
    new MainFrame ();
}

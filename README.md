# The Shark Programming Language

Welcome to the Shark programming language! Shark is a language designed to provide the speed and size of Lua, while also offering the familiar syntax of Python and the ability to run in the browser.

Overall, Shark is a very versatile language that can be used to build web/desktop/mobile applications and games with a single codebase, command line tools and anything you can imagine using pre existing libraries and systems available to python, lua, javascript, java and native code. This includes embedding the tiny shark VM in any java or native project with ease.

## Features

* **Fast and lightweight** native virtual machine written in C
* Small package size (50 KB) with minimal dependencies
* Can be used in **the browser** and compiled to portable ECMAScript code
* Includes a tiny and portable 2D game framework for small scale game development
* Backwards compatible with Python and Lua libraries and environments by means of targeting them as compilation output
* Official Java Virtual Machine implementation of both the VM and 2D game framework

## Simple Example

This example iterates the command line arguments and displays a greeting for each name it contains:

```[shell]
import system.io: printf

function main(args)
    for name in args do
        printf("Hello, %!", [name])
```

## Getting Started

To get started with Shark, read the Shark Handbook in this repo to learn the basics of Shark programming.

## Installing And Using The Shark SDK

This repository contains the Shark SDK binaries and the full source code for these binaries. To start using Shark, download the repository as a ZIP file and extract it to your preferred directory (the preferred location for the shark SDK on windows is C:\shark\).

Optionally, configure your PATH variable to allow the command line find the shark toolchain. Make sure it points to the root directory of the unzipped shark distribution.

Now open a command prompt, type 'shark' and hit enter, you should see something like:
Usage: C:\shark\bin\shark.exe <filename> <args>

## Using sharkemu To Run Shark Code

sharkemu is a tool introduced in the shark SDK version 1.1.0 to allow for quickly running a shark codebase with a single command.

To enter an interactive session using sharkemu type the following:
> shark tool play

To run sharkemu on a file just type:
> shark tool run example.shk

However, notice that sharkemu is an emulator written in shark, what makes it munch slower and more memory and CPU consuming than compiled code.
Also, the booting time for a program is bigger because sharkemu haves to compile the full source code of an application before running it.

## Compiling And Linking

Let's say you want to compile a file called hello.shk (stored elsewhere on the filesystem). First cd to the directory containing the file then type the following:

> shark tool compile c hello.shk hello.obj

The first three words 'shark tool compile' are used to call the shark compiler. The next, 'c' tells the compiler to generate C bytecode. The next is the path to the source code you want to compile and the last is the path where the object code will be stored.
This command should generate an object file called 'hello.obj'.

To link your newly created object file into a shark executable type the following:

> shark tool link c hello.obj hello hello.shar

The first three words 'shark tool link' calls the shark linker. The next, 'c' tells the linker that we're generating a C bytecode executable. The next is the path to the object file to be linked 'hello.obj'. Next is the name of the module that will be run on program start and that contains the 'main' function 'hello' and last is the name of the output shark executable 'hello.shar'. The '.shar' file extension means "SHark ARchive" and is used by shark C bytecode executables and library files.

If you followed all the steps just type 'shark hello.shar' to execute the produced shark executable.

Try tipying 'shark tool compile' and 'shark tool link' without arguments to see a help message and learn how to use those tools further.

## One-Step Building

A shortcut to generate a shark archive directly from a shark source file is 'shark tool build'. You could have typed the following to create the final executable:

> shark tool build hello.shk hello.shar

This is munch shortest and easier to use, but is only available for the C bytecode target.

Also try tipying 'shark tool build' without arguments to learn more about this tool.

## Using sharkenv And sharkmake

sharkenv and sharkmake were introduced in SDK version 1.2.0, they allow you to write portable build scripts that can be executed in any shark environment.

sharkenv is a command line interface written in pure shark, commands are evaluated as entered by the user from the command line.

To enter sharkenv type '> shark tool' without arguments.

The following commands are provided by the shark SDK:
* compile
* link
* run
* play
* make
* zip
* mkdir
* rmdir
* unlink
* echo
* exit

The 'echo' command echoes the rest of the line to console output.
The 'play' command enters sharkemu in eval mode.

Run the other commands without arguments to learn more about each one.

sharkmake is the make equivalent of shark. It executes a sequence of commands stored in a file called 'make' in the current directory.

A 'make' file should contain a command on each line. Empty lines and lines starting with '#' are ignored. Lines starting with ':' are tag lines used to split a large sequence of commands into groups that can be called independently.

An example of 'make' file:
```[shell]
# A hello world make file.

:hello
echo Hello, world!
```

To run sharkmake cd to the directory containing the make file and type '> shark tool make'.

You can request sharkmake to run one or more particular tags of a make file by passing their names as arguments. If no tag is specified the whole make file is executed from start to end.

## Gameshark Howto

To compile and run a gameshark project cd to the project's root directory and type the following commands:

> shark tool build src/main.shk bin/game.shar

> gameshark ./

The first command builds the game binary and the second executes it.

## Python Howto

To start cross compiling to python change the 'c' argument of the compiler to 'py'. For instance to compile our 'hello.shk' to python do:

> shark tool compile py hello.shk out.py

Now the result must be linked against the shark runtime (sharkrt.py located in the /rt directory of the shark SDK) to run properly. There are two ways to do this.

You can link the result dynamically against sharkrt.py by tipying:

> shark tool link py out.py hello hello.py

The resulting python file depends on 'sharkrt.py' to run so make sure the file is available to python in some way.

Second, you can link statically, which will embed the shark runtime into the final executable and make it dependency free. For this use the 'pyo' target of the linker:

> shark tool link pyo out.py hello hello.py

Whatever method you choose run the generated 'hello.py' with:

> python hello.py

When compiling to python you can directly import any python library from shark using the 'import' declaration, like if it where a regular shark module.

But this may not be enought for writting portable code. If you want to use the shark standard library from python you must compile it from source and link it against your python executable.

To compile the shark standard library from source cd to the shark SDK root directory and type the following command:> shark tool compile py lib/libpythonsystem/system.shk out/system.py

To link your program against the system library copy the generated out/system.py to the desired location then run the following:

> shark tool link py my-python-object.py main my-python-program.py system.py

To run a GS based game in python you will need to install the pygame library, compile the pygameshark library from source and link it 'correctly' against your game code.

First install pygame:

> pip install pygame

Next cd to the shark SDK root directory and type...

> shark tool compile py lib/libpythonsystem/system.shk out/system.py

...to build the system library and...

> shark tool compile py lib/libpygameshark/shark.shk out/shark.py

...to build the gameshark library.

Now cd to your game's root directory and type:

> shark tool compile py src/main.shk bin/out.py

Now link the created libraries to your game code passing 'shark' (and not 'main') as the main module, like this:

> shark tool link pyo bin/out.py shark bin/game.py path/to/system.py path/to/shark.py

## JavaScript Howto

To start cross compiling to javascript change the 'c' argument of the compiler to 'js'. For instance to compile our 'hello.shk' to javascript do:

> shark tool compile js hello.shk out.js

Now the result must be linked against the shark runtime (sharkrt.js located in the /rt directory of the shark SDK) to run properly. There are two ways to do this.
If you're creating a web page you can use a script tag to make the required 'sharkrt.js' available to your program. This requires no linking.
Otherwise you can link to it statically, which will embed the shark runtime into the final executable. For this use the 'js' target of the linker:

> shark tool link js out.js hello hello.js

When compiling to javascript you can make any global javascript variable visible from shark by declaring it in your source file as an empty variable. For instance, to access the browser's console object you can write:

var console

console::log("Hi.")

To run a GS based game in HTML5 you will need to compile the htmlgameshark library from source and link it 'correctly' against your game code.

First cd to the shark SDK root directory and type the following to build the gameshark library:

> shark tool compile js lib/libhtmlgameshark/shark.shk out/shark.js

Now cd to your game's root directory and type:

> shark tool compile js src/main.shk bin/out.js

Now link the created library to your game code passing 'shark' (and not 'main') as the main module and naming the result 'game.js', like this:

> shark tool link js bin/out.js shark bin/game.js path/to/shark.js

Last, copy the file 'template/index.html' from the shark SDK directory into your game's root directory. Launch the result in a browser to test it.

## Lua Howto

To start cross compiling to lua change the 'c' argument of the compiler to 'lua'. For instance to compile our 'hello.shk' to lua do:

> shark tool compile lua hello.shk out.lua

Now the result must be linked against the shark runtime (sharkrt.lua located in the /rt directory of the shark SDK) to run properly. There are two ways to do this.
You can link the result dynamically against sharkrt.lua by tipying:

> shark tool link lua out.lua hello hello.lua

The resulting lua file depends on 'sharkrt.lua' to run so make sure the file is available to lua in some way.
Second, you can link statically, which will embed the shark runtime into the final executable and make it dependency free. For this use the 'luao' target of the linker:

> shark tool link luao out.lua hello hello.lua

Whatever method you choose run the generated 'hello.lua' with:

> lua hello.lua

When compiling to lua you can directly import any lua library from shark using the 'import' declaration, like if it where a regular shark module. You can also make any global lua variable visible from shark by declaring it in your source file as an empty variable. For instance, to access lua's print function you can write:

var print

print("Hi.")

But this may not be enought for writting portable code. If you want to use the shark standard library from lua you must compile it from source and link it against your lua executable.

To compile the shark standard library from source cd to the shark SDK root directory and type the following command:

> shark tool compile lua lib/libluasystem/system.shk out/system.lua

To link your program against the system library copy the generated out/system.lua to the desired location then run the following:

> shark tool link lua my-lua-object.lua main my-lua-program.lua system.lua

## Ren'Py Howto

To port a GS game to Ren'Py start by launching renpy and creating a new empty project, then replace the generated 'game/script.rpy' with the file 'template/script.rpy' found in the shark SDK directory.

Next compile the shark system library and the 'renpygameshark' library using the 'rpy' target of the compiler:

> shark tool compile rpy lib/libpythonsystem/system.shk out/system.rpy

> shark tool compile rpy lib/librenpygameshark/shark.shk out/shark.rpy

Copy those files to your game's directory and you've got a basic renpy environment to run your game code.

Now, to compile your game to renpy source code cd to your game's root directory and type:

> shark tool compile rpy src/main.shk bin/game.rpy

Now copy the generated bin/game.rpy along with your game's /asset folder to your renpy game's directoy, and that's it. Launch your game to test it.

## Next Steps

Now you know basically everything needed to make full use of shark. If you need further help with Shark or want to start contributing, file an issue here on GH with your questions or suggestions.

Cheers, <br>
\- ShogunDevel

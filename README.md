# The Shark Programming Language

Welcome to the Shark programming language!

Shark is a dynamically typed application programming language designed to be highly portable, efficent and easy to use.

Some key features:
* **Fast and lightweight** native virtual machine written in C
* Can be used in **the browser** and compiled to portable ECMAScript code
* Includes a tiny and portable 2D game framework for small scale game development
* Backwards compatible with Python and Lua libraries and environments by means of targeting them as compilation output
* Official Java Virtual Machine implementation of both the VM and 2D game framework

This release is the last minor version of the 2.x branch, it includes many speed and stability improvements and removes unnecesary features from the code base. Shark development is on hiatus and little will be added until version 3.0. Cheers.

## Simple Example

This example iterates the command line arguments and displays a greeting for each name it contains:

```[javascript]
import system.io: printf

function main(args)
    for name in args do
        printf("Hello, %!", [name])
```

## Getting Started

To get started with Shark read The Shark Handbook in this repository.

## Installing And Using The Shark SDK

This repository contains the Shark SDK binaries and the full source code for these binaries. To start using Shark, download the repository as a ZIP file and extract it to your preferred directory (the preferred location for the shark SDK on windows is C:\shark\).

Optionally, configure your PATH variable to allow the command line find the shark toolchain. Make sure it points to the root directory of the unzipped shark distribution.

Now open a command prompt, type 'shark' and hit enter, you should see something like:
Usage: C:\shark\bin\shark.exe [filename] [args]

## Using sharkedit To Edit And Test Your Code

SharkEdit is a Shuriken based editor that comes pre-installed with the Shark SDK since version 2.1.0

SharkEdit is a baby program (written from scratch in under 0.7K lines of code) so use it with love.

To start creating with SharkEdit create a new folder in your filesystem, go into it and type the following:
> shuriken edit my-program.shk

You can customize the editor resolution to your preference by passing [ -r x_size y_size ] as an argument:
> shuriken edit -r 1000 640 my-program.shk

## Compiling And Linking Shark Code

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
* unzip
* mkdir
* rmdir
* unlink
* echo
* exit
* system

The 'echo' command echoes the rest of the line to console output.
The 'play' command enters sharkemu in eval mode.
The 'system' command executes a command in the host system command line interface.

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

## SharkGame Howto

To compile and run a SharkGame project cd to the project's root directory and type the following commands:

> shark tool build src/main.shk bin/game.shar

> sharkgame ./

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

To run a GS based game in HTML5 you will need to compile the htmlsharkgame library from source and link it 'correctly' against your game code.

First cd to the shark SDK root directory and type the following to build the sharkgame library:

> shark tool compile js lib/libhtmlsharkgame/shark.shk out/shark.js

Now cd to your game's root directory and type:

> shark tool compile js src/main.shk bin/out.js

Now link the created library to your game code passing 'shark' (and not 'main') as the main module and naming the result 'game.js', like this:

> shark tool link js bin/out.js shark bin/game.js path/to/shark.js

Last, copy the file 'lib/index.html' from the shark SDK directory into your game's root directory. Launch the result in a browser to test it.

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

## Implementation Details

* The CShark VM uses reference counting to manage memory and does not collect cyclic references.
* The standard library is not fully implemented in JavaScript due to it having no standard I/O, thus this is the only platform that can't bootstrap the shark compilers out of the box. This does not means the shark compilers can't run in javascript, it is possible to use the compilers even in the browser by implementing a fake I/O stream using strings.
* The bitwise operators (~ | & ^ <~ ~>) are not implemented in lua. Using them will not show a compilation error, but a runtime error when running the resulting lua code.

## Next Steps

Now you know basically everything needed to make full use of shark. If you need further help or want to start contributing, file an issue here on github with your questions or suggestions.

Cheers, <br>
\- ShogunDevel

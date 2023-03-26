# Shark Programming Language

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

```[code]
import system.io: printf

function main(args)
    for name in args do
        printf("Hello, %!", [name])
```

## Getting Started

To get started with Shark, read the Shark Handbook in this repo to learn the basics of Shark programming. Then, read the **guide** file for instructions on using the Shark toolchain to start writing Shark code.

This version of Shark is considered stable (green), and major changes will only be made through the Shark Community Process (SCP) proposals.

This repository contains the Shark SDK binaries and the full source code for these binaries. To start using Shark, download the repository as a ZIP file and extract it to your preferred directory.

If you need further help with Shark or want to start contributing, file an issue here on GH with your questions or suggestions.

Cheers, <br>
\- ShogunDevel

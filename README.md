# CHIP-8 Emulator

## Overview

This project aims to write a small and functional CHIP-8 emulator (or more accurately an interpreter, since CHIP-8 wasn't ever real hardware) in C using SDL to render the screen.

Right now a lot of the basic instructions are implemented, but it's not quite ready yet.

Note: the ```ibm.ch8``` ROM file is sourced from [this git repo](https://github.com/loktar00/chip8), and is included here temporarily for testing purposes.

## Origin

I am interested in learning more about computers, and emulation seemed like an interesting way to do so. I chose CHIP-8 because it would provide one of the simplest ways to get into this branch of computing, and followed the following guides to build this:

- A tutorial to [build a hypothetical VM](https://felix.engineer/blogs/virtual-machine-in-c), which was extremely simple as it only had a couple registers, instructions etc. While I ended up using very little of this tutorial's code, it did influence me in the initial commits. I actually built part of this tutorial and used it as the foundation before eventually changing the code drastically.

- This [excellent writeup](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/) of CHIP-8 gives a lot of information regarding how to build the emulator, and explains the major opcodes as well without any code.

## Requirements

Make sure you have the following in order to both build and run the emulator on your system:

```
gcc, make, SDL2
```

These packages vary by Linux distro, so here are the common ones:

- Fedora:

```
SDL2 SDL2-devel
```

- Ubuntu:

```
libsdl2 libsdl2-dev
```

## Installation

Clone this repo, ```cd``` into it and run ```make```. You'll get a ```vm``` executable which you can execute.

## Usage

To run a specific ROM, run

```bash
./vm /path/to/ROM/file.ch8
```

## Status

The emulator runs the test IBM ROM as of now, and the groundwork for most of the other instructions has already been laid.

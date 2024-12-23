# Engine2D - A 2D C++ Game Engine

Engine2D is a game engine written in C++ to develop 2D games. The engine is **heavily** inspired by the [Unity Scripting API](https://docs.unity3d.com/6000.0/Documentation/ScriptReference/) and is still a huge **WIP**. It uses [SDL2](https://github.com/libsdl-org/SDL) to support cross-platform graphics and hardware and [Box2D](https://github.com/erincatto/box2d) (v2.4) as the physics engine.

# Prerequisites
The dependency libraries are included in git submodules and will be built along with the project if you choose.
- C++17 Supported Compiler - https://en.cppreference.com/w/cpp/compiler_support/17
- CMake - https://cmake.org/
- SDL2 - https://github.com/libsdl-org/SDL
- SDL2_image - https://github.com/libsdl-org/SDL_image
- Box2D (v2.4) - https://github.com/erincatto/box2d
> Note: Box2D v3.0 is a complete rewrite of Box2D and is incompatible with Engine2D. For now, there are no plans to migrate to v3.0 until it improves in certain aspects I feel are lacking.

# Building
There are two ways to build the project. If you do not have any of the above prerequisite libraries installed to your standard installation paths, you can do the following:
## Building With Dependencies
- Clone the repo:
```
git clone --recurse-submodules -j4 https://github.com/notvalproate/Engine2D.git
```
- Build the library:
```
cmake -B build
cmake --build build -j4
```
The built binaries and archives will be written to `/build/bin`
> Note: Building and configuring might take some time since 3 dependencies have to be built. You can speed up the process of building by appropriately adjusting the `-j` flag and adding the number of CPU cores you want to use to build (For example on my i7-8700 I use `-j10`)

## Building With Pre-Installed Dependencies
If you have the dependencies pre-installed to your platform's standard installation paths, you can follow the steps below to avoid re-building the dependencies:
- Clone the repo:
```
git clone https://github.com/notvalproate/Engine2D.git
```
- Configure the project
```
cmake -B build
```
Then, edit CMakeCache.txt and set the `ENGINE2D_BUILD_DEPENDENCIES` option to `OFF`. If using CMake GUI, un-tick the option. Then:
```
cmake --build build -j4
```
The built binaries and archives will be written to `/build/bin` folder, and the installed dynamic libraries (.dll, .so, .dylib) of the dependencies will be copied over to the folder as well if found. If not found you may have to copy them manually.

# Running An Example
To run examples, you must first build them as well. To do that, edit CMakeCache.txt and set the `ENGINE2D_BUILD_EXAMPLES` option to `ON`. If using CMake GUI, tick the option. Then run:
```
cmake --build build -j4
```
To run an example, run the executable directly from the `/build/bin` folder as the file paths in the examples are relative. <br>
Eg. to run example1 on Windows built with MinGW, run:
```
cd build/bin
./example1.exe
```
# Installing
Install the library to the standard CMake install path by doing:
```
cmake --install build
```
To install to a custom desired path:
```
cmake --install build --prefix "custom/install/path"
```

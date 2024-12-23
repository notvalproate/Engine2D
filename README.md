# Engine2D - A 2D C++ Game Engine

Engine2D is a game engine written in C++ to develop 2D games. The engine is **heavily** inspired by the [Unity Scripting API](https://docs.unity3d.com/6000.0/Documentation/ScriptReference/) and is still a huge **WIP**. It uses [SDL2](https://github.com/libsdl-org/SDL) to support cross-platform graphics and hardware and [Box2D](https://github.com/erincatto/box2d) (v2.4) as the physics engine.

# Prerequisites
The following dependencies are included in git submodules and will be installed and built along with the project:
- SDL2 - https://github.com/libsdl-org/SDL
- SDL2_image - https://github.com/libsdl-org/SDL_image
- Box2D (v2.4) - https://github.com/erincatto/box2d
> Note: Box2D v3.0 is a complete rewrite of Box2D and is incompatible with Engine2D. For now, there are no plans to migrate to v3.0 until it improves in certain aspects I feel are lacking.

# Building
Building is the same as any other CMake project:
- Clone the repo:
```
git clone --recurse-submodules -j4 https://github.com/notvalproate/Engine2D.git
```
- Build the library:
```
cmake -B build
cmake --build build -j4
```
- To build with **examples** (There is only one for now):
```
cmake -B build
```
Then, edit CMakeCache.txt and set the `ENGINE2D_BUILD_EXAMPLES` option to `ON`. If using CMake GUI tick the option. Then:
```
cmake --build build -j4
```
The built binaries and archives will be written to `/build/bin`
> Note: Building and configuring might take some time since 3 dependencies have to be built. You can speed up the process of building by appropriately adjusting the `-j` flag and adding the number of CPU cores you want to use to build (For example on my i7-8700 I use `-j10`)

# Running An Example
To run an example, run the executable directly from the `/build/bin` folder as the file paths in the examples are relative. <br>
Eg to run example1 on Windows:
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

# Engine2D - A 2D C++ Game Engine

Engine2D is a game engine written in C++ to develop 2D games. The engine is **heavily** inspired by the [Unity Scripting API](https://docs.unity3d.com/6000.0/Documentation/ScriptReference/) and is still a huge **WIP**. It uses [SDL2](https://github.com/libsdl-org/SDL) to support cross-platform graphics and hardware and [Box2D](https://github.com/erincatto/box2d) (v2.4) as the physics engine.

# Prerequisites
> *(Still unsure if it a good idea to add the dependencies as submodules, commit them with the project, or leave them as manual installations. For now, it is manual)*

Install the following libraries to your standard installation path for CMake to easily find them. If not, you can manually specify the paths.
- SDL2 - https://github.com/libsdl-org/SDL
- SDL2_image - https://github.com/libsdl-org/SDL_image
- Box2D (v2.4) - https://github.com/erincatto/box2d
> Note: Box2D v3.0 is a complete rewrite of Box2D and is incompatible with Engine2D. For now, there are no plans to migrate to v3.0 until it improves in certain aspects I feel are lacking.

# Building
After installing your dependencies, building is the same as any other CMake project!

- To build the library:
```
cmake -B build
cmake --build build -j4
```
- To build with **examples** (There is only one for now):
```
cmake -B build
```
Then, edit CMakeCache.txt and set the `BUILD_EXAMPLES` option to `ON`. If using CMake GUI tick the option. Then:
```
cmake --build build -j4
```
The built binaries and archives will be written to `/build/bin`

# Running An Example
To run an example, run the executable directly from the `/build/bin` folder as the file paths in the examples are relative. <br>
Eg to run example1 on Windows:
```
cd build/bin
./example1.exe
```
The needed shared libraries (SDL2, SDL2_image and Box2D) should be copied to `bin` if CMake finds them. If not you may have to manually copy them (.dll, .so, .dylib) to the `bin` folder to run the.
# Installing
Install the library to the standard CMake install path by doing:
```
cmake --install build
```
To install to a custom desired path:
```
cmake --install build --prefix "custom/install/path"
```

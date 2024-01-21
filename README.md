# QuidditchField
A simple version of the Quidditch Field from the Harry Potter franchise, implemented using OpenGL.

## Introduction
The idea for the project comes from the Harry Potter franchise, or rather from the fictional sport called Quidditch. The rules of the sport are simple: the player has to score a goal in one of the three hoops of the opposing team, while flying on a broomstick. 
  
The whole environment for Quidditch sports consists of: a field where players fly on broomsticks, high hoops (goals), and the high towers where the audience sits are placed around the whole field.

In this project our goal was to make a program that would allow movement through a simple version of the Quidditch Pitch, where the viewer could explore the terrain "on foot" or "on a broom".

This project is an advanced version of the [OpenGlPrj project](https://github.com/joksim/OpenGLPrj). It uses OpenGL for rendering the 3D objects, and it operates using the following libraries:
* [assimp](https://github.com/assimp/assimp/tree/feb0303fd0ab570ee05c326aa2e39d7312ea932d) for loading the 3D objects from .obj files
* [GLAD](https://github.com/Dav1dde/glad) as OpenGL Function Loader
* [GLFW](https://github.com/glfw/glfw) for Windowing and Input
* [GLM](https://github.com/g-truc/glm) for OpenGL Mathematics
* [stb](https://github.com/nothings/stb) for images and texture loading

The filed is drawn using [Blender](https://www.blender.org/), and then exported into the `field.obj` file that is used in the project.

![image](https://github.com/tkovachka/QuidditchField/assets/100356997/0c62b527-a1d9-4e58-bbcc-b097e4fb9e41)


## Setup
Recursively clone the repository:
```
git clone --recursive https://github.com/tkovachka/QuidditchField.git
```
If you are using an IDE that supports CMake builds (QtCreator, Jetbrains CLion, Visual Studio), open the cloned directory as project (the directory `code` with the `CMakeLists.txt` file).

We recomend using out of source builds.

Project files can be generated using the following steps:

### Using CMake from cmd/git bash/powershell:
Create a folder for the project build (e.g. build). It can be in the same level/directory as the OpenGLPrj. The hierarchy should be as depicted
```
|
|-- build/
|-- code/
```
### Using CMake generate the specified project files into the build directory:
```
cd build
cmake ../code/
```
Open the project files in the spedified IDE (Visual Studio, QtCreator, ...)

### Using cmake-gui:
Open the `cmake-gui` app. For the source folder select the `code` directory. For build directory choose an empty directory (for example, directory named `build` at the same level as `code`. With both folders chosen, click __Configure__ and if successfull procede to __Generate__ the build files. A tutorial is given at: (https://cgold.readthedocs.io/en/latest/tutorials/cmake-stages.html#).


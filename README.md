# The Aviator

This is an OpenGL project inspired by: https://tympanus.net/codrops/2016/04/26/the-aviator-animating-basic-3d-scene-threejs/
The main purpose of this project is to practice OpenGL rendering techniques and applying 3D maths.

## Video
[![alt text](https://github.com/Guo-Haowei/TheAviator/blob/master/screenshots/Screenshot.png)](https://www.youtube.com/watch?v=Nv-wizLF6zk)

## FEATURES
### Motion Blur
I implement motion blur using per-object motion blur technique. By uploading the previous transformation matrix to shader, I can calculate the velocity and save the velocity of each object to a velocity map.

![alt text](https://github.com/Guo-Haowei/TheAviator/blob/master/screenshots/Velocity%20Map.png)

**Velocity Map**

Then roughly calculate the position of each fragment over 10 different time steps, and average the fragment color, this is the final color of the fragment.

![alt text](https://github.com/Guo-Haowei/TheAviator/blob/master/screenshots/Motion%20blur.png)

**Motion Blur Demo**

****

### Shadow Mapping
Render the scene from the perspective of the light source, save the depth information.

![alt text](https://github.com/Guo-Haowei/TheAviator/blob/master/screenshots/Depth%20Map.png)

**Depth Map**

In the main render pass, calculate the depth of each pixel, if the calculated depth is smaller than the depth from depth map, then this fragment is shadowed.

![alt text](https://github.com/Guo-Haowei/TheAviator/blob/master/screenshots/Shadow.png)

**Shadow Demo**

****

### Lens Flare
I implement lens flare by calculating the screen position of the light source, linearly render some textures along a line from screen position of the sun to center of the screen.

****

### Other Features
* Particle system
* Physics engine
* Collision detection and collision response
* Linear animation
* Primitives including tetrahedron, box, sphere, cylinder
* Linear Fog calculation in shader
* Alpha blending

## Implementation

* **Algorithms, data structures, and complexities**
<p>
I used structure `unordered_map<GLuint, vector<Entity*>>` to keep all the entities to render, where GLuint is the index of their corresponding Vertex Array Object(VAO) of the entities listed in the vector.
<p>
In this way, I can bind the same VAO only once each frame, and the number of VAO binding are greatly reduced.

* **Modularity, data abstraction and encapsulation**
<p>
Modulaized a few utility class to manage window, create shaders, load images and so on.
<p>
The base class of all scene object has a transformation matrix.
 
* **Platform and system dependence or independence**
<p>
This program runs on all Windows, MacOS and Linux system
* **Global constants and configurability**
<p>
In order to change global variables such as camera position, light position, etc easily without recompiling the code. Icreated a function to read all variables from a configuration file.

* **Debugging approach and utilities, testing and verifying practices**
<p> I used [RenderDoc](https://renderdoc.org/) for debugging. It can screenshot on frame and list all the draw calls and textures. However it doesn't support GLSL to well, so I can't debug the value been passed to shader with it.


## Installation

### Windows
I use Visual Studio 2017 to build this project on windows. I downloaded 32-bit glfw from http://www.glfw.org/download.html,
glew from http://glew.sourceforge.net/ and glm from https://github.com/g-truc/glm/releases/tag/0.9.8.5.

Sonar System did a great job explaining how to set up glew, glfw and glm on windows, here are his youtube tutorials:
https://www.youtube.com/watch?v=vGptI11wRxE&t=639s
https://www.youtube.com/watch?v=6nGpoY-L2Xk

If you are also using Visual Studio 2017, make sure to install Windows 8.1 SDK and Windows Universal CRT SDK.
Also, try to use glew32s.lib instead of glew32.lib ("s" stands for static)

### MacOS
If you already have `brew` installed, skip this step. Otherwise, do
```
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```
to install `brew`.

After installing `brew`, you need to download a few more packages.

```
brew install cmake
brew install pkg-config
brew install glew
brew install glfw
brew install glm
```
### Linux
Similar to `brew install`, do `apt-get` to download desired packages.
```
sudo apt-get update
sudo apt-get install cmake
sudo apt-get install pkgconf
sudo apt-get install libglfw-dev
sudo apt-get install libglew-dev
sudo apt-get install libglm-dev
```
### Compile and Run

```
mkdir build
cd build
cmake ..
make
./TheAviator
```

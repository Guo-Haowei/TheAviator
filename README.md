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

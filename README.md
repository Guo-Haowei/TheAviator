# The Aviator

This is an OpenGL project inspired by: https://tympanus.net/codrops/2016/04/26/the-aviator-animating-basic-3d-scene-threejs/
The main purpose of this project is to practice OpenGL rendering techniques and applying 3D maths.

## Screenshots
[![alt text](https://github.com/Guo-Haowei/TheAviator/blob/master/screenshots/Screenshot.png)](https://www.youtube.com/watch?v=3o_Xw1SkKuE)
#### Front face culling
![alt text](https://github.com/Guo-Haowei/TheAviator/blob/master/screenshots/Front_face_culling.png)
#### Use tetrahedron for models
![alt text](https://github.com/Guo-Haowei/TheAviator/blob/master/screenshots/Tetrahedron.png)
#### Use low-poly sphere for models
![alt text](https://github.com/Guo-Haowei/TheAviator/blob/master/screenshots/Sphere.png)

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
ccmake ..
make
./TheAviator
```
## FEATURES
* Primitives including tetrahedron, box, sphere, cylinder
* Shadow mapping
* Fog
* Transparent water
* Collision detection
* Particle effect
* Animation (hair and propeller)
* Realtime normal vector calculation using geometry shader
* UI (comming soon)

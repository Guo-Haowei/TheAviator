# The Aviator

This is an OpenGL project inspired by: https://tympanus.net/codrops/2016/04/26/the-aviator-animating-basic-3d-scene-threejs/
The main purpose of this project is to practice OpenGL rendering techniques and applying 3D maths.

## Todo
- [x] Use cmake for all platforms
- [ ] Change shadow map projection to orthographic
- [ ] Fix motion blur bug on windows
- [ ] Replace GLFW3 with SDL2
- [ ] Build on IOS and WebAssembly
- [ ] Refactor code

## Video
[![alt text](https://github.com/Guo-Haowei/TheAviator/blob/master/screenshots/Screenshot.png)](https://youtu.be/3o_Xw1SkKuE)

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

### Compile and Run

```
mkdir build
cd build
cmake ..
make
./TheAviator
```

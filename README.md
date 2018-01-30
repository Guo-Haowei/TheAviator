# The Aviator (In Progess)

This is an OpenGL coding practice project inspired by: https://tympanus.net/codrops/2016/04/26/the-aviator-animating-basic-3d-scene-threejs/

## Installation

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
If you do not have GLFW version 3, download the source distribution and run cmake:
```
wget https://github.com/glfw/glfw/releases/download/3.1.2/glfw-3.1.2.zip
unzip glfw-3.1.2.zip
cd glfw*
mkdir build
cd build
ccmake ..

make
sudo make install
```
If cmake is not able to find the glm package, do the following:
```
wget https://github.com/glfw/glfw/releases/download/3.1.2/glfw-3.1.2.zip
unzip glfw-3.1.2.zip
cd glfw*
mkdir build
cd build
ccmake ..

make
sudo make install
```

### Compile Code

```
mkdir build
cd build
ccmake ..
make
./TheAviator
```
## TODO
* Fog effect

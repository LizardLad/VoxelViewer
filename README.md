# VoxelViewer
Just a simple voxel engine in C++ using OpenGL and SDL2


# Controls

W = Forward

A = Left

S = Backwards

D = Right

C or Pagedown = Down

Space or Pageup = Up

Escape = Release cursor

Delete = End program

Left Click = Place currently selected block

Right Click = Remove block

Mousewheel = Change currently selected block

Currently selected block is printed in terminal

# Building on RPM based linux

### Install Dependancies:

sudo dnf install sdl2-devel glm-devel opengl-devel git g++ cmake make

### Clone repository:

git clone ...

cd VoxelVeiwer

#### Building:

mkdir build

cd build

cmake ..

make -j4

mkdir shaders

cp ../src/shaders/*.glsl shaders/


### Run:

./VoxelVeiwer

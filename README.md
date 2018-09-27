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

1. `sudo dnf install sdl2-devel glm-devel opengl-devel git g++ cmake make`

### Clone repository:

1. `git clone ...`

2. `cd VoxelVeiwer`

#### Building:

1. `mkdir build`

2. `cd build`

3. `cmake ..`

4. `make -j4`

5. `mkdir shaders`

6. `cp ../src/shaders/*.glsl shaders/`


### Run:

1. `./VoxelVeiwer`

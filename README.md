# AGK

Anubis Graphics Kit (AGK) is a photo-realism rendering engine made in OpenGL 4 for SDL2, the purpose of this engine is to do rendering tests and implement empirical rendering equations, techniques, concepts, & specifically the umbrella PBR.  
An official researcher article is being written together with this project to investigate a way to connect heightmaps. The research is not linked to any institution, perhaps I will publish here at the ends of the article project,  
but VokeGpu will keep updating this engine project.

If you are on Windows, please replace `"Unix Makefiles"` with `"MinGW Makefiles"` or `"Ninja"`, and run the command:  
`cmake -S . -B ./cmake-build-debug/ -G "Unix Makefiles" && cmake --build ./cmake-build-debug/`

The location of executables is on `./build/`, choose your system and run Anubis Graphics Kit. Note: If the Linux version is broken, replace the Linux `./data` with `./build/win/data/`, most of the commits are tested in Windows.

The project uses:
SDL2, GLEW, freetype, and [EKG GUI library](https://github.com/vokegpu/ekg-ui-library).

# GPU support

This project only is supported for Nvidia GPUs, sadly the shaders are incompatible with AMD GPUs.

---

# Philosophy & world rendering techniques used

The starter scene is located at `./client/scenes/starter.hpp`, you can create different scenes and load different contexts.  
The scenes work as a "script" context, where you can create objects, load assets, load models, and services to create logic systems, and scenery.

-- The features present in AGK are not marked with ~~this~~:
* Bidirectional reflectance distribution function.
* Fractal B motion (FBM).
* Chunking system.
* Procedural heightmap generation.
* Module system (possibility of fast creation of features).
* World time system with cycle of day and night (~~skybox, sun, moon & clouds too~~).
* Sky stars (real-time).
* High dynamic range (HDR).
* Dynamic input system.
* Frustum culling.
* Motion blur.
* Model files support: glTF, object wavefront, and mtllib.
* ~~Screen space occlusion ambient (SSAO)~~.
* Connected height maps.
* ~~Subsurface scattering~~.
* ~~Depth of field~~.
* ~~Animation.~~
* ~~Dynamic liquids interaction~~.
* ~~Photo-realism flowers, grass, trees, forest scenarios etc~~.

One of the most important rendering techniques used here is the umbrella of physically based rendering (PBR), the BRDF or bidirectional reflectance distribution function is the lighting equation used to process all in-scene models, also the terrain.  
![Alt text](/splash/splash-brdf-3.png?raw=true)

The BRDF makes the terrain and world objects look soft and realistic at some point, the visual effect also do subsurface scattering or known as BSSDF (bidirectional subsurface scattering distribution function) effect.
![Alt text](/splash/splash-brdf-2.png?raw=true)

The terrain is procedurally generated using compute shaders and FBM equation with Perlin noise from Ken Perlin, the volume of terrain processed from grey-scale is real-time computed using tessellation shader with 4 patches, credits to [learnopengl](https://learnopengl.com) which I have used as a book to learn about terrain rendering.
For rendering all terrain AGK come with a chunking system.  
The GUI is powered by VokeGpu [ekg ui library](https://github.com/vokegpu/ekg-ui-library) a high-performance library made in OpenGL for SDL2 written in C++.
All detailed processes are included in the article written for this project, called Real-Time Terrain Generator by Rina.

![Alt text](/splash/terrain-hmap-connect-fixed-2.png)
![img.png](/splash/real-time-stars.png)
![img.png](/splash/terrain-2.png)
![img.png](/splash/terrain-3.png)

# Contributing

We only accept codes with the following case style:
```cpp
// nocase.hpp

namespace agk::cat {
  bool checksomethingcat();
}

class foobar {
public:
  void do_something();
};

// impl:
foobar insane_cat {};
insane_cat.do_something();

// about {};
// First: This automatically init everything
// e.g:
float empty_buffer1[12] {}; // simply initialize everything
float empty_buffer2[] {}; // also yes

// Second:
// so I suggest you use a lot, by the simple case of prevent random memory.
int32_t foo; // without {} you can get something like -48329482349
```
Thank you.

# License & credits
The project is under license MIT.  
Thank you specially to author(s) of Learn OpenGL, GPU Zen 2, Real-Time Rendering Techniques, GPU Gems 3, Physically Based Rendering: From Theory to Implementation, GLSL 4: Cookbook.  
Without these books I could not learn. Learning is a process that do not have an end.

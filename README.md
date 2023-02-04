# AGK

Anubis Graphics Kit (AGK) is a photo-realism rendering engine made in OpenGL 4 for SDL2, the purpose of this engine is to do rendering tests and implement impirical rendering equations, techniques, concetps & specifically the umbrealla PBR.  
There is an official researcher article being write together with this project to investigate a way to connect heightmaps. The research is not linked to any institution, perhaps I will publish here at the ends of article project,  
but VokeGpu will keep update this engine project.

If you are on Windows, please replace `"Unix Makefiles"` with `"MinGW Makefiles"`, run the command:  
`cmake -S . -B ./cmake-build-debug/ -G "Unix Makefiles" && cmake --build ./cmake-build-debug/"`

The location of executables is in `./build/`, choose your system and run Anubis Graphics Kit.

---

# Philosophy & world rendering techniques used

-- The features present in AGK are not marked with ~~this~~:
* Bidirectional reflectance distribution function.
* Fractal B motion (FBM).
* Chunking system.
* Procedural heightmap generation.
* Module system (possibility fast creation of features).
* World time system with cycle of day and night (~~skybox, sun, moon & clouds too~~).
* High dynamic range (HDR).
* Dynamic input system.
* ~~Motion blur~~.
* ~~Screen space occlusion ambient (SSAO)~~.
* ~~Connected heightmaps.~~
* ~~Subsurface scattering~~.
* ~~Depth of field~~.
* ~~Animation.~~
* ~~Dynamic liquids interaction~~.
* ~~Photo-realism flowers, grass, trees, florest scenarios etc~~.

One of most important rendering technique used here is the umbrella of physically based rendering (PBR), the BRDF or bidirectional reflectance distribution function is the lighting equation used to process all in scene models, also the terrain.  
![Alt text](/splash/splash-brdf-1.png?raw=true)

The BRDF make the terrain and world objects looks soft and realistic at some point, the visual effect also do subsurface scattering or known as BSSDF (bidirectional subsurface scattering distribution function) effect.
![Alt text](/splash/splash-brdf-2.png?raw=true)

The terrain is procedural generated using compute shaders and FBM equation with perlin noise from Ken Perlin, the volume of terrain processed from grey-scale is real-time computed using tessellation shader with 4 patches, credits to [learnopengl](https://learnopengl.com) which I have used as book to learn about terrain generator.
For rendering all terrain AGK come with a chunking system.
![Alt text](/splash/splash-terrain-1.png)

The GUI is powered by VokeGpu [ekg ui library](https://github.com/vokegpu/ekg-ui-library) a high-performance library made in OpenGL for SDL2 written in C++.

# License & credits
The project is under license MIT.  
Thank you specially to author(s) of Learn Opengl, GPU Zen 2, Real-Time Rendering techniques, GPU Gems 3, Physically Based Rendering: From Theory to Implementation, GLSL 4: Cookbook.  
Without these books I could not learn. Learning is a process that do not have an end.
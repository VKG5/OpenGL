# OpenGL

A rendering engine written in C++ with OpenGL. WIP, trying to make a ray-tracing/path tracing engine with a complete interface. 
- [x] Resizeable Window
- [x] Phong Illumination Model (Ambient, Diffuse, Specular)
- [x] Shadows (Omni-directional)
- [x] Light Sources (Directional, Point, Spotlight)
- [x] Model loading (Now supports multiple textures!)
- [x] Texture support (Only diffuse)
- [x] Multi model support
- [x] Environment Mapping 

## Material Support
- [x] Reflective
- [x] Refractive
- [x] Fresnel
- [x] Dispersion
- [x] Specular
- [x] Metallic
- [x] Multiple Maps - Normal, Specular, Diffuse (TODO : Add Ambient Occlusion and Alpha Maps) 

## Procedural
- [x] Basic Procedural Generator

## Animation
- [ ] Animation models

## Path Tracing
This path tracer has been adapted largely from Sebastian Lague's [Ray-Tracing Series](https://github.com/SebLague/Ray-Tracing). All credits to him for making such useful and easy to understand tutorials :) The Path Tracer currently supports rendering basic shapes such as cubes and spheres. I developed it on a separate branch due to large changes in the main code, because I did not want to scrape off my older code. I am planning to integrate it in the main branch soon. Also, it is very inefficient at the moment, gives about 5-10 FPS on Path Tracing mode.

Below are some renders that were taken by pressing **'F'** on the keyboard. The scenes are pre-defined and present in the Procedural_scenes.h

[Supporting Documentation](src/Path_Tracing/Real_Time_Rendering.pdf)

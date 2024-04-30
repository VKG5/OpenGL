# OpenGL

A rendering engine written in C++ with OpenGL. WIP, trying to make a ray-tracing/path tracing engine with a complete interface. I am not on the dockable branch of ImGUI so there's no docking yet. Docking would be really cool though!
- [x] Resizeable Window
- [x] Phong Illumination Model (Ambient, Diffuse, Specular)
- [x] Shadows (Omni-directional)
- [x] Light Sources (Directional, Point, Spotlight)
- [x] Model loading (Now supports multiple textures!)
- [x] Texture support (Only diffuse)
- [x] Multi model support
- [x] Environment Mapping 

Below are two different render modes (Material Preview and Wireframe). You can refer to the [Files](/files/) folder which includes more snapshots from the application.
<div align="center">
    <img src="/files/materialPreview.png" width="400"/> <img src="/files/wireframe.png" width="400"/>
</div> 

## Physically Based Rendering Support
The renderer supports PBR materials with the following properties! There is basic control over combination of various properties and all the switching happens in 1 [Fragment Shader](/src/Imgui/Shaders/BRDF_Normals.frag). There are 4 rendering modes:

    Phong Illumination
    Blinn-Phong Illumination
    Gooch Shading
    Minnaert Shading

- [x] Reflective
- [x] Refractive
- [x] Fresnel
- [x] Dispersion
- [x] Specular
- [x] Metallic (This works inversely because of the way it is setup)
- [x] Multiple Maps - Normal, Specular, Diffuse (TODO : Add Ambient Occlusion and Alpha Maps)

One important point to keep in mind is that, the normals are being inverted while reading models. Why? I made the computations happen as such, which is not the best approach, but I will be switching that out later.

You can refer to the following playlist on [YouTube](https://www.youtube.com/playlist?list=PLHAeiYMnJIqJl-VByE7uSVqTOvJzzD0pU) that showcase the exact features!

## Procedural
This is a very simple generator that distributes random points in a NxN grid and prevents overlap. The functions can be found in [randomDistribute.cpp](/src/Imgui/commons/algorithms/randomDistribute.cpp). There are user friendly controls that can change various parameters such as the seed, gride size, point scale, and number of points. The "update" button is used to reflect the changes! *(There may be crashes at some values)*
- [x] Basic Procedural City Generator

<div align="center">
    <img src="/files/proceduralUI.png"/>
</div>

Some samples of the for the city are attached below:
<div align="center">
    <img src="/files/pcg%231.png" width="400"/> <img src="/files/pcg%232.png" width="400"/>
</div> 

The generator runs in real-time, without any noticeable delay.

## Animation
I want to integrate rigged characters. I tried using ASSIMP but didn't find that too optimal? Currently looking for more ways to do so!
- [ ] Animation models

## Path Tracing
This path tracer has been adapted largely from Sebastian Lague's [Ray-Tracing Series](https://github.com/SebLague/Ray-Tracing). All credits to him for making such useful and easy to understand tutorials :) The Path Tracer currently supports rendering basic shapes such as cubes and spheres. I developed it on a separate branch due to large changes in the main code, because I did not want to scrape off my older code. I am planning to integrate it in the main branch soon. Also, it is very inefficient at the moment, gives about 5-10 FPS on Path Tracing mode.

Below are some renders that were taken by pressing **'F'** on the keyboard. The scenes are pre-defined and present in the Procedural_scenes.h

<div align="center">
    <img src="/src/Path_Tracing/src/renders/denoisedCoolBall.png" width="400"/>  <img src="/src/Path_Tracing/src/renders/denoisedCornellBox.png" width="400"/> 
    <br>
    <img src="/src/Path_Tracing/src/renders/EmitDarkDenoise.png" width="400"/> <img src="/src/Path_Tracing/src/renders/MirrorBallsDenoise.png" width="400"/>
</div>

The renders have been denoised using the Blender Denoiser (Compositor). They were exported using the AGX color space in Blender 4.1.0

[Supporting Documentation](src/Path_Tracing/Real_Time_Rendering.pdf)

## References
I really love the following books from Packt, which go over OpenGL, CMake, and the supporting dependencies.
<ul>
    <li>
        <a href="https://subscription.packtpub.com/book/game-development/9781782167020/1">OpenGL 4 Shading Language Cookbook - Second Edition</a> : David Wolff, David A Wolff
    </li>
    <li>
        <a href="https://subscription.packtpub.com/book/programming/9781801070058/1">Modern CMake for C++</a> : Rafał Świdziński
    </li>
    <li>
        <a href="https://subscription.packtpub.com/book/game-development/9781838986193/1">3D Graphics Rendering Cookbook</a> : Sergey Kosarevsky, Viktor Latypov
    </li>
</ul>

## Conclusion
This has been one of the best projects that I have worked on in a long while! I want to improve the engine as much as I can, in free time. This is a good starting point with all the core functionality in-place with a fairly good project structure. Feel free to use and modify it as suited to your needs :D

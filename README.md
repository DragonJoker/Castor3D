|         Server     | Result |
|:------------------:|--------|
|  Travis CI (Linux) | [![Build Status](https://travis-ci.org/DragonJoker/Castor3D.svg?branch=development)](https://travis-ci.org/DragonJoker/Castor3D) |
| AppVeyor (Windows) | [![Build Status](https://ci.appveyor.com/api/projects/status/github/DragonJoker/castor3d?branch=development&svg=true)](https://ci.appveyor.com/project/DragonJoker/castor3d) |


Castor3D
========

Castor3D is a 3D engine written in C++.
It works on Windows and also on GNU/Linux.
It uses OpenGL (with GLSL).

Features
--------

- Deferred rendering.
- Normal mapping
- Parallax Occlusion mapping.
- Screen Space Ambient Occlusion.
- Shadow Mapping.
- Reflection Mapping.
- PBR rendering (Metallic and Specular), and Legacy rendering.
- HDR rendering with various tone mapping operators.
- Scene graph.
- Modular architecture through plug-ins.
- Shaders are generated automatically from material and pass configuration.
- Shaders are writable directly from C++ code.

Implemented Plug-ins
--------------------

**Renderers**
- GlRenderSystem.

**Importers**
- ASE: ASCII Scene Export mesh importer.
- 3DS: 3D Studio mesh importer.
- LWO: Lightwave Object mesh importer.
- MD2: Quake II mesh importer.
- MD3: Quake III mesh importer.
- ASSIMP: Multiple format mesh importer, replaces the previous ones if Assimp is available.
- PLY: Stanford Polygon library mesh importer.
- OBJ: Wavefront OBJ mesh importer.
- FBX: Autodesk Maya mesh importer.

**Dividers**
- Loop subdivision surfaces.
- Phong tessellation.
- PN-Triangles surfaces.

**PostEffects**
- Bloom.
- GrayScale.
- FXAA.

**Generators**
- DiamondSquareTerrain: to generate terrains inside Castor3D scenes, using diamond-quare algorithm.

**Generic**
- CastorGUI: to build GUIs inside Castor3D scenes.

**ToneMappings**
- LinearToneMapping: Default tone mapping.
- HaarmPieterDuikerToneMapping: Haarm Pieter Duiker tone mapping.
- HejlBurgessDawsonToneMapping: Hejl Burgess Dawson tone mapping.
- ReinhardToneMapping: Reinhard tone mapping.


![Sponza](http://dragonjoker.github.com/Castor3D/img/Sponza-PBR-Bloom-small.png)
![Cerberus](http://dragonjoker.github.com/Castor3D/img/Cerberus-PBR-small.png)
![Park](http://dragonjoker.github.com/Castor3D/img/Park-Legacy-small.png)

Links
-----

- [Building Castor3D](http://dragonjoker.github.com/Castor3D/pages/build)
- [Playlist on YouTube](https://www.youtube.com/playlist?list=PLKA1SVXuAbMNaFbSJyAN_4yD2bzNlgES3)

General Questions
-----------------

1. I need a feature that Castor3D do not implement, what should I do ?
  There are two ways.
  First, Castor3D is a free software and library, so you are welcome to improve
  it yourself ;-) !
  Second, just ask the author :-) ! 

2. I implemented a cool feature, can I commit it ?
  Obviously you are welcome to send the author any improvement. But obviously
  the author is quite careful with readability, correctness and stability, 
  and will read and check any contribution before including it.
  Thus, it can take time... There are few basic rules to write contributions:
  1. Do never change the indentation of any part of the code that's not yours.
  2. Do never send codes without a lot of comments in (bad or good) English.
  3. Do never send codes with obscure and/or non-English variable names.

3. How can I ask the author ?
  Just send a mail to <dragonjoker59@hotmail.com> !

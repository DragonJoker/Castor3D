<p align="center">
  <a href="https://github.com/DragonJoker/Castor3D/actions?query=workflow%3ABuild"><img alt="Build status" src="https://github.com/DragonJoker/Castor3D/workflows/Build/badge.svg"></a>
</p>


Castor3D
========

Castor3D is a 3D engine written in C++ 17.  
It works on Windows and on GNU/Linux.  
It relies on Ashes as a rendering API, allowing Vulkan, OpenGL and Direct3D 11.  

Features
--------

- Deferred rendering.
- Normal mapping.
- Parallax Occlusion mapping.
- Screen Space Ambient Occlusion (using [Scalable Ambiant Obscurance](https://casual-effects.com/research/McGuire2012SAO/index.html)).
- Shadow Mapping (allowing to choose between Raw, PCF or Variance Shadow Maps).
- Reflection/Refraction Mapping.
- PBR rendering (Metallic and Specular workflows), and Phong/Blinn-Phong rendering.
- HDR rendering with various tone mapping operators.
- Screen Space Subsurface Scattering (without backlit transmittance yet).
- Volumetric Light Scattering for directional light source projecting shadows.
- Cascaded Shadow Maps for directional light sources.
- Global Illumination through Ligh Propagation Volumes (layered or non layered, with or without geometry injection), or through Voxel Cone Tracing.
- Frustum culling.
- Scene graph.
- Render graph.
- Modular architecture through plug-ins.
- Shaders are generated automatically from material and pass configuration.
- Shaders are writable directly from C++ code.

Implemented Plug-ins
--------------------

**Importers**
- ASSIMP: Multiple format mesh importer.
- PLY: Stanford Polygon library mesh importer.

**Dividers**
- Loop subdivision surfaces.
- Phong tessellation.
- PN-Triangles surfaces.

**PostEffects**
- Bloom: HDR Bloom implementation.
- DrawEdges: Detects and renders edges, based on normal, depth, and or object ID.
- FilmGrain: To display some grain on the render.
- GrayScale.
- LightStreaks (using Kawase Light Streaks).
- FXAA Antialiasing.
- SMAA Antialiasing (1X and T2X so far).
- Linear Motion Blur.

**Generators**
- DiamondSquareTerrain: To generate terrains inside Castor3D scenes, using diamond-quare algorithm.

**Generic**
- CastorGUI: To build GUIs inside Castor3D scene files.
- ToonMaterial: A toon material (to be combined with DrawEdges plugin).

**ToneMappings**
- LinearToneMapping: Default tone mapping.
- HaarmPieterDuikerToneMapping: Haarm Pieter Duiker tone mapping.
- HejlBurgessDawsonToneMapping: Hejl Burgess Dawson tone mapping.
- ReinhardToneMapping: Reinhard tone mapping.
- Uncharted2ToneMapping: Uncharted 2 tone mapping.

Other applications
------------------
- CastorViewer: A scene viewer based on Castor3D.
- CastorMeshConverter: A converter from various mesh files to Castor3D mesh format.
- CastorMeshUpgrader: Upgrades from earlier versions of Castor3D mesh format to the latest one.
- ImgConverter: Allows you to convert any image file type to XPM or ICO.

Links
-----

- [Building Castor3D](http://dragonjoker.github.io/Castor3D/pages/build)
- [API Documentation](http://dragonjoker.github.io/Castor3D/doc/)
- [Playlist on YouTube](https://www.youtube.com/playlist?list=PLKA1SVXuAbMNaFbSJyAN_4yD2bzNlgES3)


![Sponza](http://dragonjoker.github.io/Castor3D/img/Sponza-PBR-Bloom-Small.png)
![Cerberus](http://dragonjoker.github.io/Castor3D/img/Cerberus-PBR-Small.png)
![Park, Legacy](http://dragonjoker.github.io/Castor3D/img/Park-Legacy-Small.png)
![Park, PBR](http://dragonjoker.github.io/Castor3D/img/Park-PBR-Small.png)
![Nyra, PBR](http://dragonjoker.github.io/Castor3D/img/Nyra-PBR-MR-Small.png)
![SanMiguel, PBR](http://dragonjoker.github.io/Castor3D/img/SanMiguel-PBR-SG-Small.png)

Contact
-------

You can rech me on the Discord server dedicated to my projects: [DragonJoker's Lair](https://discord.gg/jue8kW)

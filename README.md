<p align="center">
  <a href="https://github.com/DragonJoker/Castor3D/actions?query=workflow%3ABuild"><img alt="Build status" src="https://github.com/DragonJoker/Castor3D/workflows/Build/badge.svg"></a>
</p>

# Castor3D

Castor3D is a 3D engine written in C++ 17.
It works on Windows and GNU/Linux.
It relies on Ashes as a rendering API, allowing Vulkan, Direct3D 11, and OpenGL.
The project itself is compilable on supported platforms using CMake (\ref build).

## Features

- Deferred rendering for opaque objects.
- Blended Weighted rendering for transparent objects.
- Normal mapping.
- Shadow Mapping (allowing to choose between Raw, PCF or Variance Shadow Maps).
- Parallax Occlusion mapping.
- Screen Space Ambient Occlusion (using [Scalable Ambiant Obscurance](https://casual-effects.com/research/McGuire2012SAO/index.html)).
- Reflection/Refraction Mapping.
- PBR rendering (Metallic and Specular workflows), and Phong/Blinn-Phong rendering.
- HDR rendering with various tone mapping operators.
- Screen Space Subsurface Scattering (without backlit transmittance yet).
- Volumetric Light Scattering for the directional light source, if it projects shadows.
- Cascaded Shadow Maps for the directional light source.
- Global Illumination, through Light Propagation Volumes (layered or non layered, with or without geometry injection), or through Voxel Cone Tracing.
- Frustum culling.
- Scene graph.
- Render graph.
- Modular architecture through plug-ins.
- Shaders are generated automatically from pipeline configuration.
- Shaders are writable directly from C++ code.
- Scenes are described using a text format easily comprehensible and extensible.
- Asynchronous (user defined) or synchronous (thread) rendering.

## Implemented Plug-ins

### Importers
- ASSIMP: Multiple format mesh importer.
- PLY: Stanford Polygon library mesh importer.

### Dividers
- Loop subdivision surfaces.
- Phong tessellation.
- PN-Triangles surfaces.

### PostEffects
- Bloom: HDR Bloom implementation.
- DrawEdges: Detects and renders edges, based on normal, depth, and or object ID.
- FilmGrain: To display some grain on the render.
- GrayScale.
- LightStreaks (using Kawase Light Streaks).
- FXAA Antialiasing.
- SMAA Antialiasing (1X and T2X so far).
- Linear Motion Blur.

### Generators
- DiamondSquareTerrain: to generate terrains inside Castor3D scenes, using diamond-quare algorithm.

### Generic
- CastorGUI: to build GUIs inside Castor3D scenes.
- ToonMaterial: A toon material (to be combined with DrawEdges plugin).

### ToneMappings
- LinearToneMapping: Default tone mapping.
- HaarmPieterDuikerToneMapping: Haarm Pieter Duiker tone mapping.
- HejlBurgessDawsonToneMapping: Hejl Burgess Dawson tone mapping.
- ReinhardToneMapping: Reinhard tone mapping.
- Uncharted2ToneMapping: Uncharted 2 tone mapping.

## Other applications

- CastorViewer: A scene viewer based on Castor3D.
- CastorMeshConverter: A converter from various mesh files to Castor3D mesh format.
- CastorMeshUpgrader: Upgrades from earlier versions of Castor3D mesh format to the latest one.
- ImgConverter: Allows you to convert any image file type to XPM or ICO.

## Links

- [Building Castor3D](https://dragonjoker.github.io/Castor3D/pages/build)
- [API Documentation](https://dragonjoker.github.io/Castor3D/doc)
- [Playlist on YouTube](https://www.youtube.com/playlist?list=PLKA1SVXuAbMNaFbSJyAN_4yD2bzNlgES3)

## Contact

You can reach me on the Discord server dedicated to my projects: [DragonJoker's Lair](https://discord.gg/8RBzU7VAJS)

## Screenshots

![Sponza](http://dragonjoker.github.io/Castor3D/img/Sponza-PBR-Bloom-Small.png)
![Sponza, VCT](http://dragonjoker.github.io/Castor3D/img/Sponza-PBR-VCT-Small.png)
![Cerberus](http://dragonjoker.github.io/Castor3D/img/Cerberus-PBR-Small.png)
![Park, Legacy](http://dragonjoker.github.io/Castor3D/img/Park-Legacy-Small.png)
![Park, PBR](http://dragonjoker.github.io/Castor3D/img/Park-PBR-Small.png)
![Nyra, PBR](http://dragonjoker.github.io/Castor3D/img/Nyra-PBR-MR-Small.png)
![SanMiguel, PBR](http://dragonjoker.github.io/Castor3D/img/SanMiguel-PBR-SG-Small.png)
![SanMiguel, PBR, LPV](http://dragonjoker.github.io/Castor3D/img/SanMiguel-PBR-SG-LPV-Small.png)

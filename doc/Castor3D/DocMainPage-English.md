Castor3D	{#mainpage}
========

Castor3D is a 3D engine written in C++ 20.

It works on Windows and GNU/Linux.

It relies on Vulkan as a rendering API.

The project itself is compilable on supported platforms using CMake and vcpkg (cf. [build](Build-English.md)).

## Features

- Clustered lighting to compute all light sources.
- Using a visibility buffer for opaque objects.
- Blended Weighted rendering for transparent objects.
- Normal mapping (using Mikktspace or explicit bitangents specification).
- Shadow Mapping (allowing to choose between Raw, PCF or Variance Shadow Maps).
- Parallax Occlusion mapping.
- Screen Space Ambient Occlusion (using [Scalable Ambiant Obscurance](https://casual-effects.com/research/McGuire2012SAO/index.html)).
- Reflection/Refraction Mapping.
- PBR rendering (Metallic and Specular workflows), and Phong/Blinn-Phong rendering.
- HDR rendering with various tone mapping operators and various colour grading operators.
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
- Scenes are described using a text format easily comprehensible and extensible. ([Scene File](SceneFile-English.md))
- Synchronous (user defined) or asynchronous (thread) rendering.
- Using Mesh and Task shaders, if available.
- GUI primitives.

## Implemented Plug-ins

### Importers
- ASSIMP: Multiple format mesh importer, using assimp library.
- glTF: glTF 2.0 importer, more precise than assimp's provided one, using fastgltf library.

### PostEffects
- Bloom: HDR Bloom implementation.
- PbrBloom: PBR Bloom implementation.
- DrawEdges: Detects and renders edges, based on normal, depth, and or object ID.
- FilmGrain: To display some grain on the render.
- GrayScale: Converts render in gray scale.
- LightStreaks (using Kawase Light Streaks).
- FXAA Antialiasing.
- SMAA Antialiasing (1X and T2X so far).
- Linear Motion Blur.
- DepthOfField: Implementation of [this depth of field](https://pixelmischiefblog.wordpress.com/2016/11/25/bokeh-depth-of-field/)

### Generators
- DiamondSquareTerrain : to generate terrains inside Castor3D scenes, using diamond-quare algorithm.

### Generic
- ToonMaterial: A toon material (to be combined with DrawEdges plugin).
- WaterMaterial: Water material, using normal maps.
- FFTOceanRendering: Ocean rendering using FFT generated surfaces.
- WavesRendering: Basic ocean rendering, specifying waves attributes.
- AtmosphereScattering : Sky and atmosphere rendering (using [Scalable and Production Ready Sky and Atmosphere Rendering Technique](https://sebh.github.io/publications/egsr2020.pdf)).

### ToneMappings
- None: A passthrough, used when rendering to an HDR screen.
- LinearToneMapping: Default tone mapping.
- HaarmPieterDuikerToneMapping: Haarm Pieter Duiker tone mapping.
- HejlBurgessDawsonToneMapping: Hejl Burgess Dawson tone mapping.
- ReinhardToneMapping: Reinhard tone mapping.
- Uncharted2ToneMapping: Uncharted 2 tone mapping.
- ACES: ACES tone mapping.

## Other applications

- CastorViewer: A scene viewer based on Castor3D.
- CastorMeshConverter: A converter from various mesh files to Castor3D mesh format.
- CastorMeshUpgrader: Upgrades from earlier versions of Castor3D mesh format to the latest one.
- ImgConverter: Allows you to convert any image file type to XPM or ICO.
- HeightMapToNormalMap: Allows you to convert an height map to a normal map.

## Screenshots

![Sponza, VCT](http://dragonjoker.github.io/Castor3D/img/Sponza-PBR-VCT-Small.png){width=800px}
![Cerberus](http://dragonjoker.github.io/Castor3D/img/Cerberus-PBR-Small.png){width=800px}
![Park, Comparison](http://dragonjoker.github.io/Castor3D/img/Park-Small.png){width=800px}
![Nyra, PBR](http://dragonjoker.github.io/Castor3D/img/Nyra-PBR-MR-Small.png){width=800px}
![SanMiguel, PBR](http://dragonjoker.github.io/Castor3D/img/SanMiguel-PBR-SG-Small.png){width=800px}
![SanMiguel, PBR, LPV](http://dragonjoker.github.io/Castor3D/img/SanMiguel-PBR-SG-LPV-Small.png){width=800px}
![Bistro, PBR, VCT](http://dragonjoker.github.io/Castor3D/img/Bistro-PBR-VCT-Small.png){width=800px}
![FFTOcean, Terrain](http://dragonjoker.github.io/Castor3D/img/FFTOcean-Terrain-Small.png){width=800px}

See [full changelog](ChangeLog-English.md) for more details.

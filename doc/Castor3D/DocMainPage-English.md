Castor3D	{#mainpage}
========

Castor3D is a 3D engine written in C++ 17.

It works on Windows and GNU/Linux.

It relies on Ashes as a rendering API, allowing Vulkan, Direct3D 11, and OpenGL.

The project itself is compilable on supported platforms using CMake (cf. [build](Build-English.md)).

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
- Scenes are described using a text format easily comprehensible and extensible. ([Scene File](SceneFile-English.md))
- Asynchronous (timers) or synchronour (thread) rendering.

## Implemented Plug-ins

### Importers
- ASSIMP: Multiple format mesh importer.
- PLY: Stanford Polygon library mesh importer.

### Dividers
- Loop subdivision surfaces.
- Phong tessellation.
- PN-Triangles surfaces.

### PostEffects
- Bloom : HDR Bloom implementation.
- DrawEdges : Detects and renders edges, based on normal, depth, and or object ID.
- FilmGrain : To display some grain on the render.
- GrayScale.
- LightStreaks (using Kawase Light Streaks).
- FXAA Antialiasing.
- SMAA Antialiasing (1X and T2X so far).
- Linear Motion Blur.

### Generators
- DiamondSquareTerrain : to generate terrains inside Castor3D scenes, using diamond-quare algorithm.

### Generic
- DiamondSquareTerrain: to generate terrains inside Castor3D scenes, using diamond-quare algorithm.
- ToonMaterial: A toon material (to be combined with DrawEdges plugin).
- FFTOceanRendering: Ocean rendering using FFT generated surfaces.
- OceanRendering: Basic ocean rendering, specifying waves attributes.
- WaterRendering: Plane water surfaces rendering, using normal maps.

### ToneMappings
- Linear.
- HaarmPieterDuiker.
- HejlBurgessDawson.
- Reinhard.
- Uncharted2.
- ACES.

## Other applications

- CastorViewer: A scene viewer based on Castor3D.
- CastorMeshConverter: A converter from various mesh files to Castor3D mesh format.
- CastorMeshUpgrader: Upgrades from earlier versions of Castor3D mesh format to the latest one.
- ImgConverter: Allows you to convert any image file type to XPM or ICO.

## Screenshots

![Sponza, VCT](http://dragonjoker.github.io/Castor3D/img/Sponza-PBR-VCT-Small.png){width=800px}
![Cerberus](http://dragonjoker.github.io/Castor3D/img/Cerberus-PBR-Small.png){width=800px}
![Park, Comparison](http://dragonjoker.github.io/Castor3D/img/Park-Small.png){width=800px}
![Nyra, PBR](http://dragonjoker.github.io/Castor3D/img/Nyra-PBR-MR-Small.png){width=800px}
![SanMiguel, PBR](http://dragonjoker.github.io/Castor3D/img/SanMiguel-PBR-SG-Small.png){width=800px}
![SanMiguel, PBR, LPV](http://dragonjoker.github.io/Castor3D/img/SanMiguel-PBR-SG-LPV-Small.png){width=800px}
![Bistro, PBR, VCT](http://dragonjoker.github.io/Castor3D/img/Bistro-PBR-VCT-Small.png){width=800px}

See [full changelog](ChangeLog-English.md) for more details.

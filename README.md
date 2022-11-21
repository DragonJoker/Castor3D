# Castor3D
Castor3D is a 3D engine written in C++ 20.
It works on Windows and GNU/Linux.
It supports Vulkan, and relies on Ashes to extend this support to Direct3D 11 and and OpenGL.
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
- Synchronous (user defined) or asynchronous (thread) rendering.
- Using Mesh and Task shaders, if available.

## Implemented Plug-ins

### Importers
- ASSIMP: Multiple format mesh importer.

### PostEffects
- Bloom: HDR Bloom implementation.
- PbrBloom : PBR Bloom implementation.
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
- FFTOceanRendering: Ocean rendering using FFT generated surfaces.
- OceanRendering: Basic ocean rendering, specifying waves attributes.
- WaterRendering: Plane water surfaces rendering, using normal maps.
- AtmosphereScattering : Sky and atmosphere rendering (using [Scalable and Production Ready Sky and Atmosphere Rendering Technique](https://sebh.github.io/publications/egsr2020.pdf)).

### ToneMappings
- Linear
- Haarm Pieter Duiker
- Hejl Burgess Dawson
- Reinhard
- Uncharted 2
- ACES

## Links

- [Building Castor3D](https://dragonjoker.github.io/Castor3D/pages/build)
- [API Documentation](https://dragonjoker.github.io/Castor3D/doc)
- [Playlist on YouTube](https://www.youtube.com/playlist?list=PLKA1SVXuAbMNaFbSJyAN_4yD2bzNlgES3)

## Screenshots

<a href="http://dragonjoker.github.io/Castor3D/img/Sponza-PBR-VCT.png"><img alt="Sponza" src="./img/Sponza-PBR-VCT-Small.png"></a>
<a href="http://dragonjoker.github.io/Castor3D/img/Cerberus-PBR.png"><img alt="Cerberus" src="./img/Cerberus-PBR-Small.png"></a>
<a href="http://dragonjoker.github.io/Castor3D/img/Park.png"><img alt="Park, Comparison" src="./img/Park-Small.png"></a>
<a href="http://dragonjoker.github.io/Castor3D/img/Nyra-PBR-MR.png"><img alt="Nyra, PBR" src="./img/Nyra-PBR-MR-Small.png"></a>
<a href="http://dragonjoker.github.io/Castor3D/img/SanMiguel-PBR-SG.png"><img alt="SanMiguel, PBR" src="./img/SanMiguel-PBR-SG-Small.png"></a>
<a href="http://dragonjoker.github.io/Castor3D/img/SanMiguel-PBR-SG-LPV.png"><img alt="SanMiguel, PBR, LPV" src="./img/SanMiguel-PBR-SG-LPV-Small.png"></a>
<a href="http://dragonjoker.github.io/Castor3D/img/Bistro-PBR-VCT.png"><img alt="Bistro, PBR, VCT" src="./img/Bistro-PBR-VCT-Small.png"></a>
<a href="http://dragonjoker.github.io/Castor3D/img/AtmosphereScattering-Terrain-Clouds.png"><img alt="AtmosphereScattering, Terrain, Clouds" src="./img/AtmosphereScattering-Terrain-Clouds-Small.png"></a>

## Older Versions

- [v0.13.0](https://dragonjoker.github.io/Castor3D/v0.13.0/)
- [v0.12.0](https://dragonjoker.github.io/Castor3D/v0.12.0/)
- [v0.11.0](https://dragonjoker.github.io/Castor3D/v0.11.0/)
- [v0.10.0](https://dragonjoker.github.io/Castor3D/v0.10.0/)
- [v0.9.0](https://dragonjoker.github.io/Castor3D/v0.9.0/)
- [v0.8.0](https://dragonjoker.github.io/Castor3D/v0.8.0/)
- [v0.7.0](https://dragonjoker.github.io/Castor3D/v0.7.0/)

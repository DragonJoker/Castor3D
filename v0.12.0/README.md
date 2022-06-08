[Back to Main page](http://dragonjoker.github.io/Castor3D)

# Castor3D v0.11.0
Castor3D is a 3D engine written in C++ 17.
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
- FFTOceanRendering: Ocean rendering using FFT generated surfaces.
- OceanRendering: Basic ocean rendering, specifying waves attributes.
- WaterRendering: Plane water surfaces rendering, using normal maps.

### ToneMappings
- Linear
- Haarm Pieter Duiker
- Hejl Burgess Dawson
- Reinhard
- Uncharted 2
- ACES

## Links
- [Building Castor3D](https://dragonjoker.github.io/Castor3D/v0.12.0/pages/build)
- [API Documentation](https://dragonjoker.github.io/Castor3D/v0.12.0/doc)

### Screenshots
<a href="./img/Sponza-PBR-VCT.png"><img alt="Sponza" src="./img/Sponza-PBR-VCT-Small.png"></a>
<a href="./img/Cerberus-PBR.png"><img alt="Cerberus" src="./img/Cerberus-PBR-Small.png"></a>
<a href="./img/Park.png"><img alt="Park, Comparison" src="./img/Park-Small.png"></a>
<a href="./img/Nyra-PBR-MR.png"><img alt="Nyra, PBR" src="./img/Nyra-PBR-MR-Small.png"></a>
<a href="./img/SanMiguel-PBR-SG.png"><img alt="SanMiguel, PBR" src="./img/SanMiguel-PBR-SG-Small.png"></a>
<a href="./img/SanMiguel-PBR-SG-LPV.png"><img alt="SanMiguel, PBR, LPV" src="./img/SanMiguel-PBR-SG-LPV-Small.png"></a>
<a href="./img/Bistro-PBR-VCT.png"><img alt="Bistro, PBR, VCT" src="./img/Bistro-PBR-VCT-Small.png"></a>

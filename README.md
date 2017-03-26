Castor3D
========

Castor3D is a 3D engine written in C++.
It works on Windows and n GNU/Linux.
It is still a work in progress, feel free to contribute!

Features
--------

- Normal mapping.
- Screen Space Ambient Occlusion.
- Shadow Mapping.
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
- ASSIMP: Multiple format mesh importer.
- FBX: Maya FBX scene importer.
- LWO: LightWave Object importer.
- ASE: ASCII Scene Export importer.
- MD2: Quake 2 Model importer.
- MD3: Quake 3 Model importer.
- 3DS: 3DSMax models importer.
- PLY: Stanford Polygon library mesh importer.
- OBJ: Wavefront OBJ mesh importer.

**Dividers**
- Loop subdivision surfaces.
- Phong tessellation.
- PN-Triangles surfaces.

**PostEffects**
- Bloom: HDR Bloom implementation.
- GrayScale.
- FXAA Antialiasing.

**Generic**
- CastorGUI: to build GUIs inside Castor3D scenes.

**ToneMappings**
- LinearToneMapping: Default tone mapping.
- HaarmPieterDuikerToneMapping: Haarm Pieter Duiker tone mapping.
- HejlBurgessDawsonToneMapping: Hejl Burgess Dawson tone mapping.
- ReinhardToneMapping: Reinhard tone mapping.

**Techniques**
- Forward: Default one, forward rendering.
- ForwardMsaa: Forward rendering with MSAA.
- Deferred: Deferred rendering technique using forward rendering for transparent meshes.
- DeferredMsaa: Deferred rendering technique using MSAA for transparent meshes.

Links
-----

- [Building Castor3D](https://dragonjoker.github.com/Castor3D/pages/build)
- [API Documentation](https://dragonjoker.github.com/Castor3D/doc/Castor3D/English)
- [Playlist on YouTube](https://www.youtube.com/playlist?list=PLKA1SVXuAbMNaFbSJyAN_4yD2bzNlgES3)

Older Versions
--------------

- [v0.8.0](https://dragonjoker.github.com/Castor3D/v0.8.0/)
- [v0.7.0](https://dragonjoker.github.com/Castor3D/v0.7.0/)

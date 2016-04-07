Castor3D
========

Castor3D is a 3D engine written in C++.
It works on Windows and n GNU/Linux.
It is still a work in progress, feel free to contribute!

Features
--------

- Normal mapping.
- Scene graph.
- Various rendering techniques : Forward, Forward SSAA, Forward MSAA.
- Modular architecture through plug-ins.
- Shaders are generated automatically from material and pass configuration.
- Shaders are writable directly from C++ code.

Implemented Plug-ins
--------------------

**Renderers**
- GlRenderSystem.

**Importers**
- ASSIMP: Multiple format mesh importer.
- LWO: LightWave Object importer.
- ASE: ASCII Scene Export importer.
- MD2: Quake 2 Model importer.
- MD3: Quake 3 Model importer.
- 3DS: 3DSMax models importer.
- PLY: Stanford Polygon library mesh importer.
- OBJ: Wavefront OBJ mesh importer.

**Dividers**
- Loop subdivision surfaces.
- PN-Triangles surfaces.

Links
-----

- [Building Castor3D](https://dragonjoker.github.com/Castor3D/v0.7.0/pages/build)
- [API Documentation](https://dragonjoker.github.com/Castor3D/v0.7.0/doc)
- [Playlist on YouTube](https://www.youtube.com/playlist?list=PLKA1SVXuAbMNaFbSJyAN_4yD2bzNlgES3)

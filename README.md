Travis build: [![Build Status](https://travis-ci.org/DragonJoker/Castor3D.svg?branch=master)](https://travis-ci.org/DragonJoker/Castor3D)

Castor3D
========

Castor3D is a 3D engine written in C++.
It works on Windows and also on GNU/Linux.
It uses OpenGL (with GLSL).

Building Castor3D
-----------------

Castor3D uses CMake to build project files.
You just need to launch CMake to build your preferential project file.

Tested compilers are:
- Visual Studio 2015
- gcc 5.3
- gcc 6.1

CastorUtils depends on FreeImage and FreeType, Zlib and libzip.
Castor3D is based on CastorUtils.
GuiCommon depends on wxWidgets (from 2.9.5, currently 3.0).
Last (but not least) CastorViewer depends on GuiCommon, Castor3D and wxWidgets.

Some plug-ins are also built :
Renderers:
- GlRenderSystem depends on OpenGL (not GLU).

Importers:
- ASE, 3DS, LWO, OBJ, MD2, MD3.
- ASSIMP: Depends on Assimp, replaces the previous ones.
- PLY: Stanford Polygon library mesh importer.
- OBJ: Wavefront OBJ mesh importer.

Dividers:
- Loop subdivision surfaces.
- Phong tessellation.
- PN-Triangles surfaces.

Techniques:
- Direct: Default one, nothing special about it.
- Deferred: Deferred rendering technique.
- MSAA: Multi-sampling anti aliasing
- DeferredMSAA: Deferred rendering technique using MSAA for transparent meshes.

PostEffects:
- Bloom.
- GrayScale.
- FXAA.

Generic:
- CastorGUI: to build GUIs inside Castor3D scenes.

ToneMappings:
- LinearToneMapping: Default tone mapping.
- HaarmPieterDuikerToneMapping: Haarm Pieter Duiker tone mapping.
- HejlBurgessDawsonToneMapping: Hejl Burgess Dawson tone mapping.
- ReinhardToneMapping: Reinhard tone mapping.

Two additional projects are also generated:
- ImgConverter : Allows you to convert any image file type to XPM or ICO.
- CastorUtilsTest : Allows you to run unit and performance tests for
    CastorUtils.
- Castor3DTest : Allows you to run unit tests for Castor3D.

Documentation for CastorUtils and Castor3D can be generated using Doxygen.

Options
-------

On GNU/Linux the default install directory is in /usr/local.
You can change this directory by changing the CMAKE_INSTALL_PREFIX value.
The sample, tool and test applications are installed in <install_dir>/bin/
The main libraries (CastorUtils and Castor3D) are installed in
<install_dir>/lib/ on GNU/Linux, and in <install_dir>/bin/ on Windows.
The plug-ins are installed in <install_dir>/lib/Castor3D/

The build is configurable through CMake using the following options:
- **PROJECTS_BINARIES_OUTPUT_DIR**

    Used to specify the root directory for compiled binaries.
    Inside this folder, the bin, lib, and share directories will be created,
    and binaries will be put in the right folders.
    Some files will also be copied in share directory, that are used by the
    projects.
    On Windows, the external DLLs will be copied in bin folder, if they are
    found.

- **PROJECTS_USE_PRECOMPILED_HEADERS**

    Used to enable/disable the use of Precompiled Headers for the compilation.

- **PROJECTS_USE_PRETTY_PRINTING**

    If it is enabled, and if A-Style is found, new test projects will be
    created, allowing the run of A-Style over each project's source file.

- **CASTOR_BUILD_CASTOR3D**

    This special option allows you to disable build of Castor3D, and every
    other project depending on it, resulting in the build of CastorUtils and
    CastorUtilsTest only.

- **CASTOR_BUILD_PLUGINS**

    Another special option allowing you to disable the build of plug-ins.

- **CASTOR_BUILDGRP_DIVIDER**

    Enables the build of Divider plug-ins.

- **CASTOR_BUILDGRP_GENERIC**

    Enables the build of Generic plug-ins.

- **CASTOR_BUILDGRP_IMPORTER**

    Enables the build of Importer plug-ins.

- **CASTOR_BUILDGRP_INTEROP**

    Enables the build of Bindings.

- **CASTOR_BUILDGRP_POSTFX**

    Enables the build of PostEffect plug-ins.

- **CASTOR_BUILDGRP_SAMPLE**

    Enables the build of sample applications.

- **CASTOR_BUILDGRP_SETUP**

    Allows you to build setup projects.

- **CASTOR_BUILDGRP_TECHNIQUE**

    Allows you to build Technique plug-ins (note that DirectRenderTechnique is
    always built).

- **CASTOR_BUILDGRP_TEST**

    Allows you to build test applications.

- **CASTOR_BUILDGRP_TONEMAPS**

    Allows you to build ToneMapping plug-ins (note that LinearToneMapping is
    always built).

- **CASTOR_BUILDGRP_TOOL**

    Enables the build of tools ()applications and tests).

- **CASTOR_BUILD_DIVIDER_<NAME>**

    Enables the build of <Name> divider plug-in.

- **CASTOR_BUILD_GENERIC_<NAME>**

    Enables the build of <Name> generic plug-in.

- **CASTOR_BUILD_IMPORTER_<NAME>**
    Enables the build of <Name> importer plug-in.

- **CASTOR_BUILD_INTEROP_<NAME>**

    Enables the build of <Name> binding.

- **CASTOR_BUILD_POSTFX_<NAME>**

    Enables the build of <Name> post-effect plug-in.

- **CASTOR_BUILD_RENDERER_<NAME>**

    Enables the build of <Name> renderer plug-in.

- **CASTOR_BUILD_SAMPLE_<NAME>**

    Enables the build of <Name> sample application.

- **CASTOR_BUILD_TECHNIQUE_<NAME>**

    Enables the build of <Name> technique plug-in.

- **CASTOR_BUILD_TONEMAP_<NAME>**

    Enables the build of <Name> tone mapping plug-in.

- **CASTOR_BUILD_TEST_<NAME>**

    Enables the build of <Name> test application.

- **CASTOR_FORCE_ALL_IMPORTERS**

    Forces the build of every importer plug-in, even though AssimpImporter is
    built.

- **CASTOR_USE_SSE2**

    Enables use of SSE2 instructions for Point4f and Matrix4x4f operations.

Running Castor3D
----------------

To run Castor Viewer, just type 'CastorViewer' in your favourite command-line
tool.

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

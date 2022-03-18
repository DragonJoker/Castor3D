# Building Castor3D

Castor3D uses CMake to build project files.

You just need to launch CMake to build your preferential project file.
  
Tested compilers are:
- Visual Studio 2022
- gcc 10.0
- clang 12.0

## Projects

### Main projects

- CastorUtils depends on Zlib and Freetype.
- Castor3D is based on CastorUtils, Ashes and ShaderWriter.
- GuiCommon depends on Castor3D and wxWidgets (from 2.9.5, currently 3.1).
- CastorViewer depends on GuiCommon and wxWidgets.

### Plug-ins

#### Importers

  - ASSIMP: Depends on Castor3D and Assimp.
  - PLY: Depend on Castor3D.

#### Dividers

  - Loop: Depend on Castor3D.
  - Phong: Depend on Castor3D.
  - PN-Triangles: Depend on Castor3D.

#### PostEffects

  - Bloom: Depend on Castor3D.
  - DrawEdges: Depend on Castor3D.
  - FilmGrain: Depend on Castor3D.
  - FXAA: Depend on Castor3D.
  - GrayScale: Depend on Castor3D.
  - LightStreaks: Depend on Castor3D.
  - LinearMotionBlur: Depend on Castor3D.
  - SMAA: Depend on Castor3D.

#### Generators

  - DiamondSquareTerrain: Depend on Castor3D.

#### Generic

  - CastorGUI: Depend on Castor3D.
  - FFTOceanRendering: Depend on Castor3D.
  - OceanRendering: Depend on Castor3D.
  - ToonMaterial: Depend on Castor3D.
  - WaterRendering: Depend on Castor3D.

#### Particles

  - FireworksParticle: Depend on Castor3D

#### ToneMappings

  - ACESToneMapping: Depend on Castor3D.
  - LinearToneMapping: Depend on Castor3D.
  - HaarmPieterDuikerToneMapping: Depend on Castor3D.
  - HejlBurgessDawsonToneMapping: Depend on Castor3D.
  - ReinhardToneMapping: Depend on Castor3D.
  - Uncharted2ToneMapping: Depend on Castor3D.

### Test projects

- CastorTest: Base library for testing, depends on CastorUtils.
- CastorUtilsTest: Allows you to run unit and performance tests for CastorUtils, depends on CastorUtils and CastorTest.
- Castor3DTest: Allows you to run unit tests for Castor3D, depends on Castor3D and CastorTest.

### Other

- CastorMeshConverter: Depends on Castor3D and wxWidgets.
- CastorMeshUpgrader: Depends on Castor3D and wxWidgets.
- CastorTestLauncher: Depends on Castor3D.
- CastorViewer: Depends on Castor3D and wxWidgets.
- ImgConverter: Depends on wxWidgets.
- GuiCommon: Depends on Castor3D and wxWidgets.
- HeightMapToNormalMap: Depends on Castor3D and wxWidgets.

Documentation for CastorUtils and Castor3D can be generated using Doxygen.

## Options

On GNU/Linux the default install directory is in /usr/local.

You can change this directory by changing the CMAKE_INSTALL_PREFIX value.

The sample, tool and test applications are installed in install_dir/bin/

The main libraries (CastorUtils and Castor3D) are installed in install_dir/lib/ on GNU/Linux, and in install_dir/bin/ on Windows.

The plug-ins are installed in install_dir/lib/Castor3D/

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

- **CASTOR_BUILDGRP_GENERATOR**

    Enables the build of Generator plug-ins.

- **CASTOR_BUILDGRP_GENERIC**

    Enables the build of Generic plug-ins.

- **CASTOR_BUILDGRP_IMPORTER**

    Enables the build of Importer plug-ins.

- **CASTOR_BUILDGRP_INTEROP**

    Enables the build of Bindings.

- **CASTOR_BUILDGRP_PARTICLES**

    Enables the build of Particles plug-ins.

- **CASTOR_BUILDGRP_POSTFX**

    Enables the build of PostEffect plug-ins.

- **CASTOR_BUILDGRP_SAMPLE**

    Enables the build of sample applications.

- **CASTOR_BUILDGRP_SETUP**

    Allows you to build setup projects.

- **CASTOR_BUILDGRP_TEST**

    Allows you to build test applications.

- **CASTOR_BUILDGRP_TONEMAPS**

    Allows you to build ToneMapping plug-ins (note that LinearToneMapping is always built).

- **CASTOR_BUILDGRP_TOOL**

    Enables the build of tools (applications and tests).

- **CASTOR_BUILD_DIVIDER_(NAME)**

    Enables the build of (NAME) divider plug-in.

- **CASTOR_BUILD_GENERATOR_(NAME)**

    Enables the build of (NAME) generator plug-in.

- **CASTOR_BUILD_GENERIC_(NAME)**

    Enables the build of (NAME) generic plug-in.

- **CASTOR_BUILD_IMPORTER_(NAME)**

    Enables the build of (NAME) importer plug-in.

- **CASTOR_BUILD_INTEROP_(NAME)**

    Enables the build of (NAME) binding.

- **CASTOR_BUILD_POSTFX_(NAME)**

    Enables the build of (NAME) post-effect plug-in.

- **CASTOR_BUILD_RENDERER_(NAME)**

    Enables the build of (NAME) renderer plug-in.

- **CASTOR_BUILD_SAMPLE_(NAME)**

    Enables the build of (NAME) technique plug-in.

- **CASTOR_BUILD_TONEMAP_(NAME)**

    Enables the build of (NAME) tone mapping plug-in.

- **CASTOR_BUILD_TEST_(NAME)**

    Enables the build of (NAME) test application.

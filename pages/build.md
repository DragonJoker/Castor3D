[Back to Main page](https://dragonjoker.github.io/Castor3D)

# Building Castor3D

Castor3D uses CMake to build project files.

You just need to launch CMake to build your preferential project file.

Projects generation relies on vcpkg to handle dependencies.<br />

Documentation for CastorUtils and Castor3D can be generated using Doxygen.

Tested compilers are:
- Visual Studio 2022
- gcc 12.0
- clang 14.0

## Options

The build is configurable through CMake using the following options:
- **PROJECTS_BINARIES_OUTPUT_DIR**

	Used to specify the root directory for compiled binaries.
	Inside this folder, the bin, lib, and share directories will be created,
	and binaries will be put in the right folders.
	Some files will also be copied in share directory, that are used by the
	projects.
	On Windows, the external DLLs will be copied in bin folder, if they are
	found.

- **PROJECTS_GENERATE_DOC**

	Used to enable creation of documentation generation projects.

- **PROJECTS_UNITY_BUILD**

	Used to enable/disable use of Jumbo/Unity builds for the compilation.

- **PROJECTS_USE_PRECOMPILED_HEADERS**

	Used to enable/disable the use of Precompiled Headers for the compilation.

- **PROJECTS_USE_PRETTY_PRINTING**

	If it is enabled, and if A-Style is found, new test projects will be
	created, allowing the run of A-Style over each project's source file.

- **PROJECTS_WARNINGS_AS_ERRORS**

	Used to treat warnings as errors, during compilation.

- **CASTOR_BUILD_CASTOR3D**

	This special option allows you to disable build of Castor3D, and every
	other project depending on it, resulting in the build of CastorUtils and
	CastorUtilsTest only.

- **CASTOR_BUILD_PLUGINS**

	Another special option allowing you to disable the build of plug-ins.

- **CASTOR_C3D_NEEDS_GLSL**

	Enables support of GLSL shaders import.

- **CASTOR_USE_FREEIMAGE**

	Enables use of FreeImage to read PNG images.

- **CASTOR_USE_GLSLANG**

	Enables support of GLSL shaders import.

- **CASTOR_USE_MESH_SHADERS**

	Enables use of mesh shaders.

- **CASTOR_USE_TASK_SHADERS**

	Enables use of task shaders.

- **CASTOR_BUILDGRP_DEMO**

	Enables the build of example applications.

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

- **CASTOR_BUILDGRP_SETUP**

	Allows you to build setup projects.

- **CASTOR_BUILDGRP_TEST**

	Allows you to build test applications.

- **CASTOR_BUILDGRP_TONEMAPS**

	Allows you to build ToneMapping plug-ins (note that LinearToneMapping is always built).

- **CASTOR_BUILDGRP_TOOL**

	Enables the build of tool applications.

- **CASTOR_BUILD_DEMO_(NAME)**

	Enables the build of (NAME) example application.

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

- **CASTOR_BUILD_TONEMAP_(NAME)**

	Enables the build of (NAME) tone mapping plug-in.

- **CASTOR_BUILD_TEST_(NAME)**

	Enables the build of (NAME) test application.

- **CASTOR_BUILD_TOOL_(NAME)**

	Enables the build of (NAME) tool application.

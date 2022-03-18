Change log	{#changelog}
==========

This page is here to inform about the changes since the earliest versions of the engine.

## Version 0.12.0

### Important Changes
- Fixed most warnings.
- Now using RenderGraph to build a frame graph.
- Now using bindless textures.
- Implemented MultiDrawIndirect.
- Implemented a loading screen.
- Implemented texture animations (tile maps and transforms).
- Implemented scene node animations.

### New features

#### Generic
- Added DrawEdges plugin, to detect and draw edges.
- Implemented basic ocean rendering plugin.
- Implemented FFT based ocean rendering plugin.
- Implemented water rendering.
- Implemented toon materials collection.

#### ToneMappings
- Added ACES tone mapping plugin.

#### Tools
- Added HeightMapToNormalMap and fixed normal maps generation.

### Modifications

#### CastorUtils
- Added support for tile maps.
- Added conversion from array map to tile map
- Added AsyncJobQueue, based on ThreadPool.
- Added GliImageWriter.
- Added optional FreeImageLoader, when FreeImage library is found.
- Added compression to BC1/BC3 to image loading, using cvtt.
- Created SpinMutex implementation
- TextWriter now writes to a castor::StringStream.
- Image loading now also generates mipmaps.
- Integrated additional parsers.
- Reworked FileParser to parse files in two steps.

#### Castor3D
- Added frustum culling for point lights shadow maps.
- Added support for multiple rendering devices.
- All post effects are now created in the render target, to be enabled or disabled at will.
- Allowed scene import through mesh importer.
- Added Blinn-Phong pass.
- Generalised shaders, using intermediate classes : LightingModel, ReflectionModel and LightMaterial.
- Added TextureCache, and using it to merge pass textures.
- Added a way to have non pickable nodes.
- Added rotate mesh options to importers.
- Added support for a secondary UV set.
- Added support for custom additional render technique passes.
- Added support for hull/domain shaders in scene render passes.
- Added support for passes without lighting.
- Added support for safe bands in Camera.
- Allowed textures components remapping on scene import.
- Depth prepass now also writes normals.
- Enabled LPV for point lights, and fixed spot lights'.
- Events can now be skipped.
- Merged PBRMR and PBRSG lighting models to a single one, internally.
- Objects can now be excluded from culling tests.
- Removed gamma correction for textures and materials, images use srgb formats when needed.
- Removed specific PassBuffers.
- Reworked Pass text writing and parsing.
- Moved SSS Profiles to their own shader buffer.

#### CastorGui
- Introduced style classes to handle that.

#### DiamondSquareTerrain
- Allowed random seed disabling (for testing).
- Now tiling UVs

#### AssimpImporter
- Improvements regarding materials.
- Improvements regarding animations processing.
- Added scene nodes and light sources import.

#### GuiCommon
- Added custom render passes configuration.
- PostEffects configuration has been added to the tree.
- Removed materials list and some duplicates in scene view.

### Fixed Bugs

#### CastorUtils
- Fixed Matrix2x2 and Matrix3x3 inversion.
- Fixed aligned alloc on win32.

#### Castor3D
- Fixed a crash when skinning keyframe is not found.
- Fixed background pass, regarding fog.
- Fixed interaction between indirect diffuse and reflections.
- Fixed crash with morphing that could happen on release builds
- Fixed import/export of morphing animations.
- Fixed indirect lighting handling in forward passes.
- Fixed point shadow maps.
- Fixed spot shadow maps.
- Fixed render loop cleanup events processing.
- Fixed render target creation, to have the right size and format.
- Fixed swapchain reset while RenderWindow is initialising.
- Fixed tone mapping switch.

### GuiCmmon
- Fixed a crash in LanguageFileParser.
- Fixed crash on light GI type change
- Fixed crashes when trying to load hdr images.
- Fixed texture properties display.

## Version 0.11.0

### Features

#### General

- Added support of vcpkg for external dependencies.

#### CastorUtils

- Reworked castor::Matrix and castor::Point to make them hold static data, and reduce memory footprint of Matrix.

#### Castor3D

- Castor3D now relies on <a href="https://github.com/DragonJoker/Ashes">Ashes</a>, allowing support for OpenGL, Direct3D11 and Vulkan renderers.
- GlslWriter has been completely removed and replaced by <a href="https://github.com/DragonJoker/ShaderWriter">ShaderWriter</a>, allowing the generation of shaders in the appropriate rendering API shader language.
- Textures now have a TextureConfiguration instead of a TextureChannel.
- Added support for cross skyboxes.
- Enabled anisotropic filtering control from scene file.
- Added Raw and PCF shadow maps choices.
- Added sendEvent function to Engine, to be able to execute an event directly if there is an enabled device.
- Extracted binary write/parse classes to new files, to reduce includes dependencies.
- Reworked mip level management for textures loaded from files.
- Added volumetric light scattering.
- MatrixUbo is now a reference, in RenderPass, hence is owned by owner classes.
- Upgraded scene files, meshes can now hold a default material.
- Render queue now uses SceneCuller from its render pass, and this culler is used for frustum culling.
- Cascaded shadow maps are implemented and functional.
- Overlays are now rendered them in their own framebuffer, and then combined with the 3D rendering.
- Shadow maps now use texture arrays (2D or Cube).
- Added Light Propagation Volumes global illumination implementation.
- Added Voxel Cone Tracing global illumination implementation.
- Implemented texture animations (UV rotate and translate).

#### RenderSystems

- They have been completely removed, thanks to Ashes.

#### PostEffects

- Created LinearMotionBlur post effect.

#### Samples

- Rehabilitated ShaderDialog, it now can be used to display tone mapping and post effect shaders.
- ShaderDialog now displays all the shaders for the pipeline used to display a pass.
- Selected submesh's bounding box now appears in yellow.

#### Tools

- Added CastorTestLauncher, which allows to run rendering tests through the wanted rendering API.
- Added Aria, which monitors rendering tests.
- CastorMeshUpgrader tool has been added, allowing to update cmsh files with a cl tool.
- CastorMeshConverter tool has been added, allowing to convert "any" mesh file to cmsh mesh with a cl tool.
- Added DiffImage, which allows image comparison between a reference image and another one (generated through CastorTestLauncher), to automatise rendering tests.

### Fixed Bugs

#### CastorUtils

- Fixed matrix initialisation from another different size matrix.
- Fixed pixel buffer flip.

#### OBJImporter

- Few fixes in face group management.

## Version 0.10.0

### Features

#### CastorUtils

- Renamed namespace Castor to castor.
- Added ChangeTracked and GroupChangeTracked classes, allowing to track changes to a member variable.
- Renamed CubeBox and SphereBox to BoundingBox and BoundingSphere.
- Splitted Colour in two classes: RgbColour and RgbaColour.
- PlaneEquation is no more a template class.
- Created a buddy allocator class: BuddyAllocator.

#### Castor3D

- Renamed namespace Castor3D to castor3d.
- Emissive in a material is now stored on a single floating point (emissive maps still hold a colour though).
- Implemented Physical Based Rendering, using both metallic/roughness and specular/glossiness workflows.
- RenderPipeline now also holds the shader program.
- Implemented Screen Space Subsurface Scattering.
- The light pass result is now stored in two render targets: diffuse and specular.
- Vertex buffers now use a buddy allocated pool.
- Shadow mapping now uses variance shadow maps.
- Transparent meshes are now rendered using Weighted Blended rendering.
- Screen Space Ambiant Occlusion has been improved, using Scalable Ambiant Obscurance implementation.
- Parallax Occlusion mapping is implemented.
- Reflection and refraction mapping have been implemented.
- Deferred rendering has been fully integrated, for opaque objects rendering.
- Created components for submeshes, to move functionalities out of Submesh.
- Rendering now uses a depth prepass.
- Created ColourSkybox, to handle background colour and skyboxes in the same way.
- Created ShaderBuffer class, to be able to handle SSBOs or TBOs in a similar way, choosing the most appropriate one, depending on running machine.

#### Generators

- Created DiamondSquareTerrain plugin.

#### Generic

- Events from CastorGUI are now forwarded to the user.

#### Importers

- Removed almost every importer plugin, keeping only Assimp, OBJ, and PLY importers.

#### RenderSystems

- Updated to support the features provided by Castor3D.

#### PostEffects

- Created SMAA post effect.
- Created Kawase Light Streaks post effect.
- Created Film Grain post effect.

#### Techniques

- Technique plugins have been completely removed.

#### Tone mappings

- Created Uncharted 2 tone mappings.

## Fixed Bugs
#### Castor3D

- Fixed bounding boxes for animated meshes and skinned meshes.
- Repaired an reenabled frustum culling.
- Fixed alpha rejection.

## Version 0.9.0

### Features

#### CastorUtils

- Dropped libzip, and added minizip to the source tree.

#### Castor3D

- Entire review of the render loop: RenderPipeline is no more a unique instance, but it actually contains the states, the program, and uniform buffer bindings.
  It has been done to prepare the arrival of Vulkan, in which the pipeline can't be modified once it has been created, and that is now the case in Castor3D too.
- The rendering has been splitted in RenderPasses, that each have one or more RenderQueues, that are updated asynchronously on CPU.
  This allows the GPU loop to be cleaned of most CPU computations.
- Skybox have been added.
- Binary export has been completely reviewed, it now uses a chunk data format (like LWO or 3DS).
  It is used for meshes, and skeletons.
- Plug-ins interface's consistency has been improved.
- Fog basic implementations (linear, exponential, and squared exponential) have been added.
- Morphing (per-vertex) animations are now supported by Castor3D.
- Frustum culling has been implemented.
- Colour picking has been implemented.
- Shadow mapping is implemented, lights can produce shadows, and objects can cast shadows.
- Particle system has been implemented, giving to the user the implementation choice (CPU through class override, GPU through Transform Feedback or Compute shaders).
- Fixed the lighting in GLSL, to make it behave properly without dirty hacks.
- Compute shaders have been integrated to Castor3D, along with Shader Storage Buffers and Atomic Counter Buffers.
- Textures implementation have been completely reviewed, we now have TextureLayout, which holds the TextureView(s), and the TextureStorage per image.
  It has allowed the creation of cube textures.
- Textures transfer to RAM has been fixed.
- Billboard rendering no more uses a geometry shader, it now relies on hardware instantiation of a quad, and the positions are given as side attributes.
- UniformBuffer (formerly FrameVariableBuffer) no longer depends on ShaderProgram.
- A new class, UniformBufferBinding has been created which depends on both UniformBuffer and ShaderProgram.
  Instances of this class are held by RenderPipeline.
- FrameVariable class has been split in two classes: Uniform (for uniform variables contained in a UniformBuffer) and PushUniform (for uniform variables out of a UniformBuffer).

#### GlRenderSystem

- Updated to support the features provided by Castor3D.

#### Techniques

- DeferredRenderTechnique and DeferredMsaaRenderTechnique are now real deferred rendering techniques, with a light pass per light, taking care of it's impact area.
- DeferredRenderTechnique is now the default render technique, when none is specified.

#### PostEffects

- Added GrayScale post effect.
- Added FXAA post effect.

## Version 0.8.0

### Features

#### General

- Compiles now with VS >= 2013 and GCC >= 4.9.
- Direct3D support has eventually been dropped, I don't have time for it.

#### CastorUtils

- Point4f and Matrix4x4f can now use SSE2 instructions for computations.
- All Castor projects are now using UTF-8 texts, using std::string.

#### Castor3D

- The skeleton animation system is now finished and working.
- Removed every XxxRenderer class, except for OverlayRenderer.
- The topology type is no longer in Camera, but in GeometryBuffers.
- Engine has been splitted, creating a RenderLoop class, and Managers to create elements, collect them, and retrieve them.
- SceneFileParser has been extended, to be able to use parsers from external plug-ins.
- The back frame buffers are retrieved and bindable like other FrameBuffers though you can't create or add attachments to it.
- Added a debug GPU objects tracking system, to ensure their destruction is made before context destruction.
- Can now read scenes from zip archive files.
- The text overlays now can have a horizontal and a vertical alignment.
- Added an initial zip data folder, Core.zip, which contains basic materials and debug overlays.
- The shader lighting has been reviewed and fixed.
- The Scene class has been reworked, to use ObjectManager classes, and ResourceManager views.
- Now, the ShaderProgram retrieves the vertex inputs layout. This layout is used in Submesh, to create the appropriate GeometryBuffers.
- Castor3D now renders the scenes using HDR and tone mapping. The default tone mapping configuration will make the render look like there were no HDR enabled.

####  GlRenderSystem

- Lights texture now uses Texture Buffer Objects.
- Removed every GlXxxRenderer class, except for GlOverlayRenderer.
- Support for OpenGL 1.x has been removed.
- Support for OpenGL 3.x has been enforced, forcing UBOs, for example.
- Implemented a ShaderWriter, to write GLSL code from C++ source.

#### Subdividers

- Implemented a Phong tessellation subdivider.

#### Importers

- Implemented a FBX importer plug-in.

#### Techniques

- Each render technique is now implemented in its own plug-in.

#### PostEffects

- The first post effect plug-in has been implemented: Bloom.

#### CastorGUI

- New plug-in, allowing creation of GUIs inside Castor3D scenes.

#### GuiCommon

- The whole look and feel has been reworked, using wxAuiManager.
- Properties are displayed through wxTreeCtrl and wxPropertyGrid, to list objects and view their properties.

### Fixed Bugs
 
#### CastorUtils

- Fixed transformation matrices.

#### Castor3D

- Fixed hardware instantiation, and instantiation, for meshes with different materials.
- Fixed a bug in border panel overlays material management.
- Fixed an overlays bug, on Intel GPU (was due to a real bug in AMD driver that hid the bug).
- The billboards are now fixed.

#### Subdividers

- Fixed PN Triangles subdivider, to use Bezier patches.
- Updated Loop subdivider.

#### GlRenderSystem

- Fixed a crash on Linux, with Intel GPUs.

## Version 0.7.0

### Features

#### General

- Supports x64 compilation
- Cleaned up a FrameBuffer, GlFrameBuffer and Dx11FrameBuffer.
- Linux and wxWidgets 2.8 compatibility improved.
- Added AStyle support.

#### CastorUtils

- Modified DynamicLibrary::getFunction, added a template parameter that is the function type, and a method parameter that is the pointer to the function to retrieve so no more casts when using DynamicLibrary::getFunction.
- Added log level support for Logger.
- Implemented intersection function in Line3D.
  - That class now has to named constructors : fromPointAndSlope and fromPoints, to avoid confusion between them.
  - Created two classes Size and Position instead of just having a typedef for these.
- Improved FileParser, now functions have parameters and not only parameters types, and parameters parse themselves.
- castor::Point now holds a static array as it's members instead of a dynamic one.
- Removed Unicode file encoding.
- Added stream indentation support.

#### Castor3D

- Modified shaders, they now take care of the shader model, they hold all defined files and sources for each model. The choice of the source is made at compile time, taking the source with the highest supported model.
- Improved memory occupation, limited overhead of Vertex (152 to 24 bytes) and Face (140 to 16 bytes).
- Modified Subdivision, now only in Subdivider, no more in Geometry, mesh or Submesh.
- Implemented Implemented Lights in texture.
- Added hasParameter and setParameter functions in ShaderObject, to apply matrices from RenderPipeline.
- Removed ApplyXXXMatrix from IPipelineImpl, now the RenderPipeline applies matrices itself.
- Added channel for textures (colour, ambient, diffuse, specular, normal, height).
- Implemented MSAA.
- Implemented Alpha to Coverage when MSAA is activated.
- Modified default sources in GlShaderProgram and Dx9ShaderProgram, they are now computed and no more external shader file is needed.
- Implemented Frame buffers.
- Submeshes are sorted by Material's alpha in order to render submeshes without alpha before others.
- Added functions Begin and End in Mesh class to be able to iterate through submeshes without using getSubmesh and getSubmeshCount.
- Added Material functions Begin and End to retrieve passes in standard way.
- Added possibility to activate or deactivate deferred rendering, this is also possible from cscn files.
- Created TechniqueParameters class to be able to pass generic parameters instead of, for example, the samples count.
- Introducing SamplerState class to group calls to setSamplerState and all relative stuff.
- Reviewed sizes a bit : Viewport holds the internal size, RenderTarget and RenderWindow hold external size. Modified  SceneFileParser, added &lt;size&gt; directive for viewport.
- Splitted eBUFFER_MODE in BufferAccessType and BufferAccessNature.
- Modified a bit Uniform in order to take care of eFRAME_VARIABLE_TYPE previously defined for SceneFileParser.
- Modified Submesh::AddUniforms so it takes a stVERTEX_GROUP structure as a parameter.
- Introducing Buffer to manage frame variables with OpenGl UBO or DirectX 11 Constants buffer.
- Implemented initialisation and cleanup of GPU side objects in two events : InitialiseEvent and CleanupEvent.
- Merged GpuBuffer::Initialise and GpuBuffer::SetShaderProgram functions to ease their use.
- Removed auto mipmap generation, now if user wants it, he does it with TextureLayout::GenerateMipmaps function.
- Added DepthStencilState class to manage those buffers states like Direct3D 11 (who does it well).
- Added blend state, implemented raster state.
- Put a BlendState instance in Pass and removed rgb/alpha blending from the pass.
- Removed Context::Enable and Context::Disable, removed RenderState class.
- Removed setPolygonMode from Context.
- Moved Render to buffer common code (from FrameBuffer and RenderWindow) into Context::BtoBRender.
- Implemented mesh instantiation on hardware.
- Modified Submesh normals generation.
- Moved materials from Submesh to Geometry.
- Modified submeshes instance count, to make it specific to the materials used by the submesh instances.

#### GuiCommon

- Now ShaderDialog uses wxSizers.
- Using wxStcEditor to view and edit shaders.
- Materials windows now use wxSizer classes to place and resize elements.
- Added possibility to change the material used by a submesh in wxGeometriesListFrame.
- Corrected few bugs in images loading.
- Improved StcTextEditor in order to have a better syntactic coloration.

#### CastorViewer

- Now MainFrame uses wxSizers.
- Added command line options : 
  - -f/--file to load a scene on startup
  - -h/--help to display help
  - -l/--level to define log level :
    - 0 for debug to errors
    - 1 for release to errors
    - 2 for warnings to errors
    - 3 for errors only.
- Now loads plug-ins in a thread.
- Added an "Export" button.

#### GlRenderSystem

- Removed GLEW dependency.
- Implemented OpenGL debug output so I have more verbose debug outputs for OpenGL too.
- Added GL_MIRRORED_REPEAT.
- Added GL_TEXTURE_MAX_ANISOTROPY.

### misc_0_7_0_gen General

- Splitted file to have one class per file.

#### CMake

- Updated UnitTest integration, Enabled CTest.

#### CastorUtils

- Removed all static members and globals.
- Removed String class, added string.
- Removed all dependencies to boost, except for thread, for non C++11 compilers.
- Modified Point, moved functions that shouldn't be in that class in a small helper class : point. It holds normalise, getNormalised, dot, all distance functions, cosTheta, negate.
- Modified FileParser in order to be able to ignore complete sections without commenting it (such as HLSL shader blocks in GlRenderSystem...).
- Modified FileParser to take care of parsers parameters.
- Reduced images memory occupation.
- Removed Point::link, Point::copy/clopy/clone functions.
- Replaced macros for cstrnlen, cstrftime, cstrncpy, cfopen, cfopen64, cfseek and cftell by functions in namespace castor.
- Modified Colour class, it doesn't extend Point4f anymore.
- Removed macro DECLARE_SHARED_PTR, now using  CU_DeclareSmartPtr, so I replaced each XXxPtr by it's equivalent XxxSPtr.

#### Castor3D

- Merged Submesh and SmoothingGroup classes.
- Reduced memory size of vertex buffers.
- Modified Submesh::computeTangents to take care of each vertex normal.
- Modified BufferElementGroup so it can't hold anymore its buffer.
- When initialising Pass, the texture units are re-ordered so the ones with a defined channel come first.
- Passes now initialise their shaders in Pass::Initialiser instead of trying to do it at each frame.
- Changed RenderTarget : now it only holds frame buffer and all needed for that. It is now a Renderable so TargetRenderer has been created.
- RenderWindow no longer extends RenderTarget, but now it has a RenderTarget as a private class member.
- Created Texture class, StaticTexture and TextureLayout to help implementing render targets.
- RenderTarget now uses RenderTechnique to make its rendering.
- Created GeometryBuffers class which holds a Vertex buffer and an Index buffer, they are created by the RenderSystem.
- Moved RenderTechnique basic frame buffer, texture and depth buffer into RenderTarget.
- Moved TargetRenderer::PostRender code into RenderWindow.
- Created WindowRenderer::EndScene function to prepare the render of the RenderTarget's frame buffer into the window.
- Removed calls to ShaderObject in order to make it more internal to Castor3D.
- Overlays are no more Renderable, there is one instance of OverlayRenderer class in the OverlayCache. All overlays will be rendered through this instance by the OverlayCache.
- Modified Scene, added template functions to add, remove or get an object whatever it's type may be (Light, Geometry, Camera, SceneNode).

#### GlRenderSystem

- Reviewed Vertex Array Objects, they now hold their buffers so the choice of using it or not is wrapped into it.
- Geometry shader work in OpenGL, created a shader to show wireframe models with Normals, Tangents and Bitangents.

### Fixed bugs

#### General

- Added Destroy functions for each create function in plug-ins, so creation and deletion is made in DLL memory space (for Windows).
- Added ASSIMP importer plug-in to import actually unsupported file types, modified ImporterPlugin::getExtensions to be able to retrieve an array of pairs &lt;extension, file="" type=""&gt;.
- Modified Importers so no more memory leaks are detected with Visual Studio.

#### CMake

- Added possibility to change binaries directory and also MSVC temporary binaries directory, through PROJECTS_BINARIES_OUTPUT_PATH and PROJECTS_BINTEMP_OUTPUT_PATH.
- Added possibility to activate code profiling with MSVC through PROJECTS_PROFILING boolean value.

#### CastorUtils

- Modified Image to make FreeImage load images by path and no more by handle (caused trouble with some formats).
- Repaired a matrix trouble when rendering overlays.
- Corrected recursive transformations.

#### Castor3D

- Repaired alpha submeshes that were not displayed.
- Corrected mipmaps generation.

#### CastorViewer

- Moved m_pCastor3D destruction and ImageLoader::Cleanup in the OnDestroy event handler.

#### GlRenderSystem

- Corrected a bug in doGetDeferredShaderSource.
- Corrected mipmaps generation in GlStaticTexture and GlDynamicTexture.
- Corrected the gloss map in GLSL computed programs.

#### ObjImporter

- Corrected materials and textures import.

## Version 0.6.1.2
### adds

- Created Dx9Renderer to render with Direct3D 9. Stills things to correct, in shaders and matrices, but it's almost done.
- Created different files for OpenGL contexts (X11, Gtk, Msw), X11 and Gtk still untested.
- Created a splash screen for CastorViewer and CastorShape, created a dialog box in order to choose the renderer at startup.
- Added a small app to converet from an image to a XPM.
- Added new features to scene files (alpha_rejection, rgb_blend, alpha_blend, hl_shader_program, ...)

### Modifications

- Optimised CMakeLists file and 'cmake_modules' files in order to take care of various configurations in a better way.
- Revised VertexBuffers and IndexBuffers, introducing VertexDeclaration and BufferElementGroup, so we can create VertexBuffersof customisable types.
- Vertex class now derives from BufferElementGroup.
- TextureEnvironment are going to disappear in the next version, now multitexturing management is done with passes and texture units.
- Suppressed GLU dependencies.

### Bugs

- Trouble with Direct3D matrices (maybe differences between right-handed / left-handed).
- Trouble with HLSL shaders, surely linked to previous trouble.
- Still troubles with Cg shaders (not worked on it).

## Version 0.6.1.1

### Features

- Created a CMakeLists file to generate the solution from CMake.
- Reorganised folders to make them more compliant with CMake.
- Splitted Plugin class in 3 parts : Renderer, Divider et Importer.
- Created one project per importer (OBJ, PLY, MD2, MD3, 3DS, ...)
- PNTrianglesDivider is left in Castor3D because it is sstill used by Icosahedron, LoopDivider is exported in a plug-in.
- Began Cg shaders implementation. This implies a scene file modification because in Cg, uniform variables are are linked to a shader instead of being linked to the whole program. Still buggy.

### Modifications

- Suppressed FreeImagePlus dependencies.
- Vertex, Attribute and Index buffers are now created by RenderSystem so they can be created out of the renderers.
- Suppressed Singletons for the Managers (Image, Font, Scene, Material, Animation and Mesh).
- Remerged SceneNode classes.
- Created IdPoint class, it is a point with just an index. Submes::addPoint creates now IdPoints and Submesh::addFace uses them to create the vertex.
- Subdividers can  subdivide in a thread in order not to freeze the main thread. It is possible to define a function to use at the end of the subdivision (cf. CastorShape).

### Corrections

- Corrected LoopDivider which didn't reposition the Vertex

## Version 0.6.1.0

### Features

- Using std or tr1 or boost smart pointers.
- Created OpenGlCommon for common treatments between GL2RenderSystem and GL3RenderSystem
- Created class RenderPipeline which performs the matrix computings and other functions (frustum, perspective, ortho, ...)

### Modifications

- Modified MemoryManager, added a template class MemoryTraced which overload new, delete new [] et delete [] operators in order to have better trace of allocations and deallocations. Each Castor class derive from this one and 3 files are created in root folder to log allocations, deallocations : memoryleaks.log, memoryalloc.log and memorydealloc.log
- TransformationUniform no longer exists, it is now a collection of functions which computes 4x4 matrix operations
- Modification SceneNode management : base class NodeBase holds the informations that were in SceneNode, 3 derived classes (GeometryMaterial, CameraMaterial et LightMaterial) take care of each category specificities.
- MovableObject class now has less features (name and NodeBase), Light and Camera now derive from it
- Renamed UniformVariable to .
- OpenGL 3.x/4.x now fully supported.
- Modified Vertex class in order to make them include texture coordinates and normals in order to have only 1 vertex buffer instead of 3 in a mesh.

### Corrections

- Corrected the shutdown crash.
- Corrected the black render with OpenGL 3.x

## Version 0.6.0.0

### Features

- Added a projection/normal and view matrices management class, in order to suppress the fixed pipeline support in OpenGl renderers.
- Added personnal smart pointers.
- Added an agle management class in order not to take care of degrees or radians.
- Added a path class, to manage slashes or backslashes according to OS.
- Created thread safe containers derived from STL ones (vector, list, map, multimap, set).
- Splitted GLRenderer in two parts : one compatible OpenGL 2.x and before (GL2RenderSystem), one compatible OpenGL3.x et after (GL3RenderSystem).
- GL2RenderSystem : according to client configuration, the fixed pipe is suppressed or keeped

### Modifications

- Modified thread management, leaving the user choosing between Win32, MFC or boost.
- Modified point and matrices classes.
- Modified String class which now is independant from MBCS or Unicode.
- Modified File class which now only uses streams.
- Modified the render system, added a class Renderable used to create associated renderer in order to ease the creation of derived classes (submesh, scene node, light, camera...)
- Changed scenes/materials/meshes files name (respectively cscn/cmtl/cmsh)

### Corrections

- Protected managers with recursive mutexes to avoid random crashes.

### Bugs

- Still a crash at shutdown, supposedly due to shaders.
- In GL3RenderSystem, I've got a black screen (?!?) but transformations seem to be correct.

## Version 0.5.0.1

### Features

- Tangent space is computed and available in GLSL shaders.
- Only tangent is computed, bitangent can be in the shader with a cross product.
- To access to the tangent you must declare the following attribute in the vertex shader :
- attribute vec3 tangent;

### Modifications

- The points classes have their members in a union so you can call it as you wish (xyz, rgb, left right top bottom)

### Corrections

- Corrected the crash on shutdown. It was due to VBOs which are now destroyed in a final render loop.




\~french
\page changelog Suivi de versions
Cette page sert à informer des évolutions du moteur depuis les premières versions.
## Version 0.11.0
### Fonctionnalités
#### Général

- Ajout du support de vcpkg pour les dépdendances externes.

#### CastorUtils

- castor::Matrix et castor::Point ont maintenant leurs données en statique, et l'empreinte mémoire de castor::Matrix a été réduite.

#### Castor3D

- Maintenant, Castor3D s'appuie sur Ashes, permettant le support de OpenGL, Direct3D11 et Vulkan.
- GlslWriter a été complètement supprimé et remplacé par <a href="https://github.com/DragonJoker/ShaderWriter">ShaderWriter</a>, permettant l'export des shaders dans le langage shader natif pour l'API de rendu choisie.
- Ajout du support des cross skyboxes.
- Le contrôle du filtrage anisotropique a été ajouté aux scene files.
- Ajout du choix du type de shadow map (Raw, PCF ou VSM).
- Ajout d'une fonction sendEvent à Engine, pour l'exécuter directement si un device est actif.
- Extraction des fonctions de lecture/écriture binaire dans de nouveaux fichiers, pour réduire les dépendances d'inclusion.
- Revue de la gestion des mipmaps pour les images chargées depuis un fichier.
- Ajout du volumetric light scattering.
- MatrixUbo est maintenant une référence, dans RenderPass, et est donc gérée par leur parent.
- Les meshes peuvent maintenant avoir un matériau par défaut, dans les scene files, permettant de réduire leur taille.
- RenderQueue utilise maintenant un SceneCuller, permettant de spécialiser le mode de culling voulu (aucun ou frustum, pour l'instant).
- Les Cascaded shadow maps sont implémentées pour les sources lumineuses directionnelles.
- Les incrustations sont maintenant dessinées dans leur propre framebuffer, puis celui-ci est combiné avec le rendu 3D.
- Les Shadow maps utilisent maintenant des texture arrays (2D ou Cube).
- Implémentation de global illumination via les Light Propagation Volumes.
- Implémentation de global illumination via le Voxel Cone Tracing.
- Implémentation des animations de texture (rotation et translation des UV).

#### Importers

- Quelques corrections dans l'importeur OBJ.

#### RenderSystems

- Tous les render systems ont été supprimés, grâce à Ashes.

#### PostEffects

- Création de l'effet LinearMotionBlur.

#### Samples

- Ajout de CastorTestLauncher, permettant les tests de rendu via l'API de rendu voulue.
- ShaderDialog a été réactivée, pouvant maintenant être utilisée pour afficher les shaders de tone mapping et de post effect.
- ShaderDialog affiche maintenant tous les shaders pour le pipeline utilisé dans l'affichage d'une passe.
- La bounding box du submesh sélectionné apparaît maintenant en jaune.

#### Tools

- Ajout de l'outil CastorMeshUpgrader, permettant la mise à jour des fichiers cmsh en ligne de commande.
- Ajout de l'outil CastorMeshConverter, permettant de convertir les fichiers mesh supportés en fichiers cmsh, en ligne de commande.
- Ajout de l'outil DiffImage, qui effectue la comparaison entre une image de référence et une autre image (générée via CastorTestLauncher), pour automatiser les tests de rendu.

## Fixed Bugs
#### CastorUtils

- Correction de l'initialisation d'une castor::Matrix depuis une matrice de taille différente.
- Correction du flip d'un pixel buffer.

## Version 0.10.0
### Fonctionnalités
#### CastorUtils

- Renommage du namespace Castor en castor.
- Ajout des classes ChangeTracked et GroupChangeTracked, permettant de tracer les changements de valeur d'une variable membre.
- Renommage de CubeBox et SphereBox en BoundingBox et BoundingSphere.
- Séparation de Colour en deux classes : RgbColour et RgbaColour.
- PlaneEquation n'est plus une classe template.
- Création d'un allocateur : buddy allocator.

#### Castor3D

- Renommage du namespace Castor3D en castor3d.
- La composante émissive des matériaux est maintenant stockée sur un simple flottant (les textures d'émissive sont cependant toujours en couleur)..
- Le Physical Based Rendering a été implémenté, en metallic/roughness et specular/glossiness.
- RenderPipeline contient maintenant le programme shader.
- Le Screen Space Subsurface Scattering a été implémenté (sans la transmission via les faces arrières, cependant).
- Le résultat de la passe d'éclairage est maintenant stocké dans deux cibles de rendu : une pour l'éclairage diffus, et une pour la réflexion spéculaire.
- Les tampons de sommets sont maintenant alloués via un pool utilisant un buddy allocator.
- Le shadow mapping utilise maintenant des variance shadow maps.
- Les maillages transparents sont maintenant dessinés en utilisant du Weighted Blended OIT.
- Le Screen Space Ambiant Occlusion a été amélioré, en implémentant le Scalable Ambiant Obscurance.
- Le Parallax Occlusion Mapping a été implémenté.
- Implémentation des reflection et refraction maps.
- Le rendu différé a été complètement intégré au rendu, pour le rendu des objets opaques.
- Création de compopsants pour les sous-maillages, afin d'alléger la classe Submesh, et rendre son remplissage plus flexible.
- Le rendu utilise maintenant un pré-passe de profondeur.
- Créé la classe ColourSkybox, afin de gérer la couleur de fond et les skybox de la même manière.
- Création d'une classe ShaderBuffer, permettant de manipuler les TBO et SSBO de la même manière, et de choisir le plus approprié en fonction du GPU.

#### Generators

- Création d'un plugin de génération de terrain en utilisant l'algorithme Diamond Square.

#### Generic

- Les évènements générés depuis CastorGUI sont maintenant envoyés à l'utilisateur.

#### Importers

- Suppression de la quasi totalité des plugins d'import, ne gardant que Assimp, OBJ et PLY.

#### PostEffects

- Création d'un plugin post rendu : SMAA, pour un antialiasing digne de ce nom.
- Créé un plugin implémentant l'effet Kawase Light Streaks.
- Création d'un plugin affichant un effet de grain filmique.

#### Techniques

- Suppression de tous les plugins de Technique.

#### Tone mappings

- Création de l'opérateur de tone mapping Uncharted 2.

## Corrections
#### Castor3D

- Correction des bounding boxes pour les maillages animés par verrtex, et par squelette.
- Réparation et réactivation du frustum culling.
- Réparation de l'alpha rejection.

## Version 0.9.0
### Fonctionnalités
#### CastorUtils

- Dropped libzip, and added minizip to the source tree.

#### Castor3D

- Entire review of the render loop: RenderPipeline is no more a unique instance, but it actually contains the states, the program, and uniform buffer bindings.
 It has been done to prepare the arrival of Vulkan, in which the pipeline can't be modified once it has been created, and that is now the case in Castor3D too.
- The rendering has been splitted in RenderPasses, that each have one or more RenderQueues, that are updated asynchronously on CPU.
 This allows the GPU loop to be cleaned of most CPU computations.
- Skybox have been added.
- Binary export has been completely reviewed, it now uses a chunk data format (like LWO or 3DS).
 It is used for meshes, and skeletons.
- Plug-ins interface's consistency has been improved.
- Fog basic implementations (linear, exponential, and squared exponential) have been added.
- Morphing (per-vertex) animations are now supported by Castor3D.
- Frustum culling has been implemented.
- Colour picking has been implemented.
- Shadow mapping is implemented, lights can produce shadows, and objects can cast shadows.
- Particle system has been implemented, giving to the user the implementation choice (CPU through class override, GPU through Transform Feedback or Compute shaders).
- Fixed the lighting in GLSL, to make it behave properly without dirty hacks.
- Compute shaders have been integrated to Castor3D, along with Shader Storage Buffers and Atomic Counter Buffers.
- Textures implementation have been completely reviewed, we now have TextureLayout, which holds the TextureView(s), and the TextureStorage per image.
 It has allowed the creation of cube textures.
- Textures transfer to RAM has been fixed.
- Billboard rendering no more uses a geometry shader, it now relies on hardware instantiation of a quad, and the positions are given as side attributes.
- UniformBuffer (formerly FrameVariableBuffer) no longer depends on ShaderProgram.
- A new class, UniformBufferBinding has been created which depends on both UniformBuffer and ShaderProgram.
 Instances of this class are held by RenderPipeline.
- FrameVariable class has been split in two classes: Uniform (for uniform variables contained in a UniformBuffer) and PushUniform (for uniform variables out of a UniformBuffer).

#### GlRenderSystem

- Updated to support the features provided by Castor3D.

#### Techniques

- DeferredRenderTechnique and DeferredMsaaRenderTechnique are now real deferred rendering techniques, with a light pass per light, taking care of it's impact area.
- DeferredRenderTechnique is now the default render technique, when none is specified.

#### PostEffects

- Added GrayScale post effect.
- Added FXAA post effect.

## Version 0.8.0
### Modifications
#### Général

- Compile maintenant avec VS >= 2013 et GCC >= 4.9.
- Le support de Direct3D a été abandonné, n'ayant pas le temps pour l'implémenter correctement.

#### CastorUtils

- Les opérations pour les Point4f et les Matrix4x4f peuvent maintenant utiliser les instructions SSE2.
- Tous les projets du Castor utilisent maintenant des textes UTF-8, stockés dans des std::string.

#### Castor3D

- Le système d'animations par squelette est maintenant finalisé et fonctionnel.
- Suppression de toutes les classes XxxRenderer, sauf OverlayRenderer.
- Le type de topologie de dessin n'est plus dans la classe Camera, mais dans GeometryBuffers.
- Engine a été retravaillée, en ajoutant des classes RenderLoop, ainsi que des Managers, s'occupant de la création des éléments, leur stockage et leur récupération.
- SceneFileParser a été étendue, pour pouvoir utiliser des directives d'analyse depuis des plug-ins externes.
- Les tampons d'image de la fenêtre sont maintenant récupérés, et liables comme les autres tampons d'image, il n'est cependant pas possible de leur créer ou ajouter des attaches.
- Ajout d'un système de traçage des objets GPU créés, pour s'assurer que leur destruction s'effectue avant la destruction de leur contexte.
- Les incrustations texte peuvent maintenant avoir un alignement vertical et/ou horizontal.
- Castor3D peut maintenant lire les scènes depuis des archives ZIP.
- Ajout d'une archive ZIP, Core.zip, contenant des matériaux basiques, ainsi que les incrustations de débogage.
- Cette archive est chargée lors de la sélection de l'API de rendu.
- Le code shader de l'éclairage a été revu et corrigé.
- La clase Scene a été retravaillée, pour utiliser la classe ObjectManager, ainsi que les vues sur les ResourceManagers.
- Maintenant ShaderProgram récupère la configuration des attributs de sommets en enrée du vertex shader.
- Cette configuration est utilisée par la classe Submesh, pour créer les GeometryBuffers qui vont bien.
- Castor3D utilise le High Dynamic Range pour dessiner les scènes. Le mappage de tons par défaut permet d'avoir un rendu similaire à un rendu sans HDR.

####  GlRenderSystem

- La texture de lumières utilise maintenant les Texture Buffer Objects.
- Suppression des classes GlXxxRenderer.
- Le support d'OpenGL 1.x a été supprimé.
- Le support d'OpenGL 3.x a été renforcé, en forçant l'utilisation des UBOs, par exemple.
- Implémentation de ShaderWriter, pour pouvoir écrire les shaders depuis un fichier source C++.

#### Subdiviseurs

- Implémentation d'un subdiviseur tessellation de Phong.
- Mise à jour du subdiviseur Loop.

#### Importeurs

- Implémentation d'un plug-in d'importation de fichiers FBX.

#### Techniques

- Chaque technique de rendu est maintenant implémentée dans son propre plug-in.

#### PostEffects

- Le premier plug-in d'effet post rendu a été implémenté: HDR Bloom.

#### CastorGUI

- Nouveau plug-in, permettant la création d'IHM au sein des scènes Castor3D.

#### GuiCommon

- L'aspect global des application avec IHM a été retravaillé, en utilisant wxAuiManager.
- Utilisation de wxTreeCtrl et des wxPropertyGrid, pour lister les objets, et visualiser leurs propriétés.

## Fixed Bugs
#### CastorUtils

- Les matrices de transformation ont été corrigées.

#### Castor3D

- Correction de l'instanciation GPU.
- Correction d'un bug dans la gestion des matériaux des incrustations panneaux bordurés.
- Correction d'un bug pour les incrustations, révélé sur les GPU Intel (était dû à un bug du driver AMD qui cachait ce bug).
- Correction des billboards.

#### Subdividers

- Correction du sudiviseur PN Triangles, en utilisant des patches Bézier.

#### GlRenderSystem

- Correction d'un crash sous Linux, avec GPU Intel.

## Version 0.7.0
### Modifications
#### CMake

- Ajout de la possibilité de changer le dossier des binaires, au travers des variables PROJECTS_BINARIES_OUTPUT_PATH et PROJECTS_BINTEMP_OUTPUT_PATH.
- Ajout du support d'AStyle, activé via l'option PROJECTS_USE_PRETTY_PRINTING.
- Utilisation de CPack pour générer le setup WiX (ou DEB), activé via l'option PROJECTS_PACKAGE_WIX (respectivement PROJECTS_PACKAGE_DEB).
- Ajout de la possibilité d'activer le profileur de code de Visual Studio, avec l'option PROJECTS_PROFILING.
- Ajout de la possibilité de choisir quels projets du Castor sont à générer (dans le groupe CASTOR).
- Réorganisation globale de l'arborescence des fichiers

#### Général

- Découpe des fichiers afin de n'avoir plus qu'une classe par fichier.
- Support de la compilation x64.
- Compabilité avec wxWidgets 2.8 et Linux améliorée.
- Ajout de fonctions Destroy pour chaque fonction create dans les plug-ins, afin que la création et la destruction de ceux-ci se fassent dans l'espace mémoire des DLL (pour Windows).
- Ajout du plug-in d'import ASSIMP, permettant d'élargir le nombre de formats supportés. La fonction ImporterPlugin::getExtensions permet maintenant de récupérer un tableau de paires &lt;extension, file type&gt;.

#### CastorUtils

- Toutes les variables statiques et globales ont été enlevées.
- Remplacement de la classe String par une classe string.
- Suppression des dépendances à boost, à part pour thread, et uniquement pour les compilateurs ne supportant pas std::thread.
- Les macros cstrnlen, cstrftime, cstrncpy, cfopen, cfopen64, cfseek et cftell ont été remplacées par des fonctions dans Castor.
- Suppression de la macro DECLARE_SHARED_PTR, remplacée par CU_DeclareSmartPtr, et les typedefs XxxPtr ont été remplacés par leurs équivalents XxxSPtr.
- Suppression de l'encodage Unicode pour les fichiers.
- Ajout du support de l'indentation de flux.
- Améliorations de castor::FileParser :

- Il peut ignorer des sections de code sans avoir à les commenter (tels que les blocs de shader HLSL alors que le RenderSystem chargé est GlRenderSystem, etc.).
- Il prend en compte les paramètres des fonctions d'analyse.
- Les fonctions peuvent maintenant avoir des paramètres, pas uniquement des types de paramètres, et ces paramètres s'analysent eux-mêmes.


- Réduction de l'occupation mémoire des images.
- Modification de castor::Point :

- les fonctions non dépendantes ont été déplacées dans une classe d'aide : castor::point. Cette classe contient donc les fonctions normalise, getNormalised, dot, cosTheta, negate et toutes les fonctions de calcul de distance.
- Suppression des méthodes link, copy/clopy/clone.
- Il contient maintenant un tableau statique au lieu d'un tableau dynamique.


- La classe castor::Colour ne dérive plus de Point4f.
- La fonction DynamicLibrary::getFunction a été modifiée, ajout d'un paramètre template, le type de fonction, afin de ne plus avoir de conversions lors de l'utilisation de DynamicLibrary::getFunction.
- Ajout du support des niveaux de log pour castor::Logger.
- Modifications de castor::Line3D :

- Implémentation de la méthode de calcul d'intersection
- Cette classe a maintenant deux constructeurs nommés : fromPointAndSlope et FromUniforms, pour éviter les confusions.


- Création de deux nouvelles classes : Size et Positionau lieu d'un typedef pour chacune.
- Création de la classe castor::Coord pour récupérer les fonctionnalités dynamiques de l'ancien castor::Point.

#### Castor3D

- Ajout des méthodes hasParameter et setParameter dans la classe ShaderObject, pour appliquer les matrices de la classe RenderPipeline.
- Ajout de fonctions Begin et End dans la classe Mesh afin de pouvoir itérer sur les submeshes sans passer par les fonctions getSubmesh et getSubmeshCount.
- Ajout de fonctions Begin et End dans la classe Material functions Begin and End.
- Ajout de la classe Buffer pour gérer les variables uniformes avec les UBO OpenGL ou les Constant buffer de Direct3D 11.
- Ajout de la classe DepthStencilState pour gérer ces états à la mode Direct3D 11 (qui fait ça bien).
- De même, ajout des classes BlendState et RasteriserState.
- Création des classes Texture, StaticTexture et TextureLayout pour faciliter l'implémentation des cibles de rendu.
- Ajout de canaux pour les textures (couleur, ambiante, diffusion, speculaire, normale, hauteur, brillance).
- Création de la classe TechniqueParameters pour passer des paramètres spécifiques aux techniques comme, par exemple, le nombre d'échantillons (pour le MSAA).
- Introduction de la classe SamplerState pour grouper les appels à setSamplerState et autres.
- Implémentation des Frame Buffers.
- Réduction de l'occupation mémoire des tampons de sommets.
- Modification de BufferElementGroup afin qu'il ne puisse plus contenir son tampon.
- La classe Overlay n'étend plus Renderable, il n'y a plus qu'une instance de OverlayRenderer, gérée par la classe OverlayCache. Toutes les incrustations sont maintenant rendues via cette instance.
- Amélioration de l'occupation mémoire : diminution drastique de la taille d'un Vertex (de 152 à 24 octets) et d'une Face (de 140 à 16 octets).
- Modification de Subdivision, elle se trouve maintenant uniquement dans Subdivider et plus dans Geometry, Mesh ou Submesh.
- Les lumières sont maintenant implémentées dans une texture, passant la limite de 8 à 100 sources.
- Suppression des méthodes ApplyXXXMatrix de la classe IPipelineImpl, maintenant la classe RenderPipeline applique les matrices elle-même.
- Implémentation du MSAA.
- Implémentation de l'Alpha to Coverage lorsque le MSAA est activé.
- Revue des tailles : Viewport contient la taille interne, RenderTarget et RenderWindow contiennent la taille externe.
- Découpe de eBUFFER_MODE en BufferAccessType et BufferAccessNature.
- L'initialisation des objets GPU se passe maintenant avec l'utilisation de deux évènements : InitialiseEvent et CleanupEvent.
- Fusion de GpuBuffer::Initialise et GpuBuffer::setShaderProgram afin d'en simplifier l'utilisation.
- Suppression de la génération automatique des mipmaps. Maintenant, si l'utilisateur vuet les générer, il utilise la fonction TextureLayout::GenerateMipmaps.
- Implémentation de l'instanciation hardware des maillages.
- Modifications de castor3d::Submesh :

- Les classes Submesh et SmoothingGroup ont été fusionnées.
- Modification de la méthode computeTangents pour prendre en compte les normales des sommets.
- Ajout d'un overload pour la méthode addUniforms afin de pouvoir lui donner un stVERTEX_GROUP en paramètre.
- Modification de la génération des normales.
- Déplacement des matériaux, de Submesh à Geometry.
- Modification du compte des instances, pour le rendre spécifique aux matériaux utilisés par les instances du Submesh.


- Modifications de castor3d::Pass :

- Création d'une fonction Pass::Initialise afin que les shaders ne soient initialisés qu'une fois, et pas à chaque frame.
- Réorganisation des unités de texture des passes, afin que celles associées à un canal arrivent premières.


- Ajout d'une instance de BlendState dans la classe Pass et suppression de la gestion du mélange RGB/Alpha de la classe Pass.
- Modifications de castor3d::Scene :

- Ajout de la possibilité d'activer ou désactiver le deferred rendering, option disponible dans les fichier cscn.
- Ajout de fonctions templates pour ajouter, supprimer ou récupérer un objet quel que soit son type (Light, Geometry, Camera, SceneNode).
- Les Submesh sont maintenant triés en fonction du matériau appliqué afin de pouvoir, entre autres, rendre les sous-maillages avec transparence après les autres.


- Modifications de castor3d::RenderTarget :

- Modifié RenderTarget : elle contient maintenant son frame buffer et ce qui s'y rattache. C'est maintenant un Renderable et un TargetRenderer a donc été créé.
- RenderTarget utilise maintenant RenderTechnique pour effectuer son rendu.


- Modifications de castor3d::RenderWindow :

- RenderWindow n'étend plus RenderTarget, mais en a un en membre privé.
- Création d'une méthode WindowRenderer::EndScene pour préparer le rendu du frame buffer de la RenderTarget dans la fenêtre.


- Modifications des shaders :

- Suppression des appels à ShaderObject afin d'en faire une classe interne à Castor3D.
- Modification des shaders, ils prennent maintenant en compte le modèle de shader, contiennent tous les fichiers et sources définis par modèle. Le choix du modèle est fait à la compilation, en choisissant le modèle le plus haut supporté.
- Modification des sources par défaut pour les shaders. Elles sont maitenant générées automatiquement et plus aucun fichier externe n'est nécessaire.
- Modification de  afin de prendre en compte eFRAME_VARIABLE_TYPE.


- Modifications de castor3d::Context

- Suppression des méthodes Context::Enable et Context::Disable, suppression de la classe RenderState.
- Suppression de la méthode Context::setPolygonMode.



#### GlRenderSystem

- Création de la classe GeometryBuffers pour gérer les Vertex Array Objects. Ils contiennent maintenant leurs tampons, permettant d'intégrer le choix d'utiliser les VAO ou non.
- Les Geometry shader fonctionnent dans GlRenderSystem.
- Suppression de la dépendance à GLEW.
- Support des extensions de debug d'OpenGL.
- Ajout du support de GL_MIRRORED_REPEAT.
- Ajout du support de GL_TEXTURE_MAX_ANISOTROPY.

#### GuiCommon

- Maintenant ShaderDialog utilise wxSizers.
- Utilisation de wxStcEditor pour l'édition de shaders.
- La fenêtre de matériaux utilise les wxSizers pour placer et redimensionner les éléments.
- Ajout de la possibilité de changer le matériau utilisé par un sous-maillage, avec la fenêtre wxGeometriesListFrame.
- Correction de bugs dans le chargement des images.
- Amélioration de StcTextEditor afin d'avoir une meilleure coloration syntaxique.

#### CastorViewer

- Maintenant, MainFrame utilise wxSizers.
- Ajout d'options de ligne de commande :

- -f/--file pour charger un fichier de scène au chargement
- -h/--help pour afficher une aide
- -l/--level pour définir le niveau de log :

- - 0 pour logger debug, infos, warnings et erreurs
- - 1 pour logger infos, warnings et erreurs
- - 2 pour logger warnings et erreurs
- - 3 pour logger les erreurs uniquement.

 


- Maintenant les plug-ins sont chargés dans un thread
- Ajout d'un bouton &quot;Export&quot;.

### Corrections
#### General

- Modification des importeurs afin de ne plus avoir de fuites de mémoires détectées par Visual Studio.

#### CastorUtils

- castor::Image a été modifiée, j'ai retiré la possibilité de charger les images par handle, car cela provoquait des problèmes avec FreeImage sur certains formats.
- Réparation d'un problème de matrices lors du rendu des incrustations.
- Correction des transformations récursives.

#### Castor3D

- Corrections des sous-maillages ayant un matériau transparent, car ils n'étaient pas affichés.
- Corrigé la génération de mipmaps.

#### CastorViewer

- Déplacement de la destruction de m_pCastor3D dans la méthode OnDestroy.

#### GlRenderSystem

- Correction d'un bug dans doGetDeferredShaderSource.
- Correction de la génération des mipmaps dans les classes GlStaticTexture et GlDynamicTexture.
- Correction du traitement des textures de brillance dans les programmes GLSL auto générés.

#### ObjImporter

- L'import des matériaux et textures a été corrigé.

## Version 0.6.1.2
### Ajouts

- Création du renderer Dx9Renderer, pour le rendu sous Direct3D 9. Il reste des choses à régler, notamment au niveau shaders et matrices, mais l'essentiel est là.
- Création de différents fichiers pour les contextes OpenGL (X11, Gtk, Msw), je n'ai pas encore pu tester ça correctement, mes drivers ATI ayant tué OpenGL sur ma Debian (snif...).
- Création d'une boîte de dialogue de sélection de renderer au lancement de CastorViewer, CastorShape et CastorArchitect, ajout d'un splash screen pour ces 3 applis.
- Ajout d'une petite appli qui transforme n'inmporte quel type d'image en XPM, plus facile pour le développement multiplateforme avec wxWidgets.
- Ajout de nouvelles fonctionnalités aux fichiers de scène (alpha_rejection, rgb_blend, alpha_blend, hl_shader_program, ...)

### Modifications

- Optimisation du fichier CMakeLists et des fichiers du dossier 'cmake_modules' de façon à mieux prendre en charge les différentes plateformes, ainsi que les différentes configurations des PC clients.
- Revue du système de VertexBuffers et IndexBuffers, in troduction de la notion de VertexDeclaration et BufferElementGroup, ainsi on peut créer des VertexBuffers en mettant à peu près ce que l'on veut comme données dedans (parmi vertex, normals, tangents, diffuse, texcoords0,1,2,3) et ordonnées comme on veut.
- La classe Vertex dérive donc maintenant de BufferElementGroup.
- Les TextureEnvironment vont disparaitre à la prochaine version, la gestion du multitexturing se faisant maintenant directement au niveau des passes et des texture units.
- Suppression des fonctions issues de GLU, afin d'avoir une bibliothèque de moins à linker.

### Bugs

- Problème avec les matrices pour DxRenderSystem (je pense à la différence right-handed / left-handed, mais mes premiers tests à ce niveau ne sont pas concluants).
- Problème avec les shaders HLSL, je n'ai pour l'instant pas d'affichage, je pense que c'est lié aux problèmes de matrices que je rencontre avec Direct3D.
- Je n'ai pas avancé sur les shaders Cg et ai donc toujours les mêmes problèmes qu'auparavant.

## Version 0.6.1.1
### Ajouts

- Création d'un fichier CMakeLists pour générer la solution à partir de CMake.
- Réorganisation des fichiers du dossier afin de mieux se prêter à ça.
- Séparation de la classe Plugin en 3 types différents : Renderer, Divider et Importer. Ainsi on pourra ajouter de nouveaux types de plug-in aisément et l'ajout d'importers et de subdiviseurs se fera sans devoir recompiler tous les projets.
- Ca entraîne la création d'un projet par importeur existant (OBJ, PLY, MD2, MD3, 3DS, ...)
- PNTrianglesDivider reste dans le Castor car utilisé par la primitive Icosahedron, LoopDivider est quand à lui placé dans un plug-in.
- J'ai commencé à implémenter les shaders Cg. Ca implique une modification des fichiers de scène, car dans Cg les variables uniform sont liées à un programme (vertex, pixel, geometry) plutôt qu'au shader complet comme c'est le cas en GLSL. Il reste quelques trucs à revoir (je ne sais pas encore quoi) pour que ça fonctionne.

### Modifications

- Suppression de FreeImagePlus, maintenant les projets ne nécessitent plus que FreeImage.
- La création des tampons utilisés pour le rendu (Vertex, Attribute, Texture, Normals, ...) se fait maintenant par le RenderSystem => on peut donc les créer dans Castor3D et pas seulement dans les renderer (GL2, GL3 et dans le futur D3D).
- Suppression des Singletons pour les différents Managers (Image, Font, Scene, Material, Animation et Mesh).
- J'ai retouché les SceneNodes, j'ai refusionné les 3 classes différentes en une seule pour des commodités d'utilisation.
- Création d'une classe IdPoint, qui étend Point et y ajoute un Index qui correspond à son index dans le submesh. La fonction addPoint de Submesh crée maintenant des IdPoint et la fonction addFace se sert d'eux pour générer les Vertex.
- Les subdiviseurs peuvent subdiviser dans un thread à part, permettant de ne pas figer une application pendant la subdivision (cf. CastorShape). Il est possible d'assigner une fonction qui sera lancée à la fin de la subdivision (cf. CastorShape).

### Corrections

- Correction du LoopDivider qui ne repositionnait plus les Vertex (les nouveaux étaient créés mais plus replacés, dû à la nouvelle architecture des Vertex)

## Version 0.6.1.0
### Ajouts

- Utilisation des smart pointers tr1.
- Création d'un tronc commun 'OpenGLCommon' pour les 2 renderers OpenGL
- Création d'une classe de RenderPipeline qui effectue les calculs matriciels et d'autres petites fonctions (perspective, ortho, ...)

### Modifications

- Le MemoryManager (activé uniquement en debug) a été modifié, ajout d'une classe template MemoryTraced qui surcharge les operateurs new, delete new [] et delete [], ce afin d'avoir une meilleure trace des objets créés et détruits. Chaque classe des projets du Castor dérivent maintenant de celle-ci, et 3 fichiers sont donc créés dans le répertoire racine (C:\ sous Windows) : memoryleaks.log, memoryalloc.log et memorydealloc.log
- La classe TransformationUniform n'existe plus, il ne s'agit plus que d'une collection de fonctions sur matrices carrées 4x4.
- Modification de la gestion des SceneNode : Une classe de base NodeBase qui contient les informations qui se trouvaient dans SceneNode, 3 classes dérivées (GeometryMaterial, CameraMaterial et LightMaterial) qui s'occupent des fonctionnalités spécifiques au rendu de chacun des types liés (respectivement Geometry, Camera et Light).
- La classe MovableObject a maintenant moins de fonctionnalités (en fait elle n'a plus qu'un nom et un NodeBase) et les classes Light et Camera dérivent maintenant de cette classe (pour plus d'uniformité dans les déplacements de ces objets par rapport aux géométries)
- Renommage des classes UniformVariable en , pour refléter plus ce que c'est et moins lier ce genre de choses à OpenGL.
- Le module OpenGL 3.x/4.x est achevé.
- Modifier la structure des vertex de façon à ce qu'ils incluent les coordonnées de texture et les normales directement, de façon à n'avoir qu'1 buffer de vertex par mesh au lieu de 3 actuellement.

### Corrections

- Correction du plantage à la fermeture du soft en release uniquement.
- Correction du rendu noir avec OpenGL 3.x, corrigé je ne sais trop comment, mais grâce à la suppression progressive du pipeline fixe en OpenGL 2.x

## Version 0.6.0.0
### Ajouts

- Ajout d'une classe de gestion des matrices de projection, normale et visualisation, en vue de la suppression du pipeline fixe dans les renderers OpenGL.
- Ajout de smart pointers persos, inspirés de ceux de boost.
- Ajout d'une classe de gestion des angles, permettant de ne pas s'embeter avec les conversions radian-degrés.
- Ajout d'une classe de gestion de path, permettant de gérer les slash ou anti-slash selon l'OS.
- Création de conteneurs thread-safe issus de la stl (vector, list, map, multimap, set).
- Séparation du renderer OpenGL en 2 projets : un compatible OpenGL 2.x et antérieur (GL2RenderSystem) le second compatible OpenGL3.x et ultérieur (GL3RenderSystem).
- GL2RenderSystem : en fonction du PC client, suppression du pipeline fixe ou pas (si l'utilisation des shaders est permise le pipeline fixe est supprimé, sinon il est gardé)

### Modifications

- Modification de la gestion des threads et mutexes, permettant le choix de ceux qu'on utilise (Win32, MFC ou boost).
- Modification des classes de gestion des matrices et points.
- Modification de la classe de gestion des chaînes de caractères qui sont maintenant indépendantes de Unicode ou MultiByte.
- Modification de la classe de gestion de fichiers qui ne permet plus le choix IO ou Stream mais utilise Stream uniquement.
- Modification du render system, ajout d'une classe renderable qui se charge de créer le renderer associé, afin de simplifier la création des dérivés de cette classe (submesh, scene node, light, camera...)
- Changement des noms des fichiers de scène/materiaux/meshes du moteur (respectivement cscn/cmtl/cmsh)

### Corrections

- Protection par recursive mutex des données partagées (par les managers, essentiellement) permettant d'éviter des plantages aléatoires.

### A faire

- Il reste un plantage à la fermeture du soft en release uniquement, apparemment dû aux shaders.
- Pour GL3RenderSystem, j'ai un rendu noir (?!?) mais les transformations sont prises en compte correctement.

## Version 0.5.0.1
### Ajouts

- Dans cette version l'espace tangent est calculé et est disponible dans les shaders GLSL.
- Seule la tangente est calculée, la bitangente pouvant l'être dans le shader avec un produit vectoriel entre la tangente et la normale.
- Pour accéder à la tangente, il suffit de déclarer l'attribut suivant dans le vertex shader :
- attribute vec3 tangent;

### Modifications

- Les différentes classes de point (2D, 3D, 4D) ont leur membres en union, ce qui permet de personnaliser l'appel de ces membres. Ainsi un point 2D a comme membres (x, y) ou (u, v). Un point 3D a comme membres (x, y, z) ou (u, v, w) ou (r, g, b). Un point 4D a comme membres (x, y, z, w) ou (r, g, b, a) ou (left, top, right, bottom).

### Corrections

- Correction du plantage à la fermeture de l'application lors de la suppression des VBOs. J'ai intégré un buffer manager qui les supprime pendant une boucle d'affichage.

/

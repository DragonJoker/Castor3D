/*!
 *\~english
 *\mainpage Castor3D Engine
 *\section intro_sec Change log
 *This page is here to inform about the changes since the earliest versions of the engine.
 *
 *\section version_0_8_0 Version 0.8.0
 *\subsection feat_0_8_0 Features
 *\subsubsection feat_0_8_0_gen General
 *<ul>
 *<li>Compiles now with VS >= 2013 and GCC >= 4.9.
 *<li>Direct3D support has eventually been dropped, I don't have time for it.</li>
 *</ul>
 *\subsubsection feat_0_8_0_cu CastorUtils
 *<ul>
 *<li>Point4f and Matrix4x4f can now use SSE2 instructions for computations.</li>
 *<li>All Castor projects are now using UTF-8 texts, using std::string.</li>
 *</ul>
 *\subsubsection feat_0_8_0_c3d Castor3D
 *<ul>
 *<li>The skeleton animation system is now finished and working.</li>
 *<li>Removed every XxxRenderer class, except for OverlayRenderer.</li>
 *<li>The topology type is no longer in Camera, but in GeometryBuffers.</li>
 *<li>Engine has been splitted, creating a RenderLoop class, and Managers to create elements, collect them, and retrieve them.</li>
 *<li>SceneFileParser has been extended, to be able to use parsers from external plug-ins.</li>
 *<li>The back frame buffers are retrieved and bindable like other FrameBuffers though you can't create or add attachments to it.</li>
 *<li>Added a debug GPU objects tracking system, to ensure their destruction is made before context destruction.</li>
 *<li>Can now read scenes from zip archive files.</li>
 *<li>The text overlays now can have a horizontal and a vertical alignment.</li>
 *<li>Added an initial zip data folder, Core.zip, which contains basic materials and debug overlays.</li>
 *<li>The shader lighting has been reviewed and fixed.</li>
 *<li>The Scene class has been reworked, to use ObjectManager classes, and ResourceManager views.</li>
 *<li>Now, the ShaderProgram retrieves the vertex inputs layout. This layout is used in Submesh, to create the appropriate GeometryBuffers.</li>
 *<li>Castor3D now renders the scenes using HDR and tone mapping. The default tone mapping configuration will make the render look like there were no HDR enabled.</li>
 *</ul>
 *\subsubsection feat_0_8_0_gl  GlRenderSystem
 *<ul>
 *<li>Lights texture now uses Texture Buffer Objects.</li>
 *<li>Removed every GlXxxRenderer class, except for GlOverlayRenderer.</li>
 *<li>Support for OpenGL 1.x has been removed.</li>
 *<li>Support for OpenGL 3.x has been enforced, forcing UBOs, for example.</li>
 *<li>Implemented a GlslWriter, to write GLSL code from C++ source.</li>
 *</ul>
 *\subsubsection feat_0_8_0_sub Subdividers
 *<ul>
 *<li>Implemented a Phong tessellation subdivider.</li>
 *</ul>
 *\subsubsection feat_0_8_0_imp Importers
 *<ul>
 *<li>Implemented a FBX importer plug-in.</li>
 *</ul>
 *\subsubsection feat_0_8_0_tec Techniques
 *<ul>
 *<li>Each render technique is now implemented in its own plug-in.</li>
 *</ul>
 *\subsubsection feat_0_8_0_pos PostEffects
 *<ul>
 *<li>The first post effect plug-in has been implemented: Bloom.</li>
 *</ul>
 *\subsubsection feat_0_8_0_cgui CastorGUI
 *<ul>
 *<li>New plug-in, allowing creation of GUIs inside Castor3D scenes.</li>
 *</ul>
 *\subsubsection feat_0_8_0_gui GuiCommon
 *<ul>
 *<li>The whole look and feel has been reworked, using wxAuiManager.</li>
 *<li>Properties are displayed through wxTreeCtrl and wxPropertyGrid, to list objects and view their properties.</li>
 *</ul>
 *
 \subsection bugs_0_8_0 Fixed Bugs
 *\subsubsection bugs_0_8_0_cu CastorUtils
 *<ul>
 *<li>Fixed transformation matrices.</li>
 *</ul>
 *\subsubsection bugs_0_8_0_c3d Castor3D
 *<ul>
 *<li>Fixed hardware instantiation, and instantiation, for meshes with different materials.</li>
 *<li>Fixed a bug in border panel overlays material management.</li>
 *<li>Fixed an overlays bug, on Intel GPU (was due to a real bug in AMD driver that hid the bug).</li>
 *<li>The billboards are now fixed.</li>
 *</ul>
 *\subsubsection bugs_0_8_0_sub Subdividers
 *<ul>
 *<li>Fixed PN Triangles subdivider, to use Bezier patches.</li>
 *<li>Updated Loop subdivider.</li>
 *</ul>
 *\subsubsection bugs_0_8_0_gl GlRenderSystem
 *<ul>
 *<li>Fixed a crash on Linux, with Intel GPUs.</li>
 *</ul>
 *
 *\section version_0_7_0 Version 0.7.0
 *
 *\subsection feat_0_7_0 Features
 *\subsubsection feat_0_7_0_gen General
 *<ul>
 *<li>Supports x64 compilation</li>
 *<li>Cleaned up a FrameBuffer, GlFrameBuffer and Dx11FrameBuffer.</li>
 *<li>Linux and wxWidgets 2.8 compatibility improved.</li>
 *<li>Added AStyle support.</li>
 *</ul>
 *\subsubsection feat_0_7_0_cu CastorUtils
 *<ul>
 *<li>Modified DynamicLibrary::GetFunction, added a template parameter that is the function type, and a method parameter that is the pointer to the function to retrieve so no more casts when using DynamicLibrary::GetFunction.</li>
 *<li>Added log level support for Logger.</li>
 *<li>Implemented intersection function in Line3D.</li>
 *<li>That class now has to named constructors : FromPointAndSlope and FromPoints, to avoid confusion between them.</li>
 *<li>Created two classes Size and Position instead of just having a typedef for these.</li>
 *<li>Improved FileParser, now functions have parameters and not only parameters types, and parameters parse themselves.</li>
 *<li>Castor::Point now holds a static array as it's members instead of a dynamic one.</li>
 *<li>Removed Unicode file encoding.</li>
 *<li>Added stream indentation support.</li>
 *</ul>
 *\subsubsection feat_0_7_0_c3d Castor3D
 *<ul>
 *<li>Modified shaders, they now take care of the shader model, they hold all defined files and sources for each model. The choice of the source is made at compile time, taking the source with the highest supported model.</li>
 *<li>Improved memory occupation, limited overhead of Vertex (152 to 24 bytes) and Face (140 to 16 bytes).</li>
 *<li>Modified Subdivision, now only in Subdivider, no more in Geometry, mesh or Submesh.</li>
 *<li>Implemented Implemented Lights in texture.</li>
 *<li>Added HasParameter and SetParameter functions in ShaderObject, to apply matrices from Pipeline.</li>
 *<li>Removed ApplyXXXMatrix from IPipelineImpl, now the Pipeline applies matrices itself.</li>
 *<li>Added channel for textures (colour, ambient, diffuse, specular, normal, height).</li>
 *<li>Implemented MSAA.</li>
 *<li>Implemented Alpha to Coverage when MSAA is activated.</li>
 *<li>Modified default sources in GlShaderProgram and Dx9ShaderProgram, they are now computed and no more external shader file is needed.</li>
 *<li>Implemented Frame buffers.</li>
 *<li>Submeshes are sorted by Material's alpha in order to render submeshes without alpha before others.</li>
 *<li>Added functions Begin and End in Mesh class to be able to iterate through submeshes without using GetSubmesh and GetSubmeshCount.</li>
 *<li>Added Material functions Begin and End to retrieve passes in standard way.</li>
 *<li>Added possibility to activate or deactivate deferred rendering, this is also possible from cscn files.</li>
 *<li>Created TechniqueParameters class to be able to pass generic parameters instead of, for example, the samples count.</li>
 *<li>Introducing SamplerState class to group calls to SetSamplerState and all relative stuff.</li>
 *<li>Reviewed sizes a bit : Viewport holds the internal size, RenderTarget and RenderWindow hold external size. Modified  SceneFileParser, added &lt;size&gt; directive for viewport.</li>
 *<li>Splitted eBUFFER_MODE in eBUFFER_ACCESS_TYPE and eBUFFER_ACCESS_NATURE.</li>
 *<li>Modified a bit FrameVariable in order to take care of eFRAME_VARIABLE_TYPE previously defined for SceneFileParser.</li>
 *<li>Modified Submesh::AddPoints so it takes a stVERTEX_GROUP structure as a parameter.</li>
 *<li>Introducing FrameVariableBuffer to manage frame variables with OpenGl UBO or DirectX 11 Constants buffer.</li>
 *<li>Implemented initialisation and cleanup of GPU side objects in two events : InitialiseEvent and CleanupEvent.</li>
 *<li>Merged GpuBuffer::Initialise and GpuBuffer::SetShaderProgram functions to ease their use.</li>
 *<li>Removed auto mipmap generation, now if user wants it, he does it with TextureLayout::GenerateMipmaps function.</li>
 *<li>Added DepthStencilState class to manage those buffers states like Direct3D 11 (who does it well).</li>
 *<li>Added blend state, implemented raster state.</li>
 *<li>Put a BlendState instance in Pass and removed rgb/alpha blending from the pass.</li>
 *<li>Removed Context::Enable and Context::Disable, removed RenderState class.</li>
 *<li>Removed SetPolygonMode from Context.</li>
 *<li>Moved Render to buffer common code (from FrameBuffer and RenderWindow) into Context::BtoBRender.</li>
 *<li>Implemented mesh instantiation on hardware.</li>
 *<li>Modified Submesh normals generation.</li>
 *<li>Moved materials from Submesh to Geometry.</li>
 *<li>Modified submeshes instance count, to make it specific to the materials used by the submesh instances.</li>
 *</ul>
 *\subsubsection feat_0_7_0_gui GuiCommon
 *<ul>
 *<li>Now ShaderDialog uses wxSizers.</li>
 *<li>Using wxStcEditor to view and edit shaders.</li>
 *<li>Materials windows now use wxSizer classes to place and resize elements.</li>
 *<li>Added possibility to change the material used by a submesh in wxGeometriesListFrame.</li>
 *<li>Corrected few bugs in images loading.</li>
 *<li>Improved StcTextEditor in order to have a better syntactic coloration.</li>
 *</ul>
 *\subsubsection feat_0_7_0_cview CastorViewer
 *<ul>
 *<li>Now MainFrame uses wxSizers.</li>
 *<li>Added command line options : <ul>
 *<li>-f/--file to load a scene on startup</li>
 *<li>-h/--help to display help</li>
 *<li>-l/--level to define log level :<ul>
 *<li>0 for debug to errors</li>
 *<li>1 for release to errors</li>
 *<li>2 for warnings to errors</li>
 *<li>3 for errors only.</li>
 *</ul>
 *</li>
 *</ul>
 *</li>
 *<li>Now loads plug-ins in a thread.</li>
 *<li>Added an "Export" button.</li>
 *</ul>
 *\subsubsection feat_0_7_0_gl GlRenderSystem
 *<ul>
 *<li>Removed GLEW dependency.</li>
 *<li>Implemented OpenGL debug output so I have more verbose debug outputs for OpenGL too.</li>
 *<li>Added GL_MIRRORED_REPEAT.</li>
 *<li>Added GL_TEXTURE_MAX_ANISOTROPY.</li>
 *</ul>
 *
 *\subsection Miscellaneous
 *\subsubsection misc_0_7_0_gen General
 *<ul>
 *<li>Splitted file to have one class per file.</li>
 *</ul>
 *\subsubsection misc_0_7_0_cmk CMake
 *<ul>
 *<li>Updated UnitTest integration, Enabled CTest.</li>
 *</ul>
 *\subsubsection misc_0_7_0_cu CastorUtils
 *<ul>
 *<li>Removed all static members and globals.</li>
 *<li>Removed String class, added string.</li>
 *<li>Removed all dependencies to boost, except for thread, for non C++11 compilers.</li>
 *<li>Modified Point, moved functions that shouldn't be in that class in a small helper class : point. It holds normalise, get_normalised, dot, all distance functions, cos_theta, negate.</li>
 *<li>Modified FileParser in order to be able to ignore complete sections without commenting it (such as HLSL shader blocks in GlRenderSystem...).</li>
 *<li>Modified FileParser to take care of parsers parameters.</li>
 *<li>Reduced images memory occupation.</li>
 *<li>Removed Point::link, Point::copy/clopy/clone functions.</li>
 *<li>Replaced macros for cstrnlen, cstrftime, cstrncpy, cfopen, cfopen64, cfseek and cftell by functions in namespace Castor.</li>
 *<li>Modified Colour class, it doesn't extend Point4f anymore.</li>
 *<li>Removed macro DECLARE_SHARED_PTR, now using  DECLARE_SMART_PTR, so I replaced each XXxPtr by it's equivalent XxxSPtr.</li>
 *</ul>
 *\subsubsection misc_0_7_0_c3d Castor3D
 *<ul>
 *<li>Merged Submesh and SmoothingGroup classes.</li>
 *<li>Reduced memory size of vertex buffers.</li>
 *<li>Modified Submesh::ComputeTangents to take care of each vertex normal.</li>
 *<li>Modified BufferElementGroup so it can't hold anymore its buffer.</li>
 *<li>When initialising Pass, the texture units are re-ordered so the ones with a defined channel come first.</li>
 *<li>Passes now initialise their shaders in Pass::Initialiser instead of trying to do it at each frame.</li>
 *<li>Changed RenderTarget : now it only holds frame buffer and all needed for that. It is now a Renderable so TargetRenderer has been created.</li>
 *<li>RenderWindow no longer extends RenderTarget, but now it has a RenderTarget as a private class member.</li>
 *<li>Created Texture class, StaticTexture and TextureLayout to help implementing render targets.</li>
 *<li>RenderTarget now uses RenderTechnique to make its rendering.</li>
 *<li>Created GeometryBuffers class which holds a Vertex buffer and an Index buffer, they are created by the RenderSystem.</li>
 *<li>Moved RenderTechnique basic frame buffer, texture and depth buffer into RenderTarget.</li>
 *<li>Moved TargetRenderer::PostRender code into RenderWindow.</li>
 *<li>Created WindowRenderer::EndScene function to prepare the render of the RenderTarget's frame buffer into the window.</li>
 *<li>Removed calls to ShaderObject in order to make it more internal to Castor3D.</li>
 *<li>Overlays are no more Renderable, there is one instance of OverlayRenderer class in the OverlayCache. All overlays will be rendered through this instance by the OverlayCache.</li>
 *<li>Modified Scene, added template functions to add, remove or get an object whatever it's type may be (Light, Geometry, Camera, SceneNode).</li>
 *</ul>
 *\subsubsection misc_0_7_0_gl GlRenderSystem
 *<ul>
 *<li>Reviewed Vertex Array Objects, they now hold their buffers so the choice of using it or not is wrapped into it.</li>
 *<li>Geometry shader work in OpenGL, created a shader to show wireframe models with Normals, Tangents and Bitangents.</li>
 *</ul>
 *
 *\subsection bugs_0_7_0 Fixed bugs
 *\subsubsection bugs_0_7_0_gen General
 *<ul>
 *<li>Added Destroy functions for each Create function in plug-ins, so creation and deletion is made in DLL memory space (for Windows).</li>
 *<li>Added ASSIMP importer plug-in to import actually unsupported file types, modified ImporterPlugin::GetExtensions to be able to retrieve an array of pairs &lt;extension, file="" type=""&gt;.</li>
 *<li>Modified Importers so no more memory leaks are detected with Visual Studio.</li>
 *</ul>
 *\subsubsection bugs_0_7_0_cmk CMake
 *<ul>
 *<li>Added possibility to change binaries directory and also MSVC temporary binaries directory, through PROJECTS_BINARIES_OUTPUT_PATH and PROJECTS_BINTEMP_OUTPUT_PATH.</li>
 *<li>Added possibility to activate code profiling with MSVC through PROJECTS_PROFILING boolean value.</li>
 *</ul>
 *\subsubsection bugs_0_7_0_cu CastorUtils
 *<ul>
 *<li>Modified Image to make FreeImage load images by path and no more by handle (caused trouble with some formats).</li>
 *<li>Repaired a matrix trouble when rendering overlays.</li>
 *<li>Corrected recursive transformations.</li>
 *</ul>
 *\subsubsection bugs_0_7_0_c3d Castor3D
 *<ul>
 *<li>Repaired alpha submeshes that were not displayed.</li>
 *<li>Corrected mipmaps generation.</li>
 *</ul>
 *\subsubsection bugs_0_7_0_cview CastorViewer
 *<ul>
 *<li>Moved m_pCastor3D destruction and ImageLoader::Cleanup in the OnDestroy event handler.</li>
 *</ul>
 *\subsubsection bugs_0_7_0_gl GlRenderSystem
 *<ul>
 *<li>Corrected a bug in DoGetDeferredShaderSource.</li>
 *<li>Corrected mipmaps generation in GlStaticTexture and GlDynamicTexture.</li>
 *<li>Corrected the gloss map in GLSL computed programs.</li>
 *</ul>
 *\subsubsection bugs_0_7_0_obj ObjImporter
 *<ul>
 *<li>Corrected materials and textures import.</li>
 *</ul>
 *
 *
 *\section version_0_6_1_2 Version 0.6.1.2
 *
 *\subsection adds_0_6_1_2 Adds
 *<ul>
 *<li>Created Dx9Renderer to render with Direct3D 9. Stills things to correct, in shaders and matrices, but it's almost done.</li>
 *<li>Created different files for OpenGL contexts (X11, Gtk, Msw), X11 and Gtk still untested.</li>
 *<li>Created a splash screen for CastorViewer and CastorShape, created a dialog box in order to choose the renderer at startup.</li>
 *<li>Added a small app to converet from an image to a XPM.</li>
 *<li>Added new features to scene files (alpha_rejection, rgb_blend, alpha_blend, hl_shader_program, ...)</li>
 *</ul>
 *
 *\subsection mods_0_6_1_2 Modifications
 *<ul>
 *<li>Optimised CMakeLists file and 'cmake_modules' files in order to take care of various configurations in a better way.</li>
 *<li>Revised VertexBuffers and IndexBuffers, introducing VertexDeclaration and BufferElementGroup, so we can create VertexBuffersof customisable types.</li>
 *<li>Vertex class now derives from BufferElementGroup.</li>
 *<li>TextureEnvironment are going to disappear in the next version, now multitexturing management is done with passes and texture units.</li>
 *<li>Suppressed GLU dependencies.</li>
 *</ul>
 *
 *\subsection bugs_0_6_1_2 Bugs
 *<ul>
 *<li>Trouble with Direct3D matrices (maybe differences between right-handed / left-handed).</li>
 *<li>Trouble with HLSL shaders, surely linked to previous trouble.</li>
 *<li>Still troubles with Cg shaders (not worked on it).</li>
 *</ul>
 *
 *\section version_0_6_1_1 Version 0.6.1.1
 *
 *\subsection adds_0_6_1_1 Adds
 *<ul>
 *<li>Created a CMakeLists file to generate the solution from CMake.</li>
 *<li>Reorganised folders to make them more compliant with CMake.</li>
 *<li>Splitted Plugin class in 3 parts : Renderer, Divider et Importer.</li>
 *<li>Created one project per importer (OBJ, PLY, MD2, MD3, 3DS, ...)</li>
 *<li>PNTrianglesDivider is left in Castor3D because it is sstill used by Icosahedron, LoopDivider is exported in a plug-in.</li>
 *<li>Began Cg shaders implementation. This implies a scene file modification because in Cg, uniform variables are are linked to a shader instead of being linked to the whole program. Still buggy.</li>
 *</ul>
 *
 *\subsection mods_0_6_1_1 Modifications
 *<ul>
 *<li>Suppressed FreeImagePlus dependencies.</li>
 *<li>Vertex, Attribute and Index buffers are now created by RenderSystem so they can be created out of the renderers.</li>
 *<li>Suppressed Singletons for the Managers (Image, Font, Scene, Material, Animation and Mesh).</li>
 *<li>Remerged SceneNode classes.</li>
 *<li>Created IdPoint class, it is a point with just an index. Submes::AddPoint creates now IdPoints and Submesh::AddFace uses them to create the vertex.</li>
 *<li>Subdividers can  subdivide in a thread in order not to freeze the main thread. It is possible to define a function to use at the end of the subdivision (cf. CastorShape).</li>
 *</ul>
 *
 *\subsection bugs_0_6_1_1 Corrections
 *<ul>
 *<li>Corrected LoopDivider which didn't reposition the Vertex</li>
 *</ul>
 *
 *\section version_0_6_1_0 Version 0.6.1.0
 *
 *\subsection adds_0_6_1_0 Adds
 *<ul>
 *<li>Using std or tr1 or boost smart pointers.</li>
 *<li>Created OpenGlCommon for common treatments between GL2RenderSystem and GL3RenderSystem</li>
 *<li>Created class Pipeline which performs the matrix computings and other functions (frustum, perspective, ortho, ...)</li>
 *</ul>
 *
 *\subsection mods_0_6_1_0 Modifications
 *<ul>
 *<li>Modified MemoryManager, added a template class MemoryTraced which overload new, delete new [] et delete [] operators in order to have better trace of allocations and deallocations. Each Castor class derive from this one and 3 files are created in root folder to log allocations, deallocations : memoryleaks.log, memoryalloc.log and memorydealloc.log</li>
 *<li>TransformationMatrix no longer exists, it is now a collection of functions which computes 4x4 matrix operations</li>
 *<li>Modification SceneNode management : base class NodeBase holds the informations that were in SceneNode, 3 derived classes (GeometryMaterial, CameraMaterial et LightMaterial) take care of each category specificities.</li>
 *<li>MovableObject class now has less features (name and NodeBase), Light and Camera now derive from it</li>
 *<li>Renamed UniformVariable to FrameVariable.</li>
 *<li>OpenGL 3.x/4.x now fully supported.</li>
 *<li>Modified Vertex class in order to make them include texture coordinates and normals in order to have only 1 vertex buffer instead of 3 in a mesh.</li>
 *</ul>
 *
 *\subsection bugs_0_6_1_0 Corrections
 *<ul>
 *<li>Corrected the shutdown crash.</li>
 *<li>Corrected the black render with OpenGL 3.x</li>
 *</ul>
 *
 *\section version_0_6_0_0 Version 0.6.0.0
 *
 *\subsection adds_0_6_0_0 Adds
 *<ul>
 *<li>Added a projection/normal and view matrices management class, in order to suppress the fixed pipeline support in OpenGl renderers.</li>
 *<li>Added personnal smart pointers.</li>
 *<li>Added an agle management class in order not to take care of degrees or radians.</li>
 *<li>Added a path class, to manage slashes or backslashes according to OS.</li>
 *<li>Created thread safe containers derived from STL ones (vector, list, map, multimap, set).</li>
 *<li>Splitted GLRenderer in two parts : one compatible OpenGL 2.x and before (GL2RenderSystem), one compatible OpenGL3.x et after (GL3RenderSystem).</li>
 *<li>GL2RenderSystem : according to client configuration, the fixed pipe is suppressed or keeped</li>
 *</ul>
 *
 *\subsection mods_0_6_0_0 Modifications
 *<ul>
 *<li>Modified thread management, leaving the user choosing between Win32, MFC or boost.</li>
 *<li>Modified point and matrices classes.</li>
 *<li>Modified String class which now is independant from MBCS or Unicode.</li>
 *<li>Modified File class which now only uses streams.</li>
 *<li>Modified the render system, added a class Renderable used to create associated renderer in order to ease the creation of derived classes (submesh, scene node, light, camera...)</li>
 *<li>Changed scenes/materials/meshes files name (respectively cscn/cmtl/cmsh)</li>
 *</ul>
 *
 *\subsection fix_0_6_0_0 Corrections
 *<ul>
 *<li>Protected managers with recursive mutexes to avoid random crashes.</li>
 *</ul>
 *
 *\subsection bugs_0_6_0_0 Bugs
 *<ul>
 *<li>Still a crash at shutdown, supposedly due to shaders.</li>
 *<li>In GL3RenderSystem, I've got a black screen (?!?) but transformations seem to be correct.</li>
 *</ul>
 *
 *\section version_0_5_0_1 Version 0.5.0.1
 *
 *\subsection adds_0_5_0_1 Adds
 *<ul>
 *<li>Tangent space is computed and available in GLSL shaders.</li>
 *<li>Only tangent is computed, bitangent can be in the shader with a cross product.</li>
 *<li>To access to the tangent you must declare the following attribute in the vertex shader :</li>
 *<li>attribute vec3 tangent;</li>
 *</ul>
 *
 *\subsection mods_0_5_0_1 Modifications
 *<ul>
 *<li>The points classes have their members in a union so you can call it as you wish (xyz, rgb, left right top bottom)</li>
 *</ul>
 *
 *\subsection bugs_0_5_0_1 Corrections
 *<ul>
 *<li>Corrected the crash on shutdown. It was due to VBOs which are now destroyed in a final render loop.</li>
 *</ul>
 *
 *
 *
 *\~french
 *\mainpage Moteur Castor3D
 *\section intro_sec Suivi de versions
 *Cette page sert à informer des évolutions du moteur depuis les premières versions.
 *
 *\section version_0_8_0 Version 0.8.0
 *\subsection feat_0_8_0 Modifications
 *\subsubsection feat_0_8_0_gen Général
 *<ul>
 *<li>Compile maintenant avec VS >= 2013 et GCC >= 4.9.</li>
 *<li>Le support de Direct3D a été abandonné, n'ayant pas le temps pour l'implémenter correctement.</li>
 *</ul>
 *\subsubsection feat_0_8_0_cu CastorUtils
 *<ul>
 *<li>Les opérations pour les Point4f et les Matrix4x4f peuvent maintenant utiliser les instructions SSE2.</li>
 *<li>Tous les projets du Castor utilisent maintenant des textes UTF-8, stockés dans des std::string.</li>
 *</ul>
 *\subsubsection feat_0_8_0_c3d Castor3D
 *<ul>
 *<li>Le système d'animations par squelette est maintenant finalisé et fonctionnel.</li>
 *<li>Suppression de toutes les classes XxxRenderer, sauf OverlayRenderer.</li>
 *<li>Le type de topologie de dessin n'est plus dans la classe Camera, mais dans GeometryBuffers.</li>
 *<li>Engine a été retravaillée, en ajoutant des classes RenderLoop, ainsi que des Managers, s'occupant de la création des éléments, leur stockage et leur récupération.</li>
 *<li>SceneFileParser a été étendue, pour pouvoir utiliser des directives d'analyse depuis des plug-ins externes.</li>
 *<li>Les tampons d'image de la fenêtre sont maintenant récupérés, et liables comme les autres tampons d'image, il n'est cependant pas possible de leur créer ou ajouter des attaches.</li>
 *<li>Ajout d'un système de traçage des objets GPU créés, pour s'assurer que leur destruction s'effectue avant la destruction de leur contexte.</li>
 *<li>Les incrustations texte peuvent maintenant avoir un alignement vertical et/ou horizontal.</li>
 *<li>Castor3D peut maintenant lire les scènes depuis des archives ZIP.</li>
 *<li>Ajout d'une archive ZIP, Core.zip, contenant des matériaux basiques, ainsi que les incrustations de débogage.</li>
 *<li>Cette archive est chargée lors de la sélection de l'API de rendu.</li>
 *<li>Le code shader de l'éclairage a été revu et corrigé.</li>
 *<li>La clase Scene a été retravaillée, pour utiliser la classe ObjectManager, ainsi que les vues sur les ResourceManagers.</li>
 *<li>Maintenant ShaderProgram récupère la configuration des attributs de sommets en enrée du vertex shader.</li>
 *<li>Cette configuration est utilisée par la classe Submesh, pour créer les GeometryBuffers qui vont bien.</li>
 *<li>Castor3D utilise le High Dynamic Range pour dessiner les scènes. Le mappage de tons par défaut permet d'avoir un rendu similaire à un rendu sans HDR.</li>
 *</ul>
 *\subsubsection feat_0_8_0_gl  GlRenderSystem
 *<ul>
 *<li>La texture de lumières utilise maintenant les Texture Buffer Objects.</li>
 *<li>Suppression des classes GlXxxRenderer.</li>
 *<li>Le support d'OpenGL 1.x a été supprimé.</li>
 *<li>Le support d'OpenGL 3.x a été renforcé, en forçant l'utilisation des UBOs, par exemple.</li>
 *<li>Implémentation de GlslWriter, pour pouvoir écrire les shaders depuis un fichier source C++.</li>
 *</ul>
 *\subsubsection feat_0_8_0_sub Subdiviseurs
 *<ul>
 *<li>Implémentation d'un subdiviseur tessellation de Phong.</li>
 *<li>Mise à jour du subdiviseur Loop.</li>
 *</ul>
 *\subsubsection feat_0_8_0_imp Importeurs
 *<ul>
 *<li>Implémentation d'un plug-in d'importation de fichiers FBX.</li>
 *</ul>
 *\subsubsection feat_0_8_0_tec Techniques
 *<ul>
 *<li>Chaque technique de rendu est maintenant implémentée dans son propre plug-in.</li>
 *</ul>
 *\subsubsection feat_0_8_0_pos PostEffects
 *<ul>
 *<li>Le premier plug-in d'effet post rendu a été implémenté: HDR Bloom.</li>
 *</ul>
 *\subsubsection feat_0_8_0_cgui CastorGUI
 *<ul>
 *<li>Nouveau plug-in, permettant la création d'IHM au sein des scènes Castor3D.</li>
 *</ul>
 *\subsubsection feat_0_8_0_gui GuiCommon
 *<ul>
 *<li>L'aspect global des application avec IHM a été retravaillé, en utilisant wxAuiManager.</li>
 *<li>Utilisation de wxTreeCtrl et des wxPropertyGrid, pour lister les objets, et visualiser leurs propriétés.</li>
 *</ul>
 *
 \subsection bugs_0_8_0 Fixed Bugs
 *\subsubsection bugs_0_8_0_cu CastorUtils
 *<ul>
 *<li>Les matrices de transformation ont été corrigées.</li>
 *</ul>
 *\subsubsection bugs_0_8_0_c3d Castor3D
 *<ul>
 *<li>Correction de l'instanciation GPU.</li>
 *<li>Correction d'un bug dans la gestion des matériaux des incrustations panneaux bordurés.</li>
 *<li>Correction d'un bug pour les incrustations, révélé sur les GPU Intel (était dû à un bug du driver AMD qui cachait ce bug).</li>
 *<li>Correction des billboards.</li>
 *</ul>
 *\subsubsection bugs_0_8_0_sub Subdividers
 *<ul>
 *<li>Correction du sudiviseur PN Triangles, en utilisant des patches Bézier.</li>
 *</ul>
 *\subsubsection bugs_0_8_0_gl GlRenderSystem
 *<ul>
 *<li>Correction d'un crash sous Linux, avec GPU Intel.</li>
 *</ul>
 *
 *\section version_0_7_0 Version 0.7.0
 *
 *\subsection mods_0_7_0 Modifications
 *\subsubsection mods_0_7_0_cmk CMake
 *<ul>
 *<li>Ajout de la possibilité de changer le dossier des binaires, au travers des variables PROJECTS_BINARIES_OUTPUT_PATH et PROJECTS_BINTEMP_OUTPUT_PATH.</li>
 *<li>Ajout du support d'AStyle, activé via l'option PROJECTS_USE_PRETTY_PRINTING.</li>
 *<li>Utilisation de CPack pour générer le setup WiX (ou DEB), activé via l'option PROJECTS_PACKAGE_WIX (respectivement PROJECTS_PACKAGE_DEB).</li>
 *<li>Ajout de la possibilité d'activer le profileur de code de Visual Studio, avec l'option PROJECTS_PROFILING.</li>
 *<li>Ajout de la possibilité de choisir quels projets du Castor sont à générer (dans le groupe CASTOR).</li>
 *<li>Réorganisation globale de l'arborescence des fichiers</li>
 *</ul>
 *
 *\subsubsection mods_0_7_0_gen Général
 *<ul>
 *<li>Découpe des fichiers afin de n'avoir plus qu'une classe par fichier.</li>
 *<li>Support de la compilation x64.</li>
 *<li>Compabilité avec wxWidgets 2.8 et Linux améliorée.</li>
 *<li>Ajout de fonctions Destroy pour chaque fonction Create dans les plug-ins, afin que la création et la destruction de ceux-ci se fassent dans l'espace mémoire des DLL (pour Windows).</li>
 *<li>Ajout du plug-in d'import ASSIMP, permettant d'élargir le nombre de formats supportés. La fonction ImporterPlugin::GetExtensions permet maintenant de récupérer un tableau de paires &lt;extension, file type&gt;.</li>
 *</ul>
 *
 *\subsubsection mods_0_7_0_cu CastorUtils
 *<ul>
 *<li>Toutes les variables statiques et globales ont été enlevées.</li>
 *<li>Remplacement de la classe String par une classe string.</li>
 *<li>Suppression des dépendances à boost, à part pour thread, et uniquement pour les compilateurs ne supportant pas std::thread.</li>
 *<li>Les macros cstrnlen, cstrftime, cstrncpy, cfopen, cfopen64, cfseek et cftell ont été remplacées par des fonctions dans Castor.</li>
 *<li>Suppression de la macro DECLARE_SHARED_PTR, remplacée par DECLARE_SMART_PTR, et les typedefs XxxPtr ont été remplacés par leurs équivalents XxxSPtr.</li>
 *<li>Suppression de l'encodage Unicode pour les fichiers.</li>
 *<li>Ajout du support de l'indentation de flux.</li>
 *<li>Améliorations de Castor::FileParser :
 *<ul>
 *<li>Il peut ignorer des sections de code sans avoir à les commenter (tels que les blocs de shader HLSL alors que le RenderSystem chargé est GlRenderSystem, etc.).</li>
 *<li>Il prend en compte les paramètres des fonctions d'analyse.</li>
 *<li>Les fonctions peuvent maintenant avoir des paramètres, pas uniquement des types de paramètres, et ces paramètres s'analysent eux-mêmes.</li>
 *</ul>
 *</li>
 *<li>Réduction de l'occupation mémoire des images.</li>
 *<li>Modification de Castor::Point :
 *<ul>
 *<li>les fonctions non dépendantes ont été déplacées dans une classe d'aide : Castor::point. Cette classe contient donc les fonctions normalise, get_normalised, dot, cos_theta, negate et toutes les fonctions de calcul de distance.</li>
 *<li>Suppression des méthodes link, copy/clopy/clone.</li>
 *<li>Il contient maintenant un tableau statique au lieu d'un tableau dynamique.</li>
 *</ul>
 *</li>
 *<li>La classe Castor::Colour ne dérive plus de Point4f.</li>
 *<li>La fonction DynamicLibrary::GetFunction a été modifiée, ajout d'un paramètre template, le type de fonction, afin de ne plus avoir de conversions lors de l'utilisation de DynamicLibrary::GetFunction.</li>
 *<li>Ajout du support des niveaux de log pour Castor::Logger.</li>
 *<li>Modifications de Castor::Line3D :
 *<ul>
 *<li>Implémentation de la méthode de calcul d'intersection</li>
 *<li>Cette classe a maintenant deux constructeurs nommés : FromPointAndSlope et FromPoints, pour éviter les confusions.</li>
 *</ul>
 *</li>
 *<li>Création de deux nouvelles classes : Size et Positionau lieu d'un typedef pour chacune.</li>
 *<li>Création de la classe Castor::Coord pour récupérer les fonctionnalités dynamiques de l'ancien Castor::Point.</li>
 *</ul>
 *
 *\subsubsection mods_0_7_0_c3d Castor3D
 *<ul>
 *<li>Ajout des méthodes HasParameter et SetParameter dans la classe ShaderObject, pour appliquer les matrices de la classe Pipeline.</li>
 *<li>Ajout de fonctions Begin et End dans la classe Mesh afin de pouvoir itérer sur les submeshes sans passer par les fonctions GetSubmesh et GetSubmeshCount.</li>
 *<li>Ajout de fonctions Begin et End dans la classe Material functions Begin and End.</li>
 *<li>Ajout de la classe FrameVariableBuffer pour gérer les variables uniformes avec les UBO OpenGL ou les Constant buffer de Direct3D 11.</li>
 *<li>Ajout de la classe DepthStencilState pour gérer ces états à la mode Direct3D 11 (qui fait ça bien).</li>
 *<li>De même, ajout des classes BlendState et RasteriserState.</li>
 *<li>Création des classes Texture, StaticTexture et TextureLayout pour faciliter l'implémentation des cibles de rendu.</li>
 *<li>Ajout de canaux pour les textures (couleur, ambiante, diffusion, speculaire, normale, hauteur, brillance).</li>
 *<li>Création de la classe TechniqueParameters pour passer des paramètres spécifiques aux techniques comme, par exemple, le nombre d'échantillons (pour le MSAA).</li>
 *<li>Introduction de la classe SamplerState pour grouper les appels à SetSamplerState et autres.</li>
 *<li>Implémentation des Frame Buffers.</li>
 *<li>Réduction de l'occupation mémoire des tampons de sommets.</li>
 *<li>Modification de BufferElementGroup afin qu'il ne puisse plus contenir son tampon.</li>
 *<li>La classe Overlay n'étend plus Renderable, il n'y a plus qu'une instance de OverlayRenderer, gérée par la classe OverlayCache. Toutes les incrustations sont maintenant rendues via cette instance.</li>
 *<li>Amélioration de l'occupation mémoire : diminution drastique de la taille d'un Vertex (de 152 à 24 octets) et d'une Face (de 140 à 16 octets).</li>
 *<li>Modification de Subdivision, elle se trouve maintenant uniquement dans Subdivider et plus dans Geometry, Mesh ou Submesh.</li>
 *<li>Les lumières sont maintenant implémentées dans une texture, passant la limite de 8 à 100 sources.</li>
 *<li>Suppression des méthodes ApplyXXXMatrix de la classe IPipelineImpl, maintenant la classe Pipeline applique les matrices elle-même.</li>
 *<li>Implémentation du MSAA.</li>
 *<li>Implémentation de l'Alpha to Coverage lorsque le MSAA est activé.</li>
 *<li>Revue des tailles : Viewport contient la taille interne, RenderTarget et RenderWindow contiennent la taille externe.</li>
 *<li>Découpe de eBUFFER_MODE en eBUFFER_ACCESS_TYPE et eBUFFER_ACCESS_NATURE.</li>
 *<li>L'initialisation des objets GPU se passe maintenant avec l'utilisation de deux évènements : InitialiseEvent et CleanupEvent.</li>
 *<li>Fusion de GpuBuffer::Initialise et GpuBuffer::SetShaderProgram afin d'en simplifier l'utilisation.</li>
 *<li>Suppression de la génération automatique des mipmaps. Maintenant, si l'utilisateur vuet les générer, il utilise la fonction TextureLayout::GenerateMipmaps.</li>
 *<li>Implémentation de l'instanciation hardware des maillages.</li>
 *<li>Modifications de Castor3D::Submesh :
 *<ul>
 *<li>Les classes Submesh et SmoothingGroup ont été fusionnées.</li>
 *<li>Modification de la méthode ComputeTangents pour prendre en compte les normales des sommets.</li>
 *<li>Ajout d'un overload pour la méthode AddPoints afin de pouvoir lui donner un stVERTEX_GROUP en paramètre.</li>
 *<li>Modification de la génération des normales.</li>
 *<li>Déplacement des matériaux, de Submesh à Geometry.</li>
 *<li>Modification du compte des instances, pour le rendre spécifique aux matériaux utilisés par les instances du Submesh.</li>
 *</ul>
 *</li>
 *<li>Modifications de Castor3D::Pass :
 *<ul>
 *<li>Création d'une fonction Pass::Initialise afin que les shaders ne soient initialisés qu'une fois, et pas à chaque frame.</li>
 *<li>Réorganisation des unités de texture des passes, afin que celles associées à un canal arrivent premières.</li>
 *</ul>
 *</li>
 *<li>Ajout d'une instance de BlendState dans la classe Pass et suppression de la gestion du mélange RGB/Alpha de la classe Pass.</li>
 *<li>Modifications de Castor3D::Scene :
 *<ul>
 *<li>Ajout de la possibilité d'activer ou désactiver le deferred rendering, option disponible dans les fichier cscn.</li>
 *<li>Ajout de fonctions templates pour ajouter, supprimer ou récupérer un objet quel que soit son type (Light, Geometry, Camera, SceneNode).</li>
 *<li>Les Submesh sont maintenant triés en fonction du matériau appliqué afin de pouvoir, entre autres, rendre les sous-maillages avec transparence après les autres.</li>
 *</ul>
 *</li>
 *<li>Modifications de Castor3D::RenderTarget :
 *<ul>
 *<li>Modifié RenderTarget : elle contient maintenant son frame buffer et ce qui s'y rattache. C'est maintenant un Renderable et un TargetRenderer a donc été créé.</li>
 *<li>RenderTarget utilise maintenant RenderTechnique pour effectuer son rendu.</li>
 *</ul>
 *</li>
 *<li>Modifications de Castor3D::RenderWindow :
 *<ul>
 *<li>RenderWindow n'étend plus RenderTarget, mais en a un en membre privé.</li>
 *<li>Création d'une méthode WindowRenderer::EndScene pour préparer le rendu du frame buffer de la RenderTarget dans la fenêtre.</li>
 *</ul>
 *</li>
 *<li>Modifications des shaders :
 *<ul>
 *<li>Suppression des appels à ShaderObject afin d'en faire une classe interne à Castor3D.</li>
 *<li>Modification des shaders, ils prennent maintenant en compte le modèle de shader, contiennent tous les fichiers et sources définis par modèle. Le choix du modèle est fait à la compilation, en choisissant le modèle le plus haut supporté.</li>
 *<li>Modification des sources par défaut pour les shaders. Elles sont maitenant générées automatiquement et plus aucun fichier externe n'est nécessaire.</li>
 *<li>Modification de FrameVariable afin de prendre en compte eFRAME_VARIABLE_TYPE.</li>
 *</ul>
 *</li>
 *<li>Modifications de Castor3D::Context
 *<ul>
 *<li>Suppression des méthodes Context::Enable et Context::Disable, suppression de la classe RenderState.</li>
 *<li>Suppression de la méthode Context::SetPolygonMode.</li>
 *</ul>
 *</li>
 *</ul>
 *
 *\subsubsection mods_0_7_0_gl GlRenderSystem
 *<ul>
 *<li>Création de la classe GeometryBuffers pour gérer les Vertex Array Objects. Ils contiennent maintenant leurs tampons, permettant d'intégrer le choix d'utiliser les VAO ou non.</li>
 *<li>Les Geometry shader fonctionnent dans GlRenderSystem.</li>
 *<li>Suppression de la dépendance à GLEW.</li>
 *<li>Support des extensions de debug d'OpenGL.</li>
 *<li>Ajout du support de GL_MIRRORED_REPEAT.</li>
 *<li>Ajout du support de GL_TEXTURE_MAX_ANISOTROPY.</li>
 *</ul>
 *
 *\subsubsection mods_0_7_0_gui GuiCommon
 *<ul>
 *<li>Maintenant ShaderDialog utilise wxSizers.</li>
 *<li>Utilisation de wxStcEditor pour l'édition de shaders.</li>
 *<li>La fenêtre de matériaux utilise les wxSizers pour placer et redimensionner les éléments.</li>
 *<li>Ajout de la possibilité de changer le matériau utilisé par un sous-maillage, avec la fenêtre wxGeometriesListFrame.</li>
 *<li>Correction de bugs dans le chargement des images.</li>
 *<li>Amélioration de StcTextEditor afin d'avoir une meilleure coloration syntaxique.</li>
 *</ul>
 *
 *\subsubsection mods_0_7_0_cview CastorViewer
 *<ul>
 *<li>Maintenant, MainFrame utilise wxSizers.</li>
 *<li>Ajout d'options de ligne de commande :
 *<ul>
 *<li>-f/--file pour charger un fichier de scène au chargement</li>
 *<li>-h/--help pour afficher une aide</li>
 *<li>-l/--level pour définir le niveau de log :
 *<ul>
 *<li>- 0 pour logger debug, infos, warnings et erreurs</li>
 *<li>- 1 pour logger infos, warnings et erreurs</li>
 *<li>- 2 pour logger warnings et erreurs</li>
 *<li>- 3 pour logger les erreurs uniquement.</li>
 *</ul>
 </li>
 *</ul>
 *</li>
 *<li>Maintenant les plug-ins sont chargés dans un thread</li>
 *<li>Ajout d'un bouton &quot;Export&quot;.</li>
 *</ul>
 *
 *\subsection bugs_0_7_0 Corrections
 *\subsubsection bugs_0_7_0_gen General
 *<ul>
 *<li>Modification des importeurs afin de ne plus avoir de fuites de mémoires détectées par Visual Studio.</li>
 *</ul>
 *
 *\subsubsection bugs_0_7_0_cu CastorUtils
 *<ul>
 *<li>Castor::Image a été modifiée, j'ai retiré la possibilité de charger les images par handle, car cela provoquait des problèmes avec FreeImage sur certains formats.</li>
 *<li>Réparation d'un problème de matrices lors du rendu des incrustations.</li>
 *<li>Correction des transformations récursives.</li>
 *</ul>
 *
 *\subsubsection bugs_0_7_0_c3d Castor3D
 *<ul>
 *<li>Corrections des sous-maillages ayant un matériau transparent, car ils n'étaient pas affichés.</li>
 *<li>Corrigé la génération de mipmaps.</li>
 *</ul>
 *
 *\subsubsection bugs_0_7_0_cview CastorViewer
 *<ul>
 *<li>Déplacement de la destruction de m_pCastor3D dans la méthode OnDestroy.</li>
 *</ul>
 *
 *\subsubsection bugs_0_7_0_gl GlRenderSystem
 *<ul>
 *<li>Correction d'un bug dans DoGetDeferredShaderSource.</li>
 *<li>Correction de la génération des mipmaps dans les classes GlStaticTexture et GlDynamicTexture.</li>
 *<li>Correction du traitement des textures de brillance dans les programmes GLSL auto générés.</li>
 *</ul>
 *
 *\subsubsection bugs_0_7_0_obj ObjImporter
 *<ul>
 *<li>L'import des matériaux et textures a été corrigé.</li>
 *</ul>
 *
 *\section version_0_6_1_2 Version 0.6.1.2
 *
 *\subsection adds_0_6_1_2 Ajouts
 *<ul>
 *<li>Création du renderer Dx9Renderer, pour le rendu sous Direct3D 9. Il reste des choses à régler, notamment au niveau shaders et matrices, mais l'essentiel est là.</li>
 *<li>Création de différents fichiers pour les contextes OpenGL (X11, Gtk, Msw), je n'ai pas encore pu tester ça correctement, mes drivers ATI ayant tué OpenGL sur ma Debian (snif...).</li>
 *<li>Création d'une boîte de dialogue de sélection de renderer au lancement de CastorViewer, CastorShape et CastorArchitect, ajout d'un splash screen pour ces 3 applis.</li>
 *<li>Ajout d'une petite appli qui transforme n'inmporte quel type d'image en XPM, plus facile pour le développement multiplateforme avec wxWidgets.</li>
 *<li>Ajout de nouvelles fonctionnalités aux fichiers de scène (alpha_rejection, rgb_blend, alpha_blend, hl_shader_program, ...)</li>
 *</ul>
 *
 *\subsection mods_0_6_1_2 Modifications
 *<ul>
 *<li>Optimisation du fichier CMakeLists et des fichiers du dossier 'cmake_modules' de façon à mieux prendre en charge les différentes plateformes, ainsi que les différentes configurations des PC clients.</li>
 *<li>Revue du système de VertexBuffers et IndexBuffers, in troduction de la notion de VertexDeclaration et BufferElementGroup, ainsi on peut créer des VertexBuffers en mettant à peu près ce que l'on veut comme données dedans (parmi vertex, normals, tangents, diffuse, texcoords0,1,2,3) et ordonnées comme on veut.</li>
 *<li>La classe Vertex dérive donc maintenant de BufferElementGroup.</li>
 *<li>Les TextureEnvironment vont disparaitre à la prochaine version, la gestion du multitexturing se faisant maintenant directement au niveau des passes et des texture units.</li>
 *<li>Suppression des fonctions issues de GLU, afin d'avoir une bibliothèque de moins à linker.</li>
 *</ul>
 *
 *\subsection bugs_0_6_1_2 Bugs
 *<ul>
 *<li>Problème avec les matrices pour DxRenderSystem (je pense à la différence right-handed / left-handed, mais mes premiers tests à ce niveau ne sont pas concluants).</li>
 *<li>Problème avec les shaders HLSL, je n'ai pour l'instant pas d'affichage, je pense que c'est lié aux problèmes de matrices que je rencontre avec Direct3D.</li>
 *<li>Je n'ai pas avancé sur les shaders Cg et ai donc toujours les mêmes problèmes qu'auparavant.</li>
 *</ul>
 *
 *\section version_0_6_1_1 Version 0.6.1.1
 *
 *\subsection adds_0_6_1_1 Ajouts
 *<ul>
 *<li>Création d'un fichier CMakeLists pour générer la solution à partir de CMake.</li>
 *<li>Réorganisation des fichiers du dossier afin de mieux se prêter à ça.</li>
 *<li>Séparation de la classe Plugin en 3 types différents : Renderer, Divider et Importer. Ainsi on pourra ajouter de nouveaux types de plug-in aisément et l'ajout d'importers et de subdiviseurs se fera sans devoir recompiler tous les projets.</li>
 *<li>Ca entraîne la création d'un projet par importeur existant (OBJ, PLY, MD2, MD3, 3DS, ...)</li>
 *<li>PNTrianglesDivider reste dans le Castor car utilisé par la primitive Icosahedron, LoopDivider est quand à lui placé dans un plug-in.</li>
 *<li>J'ai commencé à implémenter les shaders Cg. Ca implique une modification des fichiers de scène, car dans Cg les variables uniform sont liées à un programme (vertex, pixel, geometry) plutôt qu'au shader complet comme c'est le cas en GLSL. Il reste quelques trucs à revoir (je ne sais pas encore quoi) pour que ça fonctionne.</li>
 *</ul>
 *
 *\subsection mods_0_6_1_1 Modifications
 *<ul>
 *<li>Suppression de FreeImagePlus, maintenant les projets ne nécessitent plus que FreeImage.</li>
 *<li>La création des tampons utilisés pour le rendu (Vertex, Attribute, Texture, Normals, ...) se fait maintenant par le RenderSystem => on peut donc les créer dans Castor3D et pas seulement dans les renderer (GL2, GL3 et dans le futur D3D).</li>
 *<li>Suppression des Singletons pour les différents Managers (Image, Font, Scene, Material, Animation et Mesh).</li>
 *<li>J'ai retouché les SceneNodes, j'ai refusionné les 3 classes différentes en une seule pour des commodités d'utilisation.</li>
 *<li>Création d'une classe IdPoint, qui étend Point et y ajoute un Index qui correspond à son index dans le submesh. La fonction AddPoint de Submesh crée maintenant des IdPoint et la fonction AddFace se sert d'eux pour générer les Vertex.</li>
 *<li>Les subdiviseurs peuvent subdiviser dans un thread à part, permettant de ne pas figer une application pendant la subdivision (cf. CastorShape). Il est possible d'assigner une fonction qui sera lancée à la fin de la subdivision (cf. CastorShape).</li>
 *</ul>
 *
 *\subsection fix_0_6_1_1 Corrections
 *<ul>
 *<li>Correction du LoopDivider qui ne repositionnait plus les Vertex (les nouveaux étaient créés mais plus replacés, dû à la nouvelle architecture des Vertex)</li>
 *</ul>
 *
 *\section version_0_6_1_0 Version 0.6.1.0
 *
 *\subsection add_0_6_1_0 Ajouts
 *<ul>
 *<li>Utilisation des smart pointers tr1.</li>
 *<li>Création d'un tronc commun 'OpenGLCommon' pour les 2 renderers OpenGL</li>
 *<li>Création d'une classe de Pipeline qui effectue les calculs matriciels et d'autres petites fonctions (perspective, ortho, ...)</li>
 *</ul>
 *
 *\subsection mods_0_6_1_0 Modifications
 *<ul>
 *<li>Le MemoryManager (activé uniquement en debug) a été modifié, ajout d'une classe template MemoryTraced qui surcharge les operateurs new, delete new [] et delete [], ce afin d'avoir une meilleure trace des objets créés et détruits. Chaque classe des projets du Castor dérivent maintenant de celle-ci, et 3 fichiers sont donc créés dans le répertoire racine (C:\ sous Windows) : memoryleaks.log, memoryalloc.log et memorydealloc.log</li>
 *<li>La classe TransformationMatrix n'existe plus, il ne s'agit plus que d'une collection de fonctions sur matrices carrées 4x4.</li>
 *<li>Modification de la gestion des SceneNode : Une classe de base NodeBase qui contient les informations qui se trouvaient dans SceneNode, 3 classes dérivées (GeometryMaterial, CameraMaterial et LightMaterial) qui s'occupent des fonctionnalités spécifiques au rendu de chacun des types liés (respectivement Geometry, Camera et Light).</li>
 *<li>La classe MovableObject a maintenant moins de fonctionnalités (en fait elle n'a plus qu'un nom et un NodeBase) et les classes Light et Camera dérivent maintenant de cette classe (pour plus d'uniformité dans les déplacements de ces objets par rapport aux géométries)</li>
 *<li>Renommage des classes UniformVariable en FrameVariable, pour refléter plus ce que c'est et moins lier ce genre de choses à OpenGL.</li>
 *<li>Le module OpenGL 3.x/4.x est achevé.</li>
 *<li>Modifier la structure des vertex de façon à ce qu'ils incluent les coordonnées de texture et les normales directement, de façon à n'avoir qu'1 buffer de vertex par mesh au lieu de 3 actuellement.</li>
 *</ul>
 *
 *\subsection fix_0_6_1_0 Corrections
 *<ul>
 *<li>Correction du plantage à la fermeture du soft en release uniquement.</li>
 *<li>Correction du rendu noir avec OpenGL 3.x, corrigé je ne sais trop comment, mais grâce à la suppression progressive du pipeline fixe en OpenGL 2.x</li>
 *</ul>
 *
 *\section version_0_6_0_0 Version 0.6.0.0
 *
 *\subsection adds_0_6_0_0 Ajouts
 *<ul>
 *<li>Ajout d'une classe de gestion des matrices de projection, normale et visualisation, en vue de la suppression du pipeline fixe dans les renderers OpenGL.</li>
 *<li>Ajout de smart pointers persos, inspirés de ceux de boost.</li>
 *<li>Ajout d'une classe de gestion des angles, permettant de ne pas s'embeter avec les conversions radian-degrés.</li>
 *<li>Ajout d'une classe de gestion de path, permettant de gérer les slash ou anti-slash selon l'OS.</li>
 *<li>Création de conteneurs thread-safe issus de la stl (vector, list, map, multimap, set).</li>
 *<li>Séparation du renderer OpenGL en 2 projets : un compatible OpenGL 2.x et antérieur (GL2RenderSystem) le second compatible OpenGL3.x et ultérieur (GL3RenderSystem).</li>
 *<li>GL2RenderSystem : en fonction du PC client, suppression du pipeline fixe ou pas (si l'utilisation des shaders est permise le pipeline fixe est supprimé, sinon il est gardé)</li>
 *</ul>
 *
 *\subsection mods_0_6_0_0 Modifications
 *<ul>
 *<li>Modification de la gestion des threads et mutexes, permettant le choix de ceux qu'on utilise (Win32, MFC ou boost).</li>
 *<li>Modification des classes de gestion des matrices et points.</li>
 *<li>Modification de la classe de gestion des chaînes de caractères qui sont maintenant indépendantes de Unicode ou MultiByte.</li>
 *<li>Modification de la classe de gestion de fichiers qui ne permet plus le choix IO ou Stream mais utilise Stream uniquement.</li>
 *<li>Modification du render system, ajout d'une classe renderable qui se charge de créer le renderer associé, afin de simplifier la création des dérivés de cette classe (submesh, scene node, light, camera...)</li>
 *<li>Changement des noms des fichiers de scène/materiaux/meshes du moteur (respectivement cscn/cmtl/cmsh)</li>
 *</ul>
 *
 *\subsection fix_0_6_0_0 Corrections
 *<ul>
 *<li>Protection par recursive mutex des données partagées (par les managers, essentiellement) permettant d'éviter des plantages aléatoires.</li>
 *</ul>
 *
 *\subsection todo_0_6_0 A faire
 *<ul>
 *<li>Il reste un plantage à la fermeture du soft en release uniquement, apparemment dû aux shaders.</li>
 *<li>Pour GL3RenderSystem, j'ai un rendu noir (?!?) mais les transformations sont prises en compte correctement.</li>
 *</ul>
 *
 *\section version_0_5_0_1 Version 0.5.0.1
 *
 *\subsection adds_0_5_0_1 Ajouts
 *<ul>
 *<li>Dans cette version l'espace tangent est calculé et est disponible dans les shaders GLSL.</li>
 *<li>Seule la tangente est calculée, la bitangente pouvant l'être dans le shader avec un produit vectoriel entre la tangente et la normale.</li>
 *<li>Pour accéder à la tangente, il suffit de déclarer l'attribut suivant dans le vertex shader :</li>
 *<li>attribute vec3 tangent;</li>
 *</ul>
 *
 *\subsection mods_0_5_0_1 Modifications
 *<ul>
 *<li>Les différentes classes de point (2D, 3D, 4D) ont leur membres en union, ce qui permet de personnaliser l'appel de ces membres. Ainsi un point 2D a comme membres (x, y) ou (u, v). Un point 3D a comme membres (x, y, z) ou (u, v, w) ou (r, g, b). Un point 4D a comme membres (x, y, z, w) ou (r, g, b, a) ou (left, top, right, bottom).</li>
 *</ul>
 *
 *\subsection fix_0_5_0_1 Corrections
 *<ul>
 *<li>Correction du plantage à la fermeture de l'application lors de la suppression des VBOs. J'ai intégré un buffer manager qui les supprime pendant une boucle d'affichage.</li>
 *</ul>
 */

/*!
 *\~english
 *\mainpage Castor3D Engine
 *\section intro_sec Change log
 *This page is here to inform about the changes since the earliest versions of the engine
 *
 *\section version_0_6_1_2 Version 0.6.1.2
 *
 *\subsection Adds
 *<ul>
 *<li>Created Dx9Renderer to render with Direct3D 9. Stills things to correct, in shaders and matrices, but it's almost done.</li>
 *<li>Created different files for OpenGL contexts (X11, Gtk, Msw), X11 and Gtk still untested.</li>
 *<li>Created a splash screen for CastorViewer and CastorShape, created a dialog box in order to choose the renderer at startup.</li>
 *<li>Added a small app to converet from an image to a XPM.</li>
 *<li>Added new features to scene files (alpha_rejection, rgb_blend, alpha_blend, hl_shader_program, ...)</li>
 *</ul>
 *
 *\subsection Modifications
 *<ul>
 *<li>Optimised CMakeLists file and 'cmake_modules' files in order to take care of various configurations in a better way.</li>
 *<li>Revised VertexBuffers and IndexBuffers, introducing VertexDeclaration and BufferElementGroup, so we can create VertexBuffersof customisable types.</li>
 *<li>Vertex class now derives from BufferElementGroup.</li>
 *<li>TextureEnvironment are going to disappear in the next version, now multitexturing management is done with passes and texture units.</li>
 *<li>Suppressed GLU dependencies.</li>
 *</ul>
 *
 *\subsection Bugs
 *<ul>
 *<li>Trouble with Direct3D matrices (maybe differences between right-handed / left-handed).</li>
 *<li>Trouble with HLSL shaders, surely linked to previous trouble.</li>
 *<li>Still troubles with Cg shaders (not worked on it).</li>
 *</ul>
 *
 *\section version_0_6_1_1 Version 0.6.1.1
 *
 *\subsection Adds
 *<ul>
 *<li>Created a CMakeLists file to generate the solution from CMake.</li>
 *<li>Reorganised folders to make them more compliant with CMake.</li>
 *<li>Splitted Plugin class in 3 parts : Renderer, Divider et Importer.</li>
 *<li>Created one project per importer (OBJ, PLY, MD2, MD3, 3DS, ...)</li>
 *<li>PNTrianglesDivider is left in Castor3D because it is sstill used by Icosahedron, LoopDivider is exported in a plugin.</li>
 *<li>Began Cg shaders implementation. This implies a scene file modification because in Cg, uniform variables are are linked to a shader instead of being linked to the whole program. Still buggy.</li>
 *</ul>
 *
 *\subsection Modifications
 *<ul>
 *<li>Suppressed FreeImagePlus dependencies.</li>
 *<li>Vertex, Attribute and Index buffers are now created by RenderSystem so they can be created out of the renderers.</li>
 *<li>Suppressed Singletons for the Managers (Image, Font, Scene, Material, Animation and Mesh).</li>
 *<li>Remerged SceneNode classes.</li>
 *<li>Created IdPoint class, it is a point with just an index. Submes::AddPoint creates now IdPoints and Submesh::AddFace uses them to create the vertex.</li>
 *<li>Subdividers can  subdivide in a thread in order not to freeze the main thread. It is possible to define a function to use at the end of the subdivision (cf. CastorShape).</li>
 *</ul>
 *
 *\subsection Corrections
 *<ul>
 *<li>Corrected LoopDivider which didn't reposition the Vertex</li>
 *</ul>
 *
 *\section version_0_6_1_0 Version 0.6.1.0
 *
 *\subsection Adds
 *<ul>
 *<li>Using std or tr1 or boost smart pointers.</li>
 *<li>Created OpenGlCommon for common treatments between GL2RenderSystem and GL3RenderSystem</li>
 *<li>Created class Pipeline which performs the matrix computings and other functions (frustum, perspective, ortho, ...)</li>
 *</ul>
 *
 *\subsection Modifications
 *<ul>
 *<li>Modified MemoryManager, added a template class MemoryTraced which overload new, delete new [] et delete [] operators in order to have better trace of allocations and deallocations. Each Castor class derive from this one and 3 files are created in root folder to log allocations, deallocations : memoryleaks.log, memoryalloc.log and memorydealloc.log</li>
 *<li>TransformationMatrix no longer exists, it is now a collection of functions which computes 4x4 matrix operations</li>
 *<li>Modification SceneNode management : base class NodeBase holds the informations that were in SceneNode, 3 derived classes (GeometryNode, CameraNode et LightNode) take care of each category specificities.</li>
 *<li>MovableObject class now has less features (name and NodeBase), Light and Camera now derive from it</li>
 *<li>Renamed UniformVariable to FrameVariable.</li>
 *<li>OpenGL 3.x/4.x now fully supported.</li>
 *<li>Modified Vertex class in order to make them include texture coordinates and normals in order to have only 1 vertex buffer instead of 3 in a mesh.</li>
 *</ul>
 *
 *\subsection Corrections
 *<ul>
 *<li>Corrected the shutdown crash.</li>
 *<li>Corrected the black render with OpenGL 3.x</li>
 *</ul>
 *
 *\section version_0_6_0_0 Version 0.6.0.0
 *
 *\subsection Adds
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
 *\subsection Modifications
 *<ul>
 *<li>Modified thread management, leaving the user choosing between Win32, MFC or boost.</li>
 *<li>Modified point and matrices classes.</li>
 *<li>Modified String class which now is independant from MBCS or Unicode.</li>
 *<li>Modified File class which now only uses streams.</li>
 *<li>Modified the render system, added a class Renderable used to create associated renderer in order to ease the creation of derived classes (submesh, scene node, light, camera...)</li>
 *<li>Changed scenes/materials/meshes files name (respectively cscn/cmtl/cmsh)</li>
 *</ul>
 *
 *\subsection Corrections
 *<ul>
 *<li>Protected managers with recursive mutexes to avoid random crashes.</li>
 *</ul>
 *
 *\subsection Bugs
 *<ul>
 *<li>Still a crash at shutdown, supposedly due to shaders.</li>
 *<li>In GL3RenderSystem, I've got a black screen (?!?) but transformations seem to be correct.</li>
 *</ul>
 *
 *\section version_0_5_0_1 Version 0.5.0.1
 *
 *\subsection Adds
 *<ul>
 *<li>Tangent space is computed and available in GLSL shaders.</li>
 *<li>Only tangent is computed, bitangent can be in the shader with a cross product.</li>
 *<li>To access to the tangent you must declare the following attribute in the vertex shader :</li>
 *<li>attribute vec3 tangent;</li>
 *</ul>
 *
 *\subsection Modifications
 *<ul>
 *<li>The points classes have their members in a union so you can call it as you wish (xyz, rgb, left right top bottom)</li>
 *</ul>
 *
 *\subsection Corrections
 *<ul>
 *<li>Corrected the crash on shutdown. It was due to VBOs which are now destroyed in a final render loop.</li>
 *</ul>
 *
 *
 *
 *\~french
 *\mainpage Moteur Castor3D
 *\section intro_sec Suivi de versions
 *Cette page sert à informer de évolutions du moteur depuis les premières versions
 *
 *\section version_0_6_1_2 Version 0.6.1.2
 *
 *\subsection Ajouts
 *<ul>
 *<li>Création du renderer Dx9Renderer, pour le rendu sous Direct3D 9. Il reste des choses à régler, notamment au niveau shaders et matrices, mais l'essentiel est là.</li>
 *<li>Création de différents fichiers pour les contextes OpenGL (X11, Gtk, Msw), je n'ai pas encore pu tester ça correctement, mes drivers ATI ayant tué OpenGL sur ma Debian (snif...).</li>
 *<li>Création d'une boîte de dialogue de sélection de renderer au lancement de CastorViewer, CastorShape et CastorArchitect, ajout d'un splash screen pour ces 3 applis.</li>
 *<li>Ajout d'une petite appli qui transforme n'inmporte quel type d'image en XPM, plus facile pour le développement multiplateforme avec wxWidgets.</li>
 *<li>Ajout de nouvelles fonctionnalités aux fichiers de scène (alpha_rejection, rgb_blend, alpha_blend, hl_shader_program, ...)</li>
 *</ul>
 *
 *\subsection Modifications
 *<ul>
 *<li>Optimisation du fichier CMakeLists et des fichiers du dossier 'cmake_modules' de façon à mieux prendre en charge les différentes plateformes, ainsi que les différentes configurations des PC clients.</li>
 *<li>Revue du système de VertexBuffers et IndexBuffers, in troduction de la notion de VertexDeclaration et BufferElementGroup, ainsi on peut créer des VertexBuffers en mettant à peu près ce que l'on veut comme données dedans (parmi vertex, normals, tangents, diffuse, texcoords0,1,2,3) et ordonnées comme on veut.</li>
 *<li>La classe Vertex dérive donc maintenant de BufferElementGroup.</li>
 *<li>Les TextureEnvironment vont disparaitre à la prochaine version, la gestion du multitexturing se faisant maintenant directement au niveau des passes et des texture units.</li>
 *<li>Suppression des fonctions issues de GLU, afin d'avoir une librairie de moins à linker.</li>
 *</ul>
 *
 *\subsection Bugs
 *<ul>
 *<li>Problème avec les matrices pour DxRenderSystem (je pense à la différence right-handed / left-handed, mais mes premiers tests à ce niveau ne sont pas concluants).</li>
 *<li>Problème avec les shaders HLSL, je n'ai pour l'instant pas d'affichage, je pense que c'est lié aux problèmes de matrices que je rencontre avec Direct3D.</li>
 *<li>Je n'ai pas avancé sur les shaders Cg et ai donc toujours les mêmes problèmes qu'auparavant.</li>
 *</ul>
 *
 *\section version_0_6_1_1 Version 0.6.1.1
 *
 *\subsection Ajouts
 *<ul>
 *<li>Création d'un fichier CMakeLists pour générer la solution à partir de CMake.</li>
 *<li>Réorganisation des fichiers du dossier afin de mieux se prêter à ça.</li>
 *<li>Séparation de la classe Plugin en 3 types différents : Renderer, Divider et Importer. Ainsi on pourra ajouter de nouveaux types de plugin aisément et l'ajout d'importers et de subdiviseurs se fera sans devoir recompiler tous les projets.</li>
 *<li>Ca entraîne la création d'un projet par importeur existant (OBJ, PLY, MD2, MD3, 3DS, ...)</li>
 *<li>PNTrianglesDivider reste dans le Castor car utilisé par la primitive Icosahedron, LoopDivider est quand à lui placé dans un plugin.</li>
 *<li>J'ai commencé à implémenter les shaders Cg. Ca implique une modification des fichiers de scène, car dans Cg les variables uniform sont liées à un programme (vertex, pixel, geometry) plutôt qu'au shader complet comme c'est le cas en GLSL. Il reste quelques trucs à revoir (je ne sais pas encore quoi) pour que ça fonctionne.</li>
 *</ul>
 *
 *\subsection Modifications
 *<ul>
 *<li>Suppression de FreeImagePlus, maintenant les projets ne nécessitent plus que FreeImage.</li>
 *<li>La création des tampons utilisés pour le rendu (Vertex, Attribute, Texture, Normals, ...) se fait maintenant par le RenderSystem => on peut donc les créer dans Castor3D et pas seulement dans les renderer (GL2, GL3 et dans le futur D3D).</li>
 *<li>Suppression des Singletons pour les différents Managers (Image, Font, Scene, Material, Animation et Mesh).</li>
 *<li>J'ai retouché les SceneNodes, j'ai refusionné les 3 classes différentes en une seule pour des commodités d'utilisation.</li>
 *<li>Création d'une classe IdPoint, qui étend Point et y ajoute un Index qui correspond à son index dans le submesh. La fonction AddPoint de Submesh crée maintenant des IdPoint et la fonction AddFace se sert d'eux pour générer les Vertex.</li>
 *<li>Les subdiviseurs peuvent subdiviser dans un thread à part, permettant de ne pas figer une application pendant la subdivision (cf. CastorShape). Il est possible d'assigner une fonction qui sera lancée à la fin de la subdivision (cf. CastorShape).</li>
 *</ul>
 *
 *\subsection Corrections
 *<ul>
 *<li>Correction du LoopDivider qui ne repositionnait plus les Vertex (les nouveaux étaient créés mais plus replacés, dû à la nouvelle architecture des Vertex)</li>
 *</ul>
 *
 *\section version_0_6_1_0 Version 0.6.1.0
 *
 *\subsection Ajouts
 *<ul>
 *<li>Utilisation des smart pointers tr1.</li>
 *<li>Création d'un tronc commun 'OpenGLCommon' pour les 2 renderers OpenGL</li>
 *<li>Création d'une classe de Pipeline qui effectue les calculs matriciels et d'autres petites fonctions (perspective, ortho, ...)</li>
 *</ul>
 *
 *\subsection Modifications
 *<ul>
 *<li>Le MemoryManager (activé uniquement en debug) a été modifié, ajout d'une classe template MemoryTraced qui surcharge les operateurs new, delete new [] et delete [], ce afin d'avoir une meilleure trace des objets créés et détruits. Chaque classe des projets du Castor dérivent maintenant de celle-ci, et 3 fichiers sont donc créés dans le répertoire racine (C:\ sous Windows) : memoryleaks.log, memoryalloc.log et memorydealloc.log</li>
 *<li>La classe TransformationMatrix n'existe plus, il ne s'agit plus que d'une collection de fonctions sur matrices carrées 4x4.</li>
 *<li>Modification de la gestion des SceneNode : Une classe de base NodeBase qui contient les informations qui se trouvaient dans SceneNode, 3 classes dérivées (GeometryNode, CameraNode et LightNode) qui s'occupent des fonctionnalités spécifiques au rendu de chacun des types liés (respectivement Geometry, Camera et Light).</li>
 *<li>La classe MovableObject a maintenant moins de fonctionnalités (en fait elle n'a plus qu'un nom et un NodeBase) et les classes Light et Camera dérivent maintenant de cette classe (pour plus d'uniformité dans les déplacements de ces objets par rapport aux géométries)</li>
 *<li>Renommage des classes UniformVariable en FrameVariable, pour refléter plus ce que c'est et moins lier ce genre de choses à OpenGL.</li>
 *<li>Le module OpenGL 3.x/4.x est achevé.</li>
 *<li>Modifier la structure des vertex de façon à ce qu'ils incluent les coordonnées de texture et les normales directement, de façon à n'avoir qu'1 buffer de vertex par mesh au lieu de 3 actuellement.</li>
 *</ul>
 *
 *\subsection Corrections
 *<ul>
 *<li>Correction du plantage à la fermeture du soft en release uniquement.</li>
 *<li>Correction du rendu noir avec OpenGL 3.x, corrigé je ne sais trop comment, mais grâce à la suppression progressive du pipeline fixe en OpenGL 2.x</li>
 *</ul>
 *
 *\section version_0_6_0_0 Version 0.6.0.0
 *
 *\subsection Ajouts
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
 *\subsection Modifications
 *<ul>
 *<li>Modification de la gestion des threads et mutexes, permettant le choix de ceux qu'on utilise (Win32, MFC ou boost).</li>
 *<li>Modification des classes de gestion des matrices et points.</li>
 *<li>Modification de la classe de gestion des chaînes de caractères qui sont maintenant indépendantes de Unicode ou MultiByte.</li>
 *<li>Modification de la classe de gestion de fichiers qui ne permet plus le choix IO ou Stream mais utilise Stream uniquement.</li>
 *<li>Modification du render system, ajout d'une classe renderable qui se charge de créer le renderer associé, afin de simplifier la création des dérivés de cette classe (submesh, scene node, light, camera...)</li>
 *<li>Changement des noms des fichiers de scène/materiaux/meshes du moteur (respectivement cscn/cmtl/cmsh)</li>
 *</ul>
 *
 *\subsection Corrections
 *<ul>
 *<li>Protection par recursive mutex des données partagées (par les managers, essentiellement) permettant d'éviter des plantages aléatoires.</li>
 *</ul>
 *
 *\subsection A faire
 *<ul>
 *<li>Il reste un plantage à la fermeture du soft en release uniquement, apparemment dû aux shaders.</li>
 *<li>Pour GL3RenderSystem, j'ai un rendu noir (?!?) mais les transformations sont prises en compte correctement.</li>
 *</ul>
 *
 *\section version_0_5_0_1 Version 0.5.0.1
 *
 *\subsection Ajouts
 *<ul>
 *<li>Dans cette version l'espace tangent est calculé et est disponible dans les shaders GLSL.</li>
 *<li>Seule la tangente est calculée, la bitangente pouvant l'être dans le shader avec un produit vectoriel entre la tangente et la normale.</li>
 *<li>Pour accéder à la tangente, il suffit de déclarer l'attribut suivant dans le vertex shader :</li>
 *<li>attribute vec3 tangent;</li>
 *</ul>
 *
 *\subsection Modifications
 *<ul>
 *<li>Les différentes classes de point (2D, 3D, 4D) ont leur membres en union, ce qui permet de personnaliser l'appel de ces membres. Ainsi un point 2D a comme membres (x, y) ou (u, v). Un point 3D a comme membres (x, y, z) ou (u, v, w) ou (r, g, b). Un point 4D a comme membres (x, y, z, w) ou (r, g, b, a) ou (left, top, right, bottom).</li>
 *</ul>
 *
 *\subsection Corrections
 *<ul>
 *<li>Correction du plantage à la fermeture de l'application lors de la suppression des VBOs. J'ai intégré un buffer manager qui les supprime pendant une boucle d'affichage.</li>
 *</ul>
 */

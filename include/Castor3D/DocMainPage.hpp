/*! \mainpage Castor3D Engine
 *
 * \section intro_sec Suivi de versions
 *
 * \section version_0_6_1_2 Version 0.6.1.2
 *
 * \subsection Ajouts
 * Création du renderer Dx9Renderer, pour le rendu sous Direct3D 9. Il reste des choses à régler, notamment au niveau shaders et matrices, mais l'essentiel est là.
 * Création de différents fichiers pour les contextes OpenGL (X11, Gtk, Msw), je n'ai pas encore pu tester ça correctement, mes drivers ATI ayant tué OpenGL sur ma Debian (snif...).
 * Création d'une boîte de dialogue de sélection de renderer au lancement de CastorViewer, CastorShape et CastorArchitect, ajout d'un splash screen pour ces 3 applis.
 * Ajout d'une petite appli qui transforme n'inmporte quel type d'image en XPM, plus facile pour le développement multiplateforme avec wxWidgets.
 * Ajout de nouvelles fonctionnalités aux fichiers de scène (alpha_rejection, rgb_blend, alpha_blend, hl_shader_program, ...)
 *
 * \subsection Modifications
 * Optimisation du fichier CMakeLists et des fichiers du dossier 'cmake_modules' de façon à mieux prendre en charge les différentes plateformes, ainsi que les différentes configurations des PC clients.
 * Revue du système de VertexBuffers et IndexBuffers, in troduction de la notion de VertexDeclaration et BufferElement, ainsi on peut créer des VertexBuffers en mettant à peu près ce que l'on veut comme données dedans (parmi vertex, normals, tangents, diffuse, texcoords0,1,2,3) et ordonnées comme on veut.
 * La classe Vertex dérive donc maintenant de BufferElement.
 * Les TextureEnvironment vont disparaitre à la prochaine version, la gestion du multitexturing se faisant maintenant directement au niveau des passes et des texture units.
 * Suppression des fonctions issues de GLU, afin d'avoir une librairie de moins à linker.
 *
 * \subsection Bugs
 * Problème avec les matrices pour Dx9RenderSystem (je pense à la différence right-handed / left-handed, mais mes premiers tests à ce niveau ne sont pas concluants).
 * Problème avec les shaders HLSL, je n'ai pour l'instant pas d'affichage, je pense que c'est lié aux problèmes de matrices que je rencontre avec Direct3D.
 * Je n'ai pas avancé sur les shaders Cg et ai donc toujours les mêmes problèmes qu'auparavant.
 *
 * \subsection Dépendances
 * Pour compiler CastorUtils, vous aurez besoin de boost (date-time, filesystem et system), FreeImage et freetype2.
 * Pour compiler Castor3D, vous aurez besoin de boost (date-time, filesystem et system), FreeImage, freetype2 et Cg.
 * Pour compiler OpenGlCommon, Gl2RenderSystem et Gl3RenderSystem vous aurez besoin de OpenGL, Glew et CgGL.
 * Pour compiler Dx9RenderSystem, il vous faudre avoir un DirectXSDK (testé avec Septembre 2006 et Aout 2009)
 * Pour compiler GuiCommon, CastorShape, CastorViewer et CastorArchitect, vous aurez besoin de wxWidgets.
 *
 * \section version_0_6_1_1 Version 0.6.1.1
 *
 * \subsection Ajouts
 *Création d'un fichier CMakeLists pour générer la solution à partir de CMake.
 *Réorganisation des fichiers du dossier afin de mieux se prêter à ça.
 *Séparation de la classe Plugin en 3 types différents : Renderer, Divider et Importer. Ainsi on pourra ajouter de nouveaux types de plugin aisément et l'ajout d'importers et de subdiviseurs se fera sans devoir recompiler tous les projets.
 *Ca entraîne la création d'un projet par importeur existant (OBJ, PLY, MD2, MD3, 3DS, ...)
 *PNTrianglesDivider reste dans le Castor car utilisé par la primitive Icosaedron, LoopDivider est quand à lui placé dans un plugin.
 *J'ai commencé à implémenter les shaders Cg. Ca implique une modification des fichiers de scène, car dans Cg les variables uniform sont liées à un programme (vertex, pixel, geometry) plutôt qu'au shader complet comme c'est le cas en GLSL. Il reste quelques trucs à revoir (je ne sais pas encore quoi) pour que ça fonctionne.
 *
 * \subsection Modifications
 *Suppression de FreeImagePlus, maintenant les projets ne nécessitent plus que FreeImage.
 *La création des Buffers utilisés pour le rendu (Vertex, Attribute, Texture, Normals, ...) se fait maintenant par le RenderSystem => on peut donc les créer dans Castor3D et pas seulement dans les renderer (GL2, GL3 et dans le futur D3D).
 *Suppression des Singletons pour les différents Managers (Image, Font, Scene, Material, Animation et Mesh).
 *J'ai retouché les SceneNodes, j'ai refusionné les 3 classes différentes en une seule pour des commodités d'utilisation.
 *Création d'une classe IdPoint, qui étend Point et y ajoute un Index qui correspond à son index dans le submesh. La fonction AddPoint de Submesh crée maintenant des IdPoint et la fonction AddFace se sert d'eux pour générer les Vertex.
 *Les subdiviseurs peuvent subdiviser dans un thread à part, permettant de ne pas figer une application pendant la subdivision (cf. CastorShape). Il est possible d'assigner une fonction qui sera lancée à la fin de la subdivision (cf. CastorShape).
 *
 * \subsection Corrections de Bugs
 *Correction du LoopDivider qui ne repositionnait plus les Vertex (les nouveaux étaient créés mais plus replacés, dû à la nouvelle architecture des Vertex)
 *
 * \subsection Dépendances
 *Pour compiler CastorUtils et Castor3D, vous aurez besoin de boost, FreeImage
 *Pour compiler OpenGLCommon, GL2RenderSystem et GL3RenderSystem vous aurez besoin de OpenGL, GLU et GLEW
 *Pour compiler CastorShape et CastorViewer, vous aurez besoin de wxWidget
 *
 * \section version_0_6_1_0 Version 0.6.1.0
 * 
 * \subsection Ajouts
 *Utilisation des smart pointers tr1.
 *Création d'un tronc commun 'OpenGLCommon' pour les 2 renderers OpenGL
 *Création d'une classe de Pipeline qui effectue les calculs matriciels et d'autres petites fonctions (perspective, ortho, ...)
 *
 * \subsection Modifications
 *Le MemoryManager (activé uniquement en debug) a été modifié, ajout d'une classe template MemoryTraced qui surcharge les operateurs new, delete new [] et delete [], ce afin d'avoir une meilleure trace des objets créés et détruits. Chaque classe des projets du Castor dérivent maintenant de celle-ci, et 3 fichiers sont donc créés dans le répertoire racine (C:\ sous Windows) : memoryleaks.log, memoryalloc.log et memorydealloc.log
 *La classe TransformationMatrix n'existe plus, il ne s'agit plus que d'une collection de fonctions sur matrices carrées 4x4.
 *Modification de la gestion des SceneNode : Une classe de base NodeBase qui contient les informations qui se trouvaient dans SceneNode, 3 classes dérivées (GeometryNode, CameraNode et LightNode) qui s'occupent des fonctionnalités spécifiques au rendu de chacun des types liés (respectivement Geometry, Camera et Light).
 *La classe MovableObject a maintenant moins de fonctionnalités (en fait elle n'a plus qu'un nom et un NodeBase) et les classes Light et Camera dérivent maintenant de cette classe (pour plus d'uniformité dans les déplacements de ces objets par rapport aux géométries)
 *Renommage des classes UniformVariable en FrameVariable, pour refléter plus ce que c'est et moins lier ce genre de choses à OpenGL.
 *Le module OpenGL 3.x/4.x est achevé.
 *Modifier la structure des vertex de façon à ce qu'ils incluent les coordonnées de texture et les normales directement, de façon à n'avoir qu'1 buffer de vertex par mesh au lieu de 3 actuellement.
 *
 * \subsection Corrections de Bugs
 *Correction du plantage à la fermeture du soft en release uniquement.
 *Correction du rendu noir avec OpenGL 3.x, corrigé je ne sais trop comment, mais grâce à la suppression progressive du pipeline fixe en OpenGL 2.x
 *
 * \subsection Dépendances
 *Pour compiler CastorUtils et Castor3D, vous aurez besoin de boost, FreeImage et FreeImagePlus
 *Pour compiler OpenGLCommon, GL2RenderSystem et GL3RenderSystem vous aurez besoin de GLEW
 *Pour compiler CastorShape et CastorViewer, vous aurez besoin de wxWidget
 *
 * \section version_0_6_0_0 Version 0.6.0.0
 *
 * \subsection Ajouts
 *Ajout d'une classe de gestion des matrices de projection, normale et visualisation, en vue de la suppression du pipeline fixe dans les renderers OpenGL.
 *Ajout de smart pointers persos, inspirés de ceux de boost.
 *Ajout d'une classe de gestion des angles, permettant de ne pas s'embeter avec les conversions radian-degrés.
 *Ajout d'une classe de gestion de path, permettant de gérer les slash ou anti-slash selon l'OS.
 *Création de conteneurs thread-safe issus de la stl (vector, list, map, multimap, set).
 *Séparation du renderer OpenGL en 2 projets : un compatible OpenGL 2.x et antérieur (GL2RenderSystem) le second compatible OpenGL3.x et ultérieur (GL3RenderSystem).
 *GL2RenderSystem : en fonction du PC client, suppression du pipeline fixe ou pas (si l'utilisation des shaders est permise le pipeline fixe est supprimé, sinon il est gardé)
 *
 * \subsection Modifications
 *Modification de la gestion des threads et mutexes, permettant le choix de ceux qu'on utilise (Win32, MFC ou boost).
 *Modification des classes de gestion des matrices et points.
 *Modification de la classe de gestion des chaînes de caractères qui sont maintenant indépendantes de Unicode ou MultiByte.
 *Modification de la classe de gestion de fichiers qui ne permet plus le choix IO ou Stream mais utilise Stream uniquement.
 *Modification du render system, ajout d'une classe renderable qui se charge de créer le renderer associé, afin de simplifier la création des dérivés de cette classe (submesh, scene node, light, camera...)
 *Changement des noms des fichiers de scène/materiaux/meshes du moteur (respectivement cscn/cmtl/cmsh)
 *
 * \subsection Corrections de Bugs
 *Protection par recursive mutex des données partagées (par les managers, essentiellement) permettant d'éviter des plantages aléatoires.
 *
 * \subsection A faire
 *Il reste un plantage à la fermeture du soft en release uniquement, apparemment dû aux shaders.
 *Pour GL3RenderSystem, j'ai un rendu noir (?!?) mais les transformations sont prises en compte correctement.
 *
 * \subsection Dépendances
 *Pour compiler CastorUtils et Castor3D, vous aurez besoin de boost, FreeImage, FreeImagePlus et glm (OpenGL Mathematics)
 *Pour compiler GL2RenderSystem et GL3RenderSystem vous aurez besoin de GLEW
 *Pour compiler CastorShape et CastorViewer, vous aurez besoin de wxWidget
 *
 * \section version_0_5_0_1 Version 0.5.0.1
 * 
 * \subsection Ajouts
 *Dans cette version l'espace tangent est calculé et est disponible dans les shaders GLSL.
 *Seule la tangente est calculée, la bitangente pouvant l'être dans le shader avec un produit vectoriel entre la tangente et la normale.
 *Pour accéder à la tangente, il suffit de déclarer l'attribut suivant dans le vertex shader :
 *attribute vec3 tangent;
 *
 * \subsection Modifications
 *Les différentes classes de point (2D, 3D, 4D) ont leur membres en union, ce qui permet de personnaliser l'appel de ces membres. Ainsi un point 2D a comme membres (x, y) ou (u, v). Un point 3D a comme membres (x, y, z) ou (u, v, w) ou (r, g, b). Un point 4D a comme membres (x, y, z, w) ou (r, g, b, a) ou (left, top, right, bottom).
 *
 * \subsection Corrections de Bugs
 *Correction du plantage à la fermeture de l'application lors de la suppression des VBOs. J'ai intégré un buffer manager qui les supprime pendant une boucle d'affichage.
 */
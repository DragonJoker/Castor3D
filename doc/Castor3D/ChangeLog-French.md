Suivi de versions	{#changelog}
=================

Cette page sert à informer des évolutions du moteur depuis les premières versions.

## Version 0.12.0

### Important Changes
- Correction de la majorité des warnings de compilation.
- Utilisation de RenderGraph pour la construction du frame graph.
- Utilisation des bindless textures.
- Implémenté le MultiDrawIndirect.
- Implémentation d'un écran de chargement.
- Implémentation des animations de textures.
- Implémentation des animations de scene nodes.

### New features

#### Generic
- Ajout du plugin DrawEdges, pour détecter et dessiner les bordures.
- Implémentation d'un plugin de rendu basique d'océan.
- Implémentation d'un plugin de rendu d'océan basé sur les Fast Fourier Transform.
- Implémentation d'un plugin de rendu d'eau sur surface plane.
- Implémentation d'une collection de matériaux toon.

#### ToneMappings
- Ajout d'un plugin de tone mapping ACES.

#### Tools
- Ajout de l'application HeightMapToNormalMap.

### Modifications

#### CastorUtils
- Ajout du support des tile maps.
- Ajout de la conversion depuis array map vers tile mapp
- Ajout d'AsyncJobQueue, basée sur ThreadPool.
- Ajout de GliImageWriter.
- FreeImageLoader est maintenant optionnel.
- Ajout de la compression en BC1/BC3 lors du chargement des images, en utilisant cvtt.
- Ajout d'une implémentation de SpinMutex.
- TextWriter écrit maintenant dans un castor::StringStream.
- Génération des mipmaps lors du chargement des images.
- Intégration de la gestion des parsers additionnels dans FileParser.
- Le parsing des brace files se fait maintenant en deux étapes.

#### Castor3D
- Ajout du frustum culling pour les point lights shadow maps.
- Ajout du support de plusieurs rendering devices.
- Tous les post effects sont maintenant créés avec la render target, pour être activables/désactivables à l'envi.
- Ajout de l'import de scène en plus de l'import de mesh.
- Ajout d'une passe Blinn-Phong.
- Généralisation des shaders, en passant par des classes intermédiaires : LightingModel, ReflectionModel et LightMaterial.
- Ajout d'un TextureCache, utilisé pour fusionner les images utilisées par une passe.
- Ajout d'un moyen d'avoir des objets non sélectionnables.
- Ajout d'options de rotation de mesh lors de l'import.
- Ajout du support d'un second set d'UV.
- Ajout du support de render nodes passes custom.
- Ajout du support des tessellation shaders pour les render nodes passes.
- Ajout du support des material passes non affectées par l'éclairage.
- Ajout du support des safe bands dans Camera.
- Ajout de la possibilité de remapper les composantes d'une texture lors de l'import d'une scène.
- La depth prepass écrit maintenant aussi les normales.
- Implémenté les LPV pour les point lights.
- Les Events peuvent maintenant être ignorés.
- Fusion des LightingModels PBRMR et PBRSG en interne.
- Ajout de la possibilité d'exclure des objects des tests de culling.
- Suppression de la correction gamma pour les textures, en utilisant les formats SRGB.
- Suppression des PassBuffers spécifiques.
- Réorganisation de l'écriture et lecture des Pass dans les fichiers de scène.
- Déplacement des profils de SSS dans leur propre shader buffer.

#### CastorGui
- Ajout des classes de style, afin d'en avoir une meilleure gestion.

#### DiamondSquareTerrain
- Ajout d'une option permettant de désactiver la random seed.
- Ajout d'une option permettant de tiler les UV.

#### AssimpImporter
- Améliorations relatives à l'import des matériaux.
- Amélioration de l'import des animations.
- Ajout de l'import des scene nodes et sources lumineuses.

#### GuiCommon
- Ajout de l'affichage de la configuration des render nodes passes custom.
- La configuration des PostEffects a été ajoutée à l'arbre.
- Suppression de la liste de matériaux, et de doublons dans la vue de scène.

### Fixed Bugs

#### CastorUtils
- Correction de l'inversion de Matrix2x2 et Matrix3x3.
- Correction de l'allocation alignée sous Windows.

#### Castor3D
- Correction d'un crash lorsqu'une keyframe de skinning manque.
- Correction de la background pass par rapport au brouillard.
- Correction de l'interaction entre l'indirect diffuse et les réflexions.
- Correction d'un crash en release avec le morphing.
- Correction de l'import/export des animations de morphing.
- Correction de l'éclairage indirect dans les forward passes.
- Correction des point shadow maps.
- Correction des spot shadow maps.
- Correction du traitement des cleanup event dans la render loop.
- Correction de la création de render target creation, afin de lui donner les bonnes taille et pixel format.
- Correction de la réinitialisation de la swapchain lors de l'initialisation de la RenderWindow.
- Correction du changement au runtime du tone mapping.

### GuiCmmon
- Correction d'un crash dans LanguageFileParser.
- Correction d'un crash lors du changement de l'algo de GI d'une source lumineuse.
- Correction des crashes lors du chargement des images HDR.
- Correction de l'affichage des propriétés des textures.

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

- Suppression de libzip, et ajout de minizip aux sources.

#### Castor3D

- Revue complète de la boucle de rendu : RenderPipeline n'est plus une instance unique, mais contient les états, le programme et les bindings des uniform buffers.
  Cela a été fait afin de préparer l'arrivée de Vulkan, pour lequel le pipeline est immuable, et c'est maintenant le cas pour Castor3D aussi.
- Le rendu a été divisé en RenderPasses, qui ont chacune une ou plusieurs RenderQueues, qui sont mises à jour de manière asynchrone sur le CPU.
  Cela permet à la boucle GPU d'^être débarassée de la plupart des calculs CPU.
- Ajout des Skybox.
- L'export binaire a été complètement revu, il utilise maintenant un format basé sur des chunks (comme LWO ou 3DS).
  Il est utilisé pour les maillages et les squelettes.
- Amélioration de la cohérence de l'interface des plugins.
- Implémentation de brouillards basiques (linéaire, exponentiel, exponentiel au carré).
- Ajout du support des animations de type morphing (par vertex).
- Implémentation du frustum culling.
- Implémentation du colour picking.
- Ajout du shadow mapping, les lumières peuvent produire des ombres, et les objets peuvent projeter des ombres.
- Implémentation de systèmes de particules, en donnant le choix de l'implémentation à l'utilisateur (CPU, Transform Feedback, ou Compute shaders).
- Correction de l'éclairage en GLSL, pour supprimer les dirty hacks.
- Ajout du support des compute shaders, avec les shader storage buffers et les atomic counter buffers.
- L'implémentation des textures a été complètement retravaillée, il y a maintenant TextureLayout, qui contient les TextureViews, et le TextureStorage par image.
  Cela a permis la création des textures cube.
- Correction du tranfert des textures en RAM.
- Le rendu des billboards n'utilise plus de geometry shader, il se base maintenant sur l'instantiation d'un quad, et les positions sont donnés en attributs d'instance.
- UniformBuffer (précédemment nommé FrameVariableBuffer) ne dépend plus de ShaderProgram.
- Une nouvelle classe, UniformBufferBinding a été créée, dépendant elle de ShaderProgram et UniformBuffer.
  Les instances de cette classe sont contenues dans RenderPipeline.
- La classe FrameVariable a été divisée en deux classes : Unidorm (pour les variables contenues dans un UniformBuffer) et PushUniform (pour les autres variables uniformes).

#### GlRenderSystem

- Mise à jour pour supporter les features fournies par Castor3D.

#### Techniques

- DeferredRenderTechnique et DeferredMsaaRenderTechnique sont maintenant de véritables techniques de rendu différé, avec une light pass par source lumineuse, en prenant en compte leur aire d'impact.
- DeferredRenderTechnique est maintenant la technique de rendu par défaut.

#### PostEffects

- Ajout de GrayScale.
- Ajout de FXAA.

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

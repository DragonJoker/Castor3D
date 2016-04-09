/*!
 *\~english
 *\mainpage CastorUtils Library
 *\section intro_sec Introduction
 *This library contains some helper classes to ease the development of Castor3D engine.<br>
 *\section summ Summary
 *\subsection data Data
 *<ul>
 *<li>File management base class</li>
 *<li>BinaryFile and TextFile specialisations</li>
 *<li>FileParser, a brace file parser, along with its FileParserContext</li>
 *<li>Loader base class</li>
 *<li>BinaryLoader and TextLoader specialisations</li>
 *<li>ZipArchive <b>(new in 0.8.0)</b></li>
 *</ul>
 *\subsection dsgn Design
 *<ul>
 *<li>Collection, a thread-safe container</li>
 *<li>Factory</li>
 *<li>Named</li>
 *<li>NonCopyable</li>
 *<li>Unique</li>
 *</ul>
 *\subsection gfx Graphics
 *<ul>
 *<li>Colour and ColourComponent</li>
 *<li>ContainerBox, CubeBox and SphereBox</li>
 *<li>Font, Glyph</li>
 *<li>Image</li>
 *<li>PixelFormat, Pixel and PxBuffer, to manage pixel conversions</li>
 *<li>Position, Size</li>
 *</ul>
 *\subsection math Mathematics
 *<ul>
 *<li>Angle conversions</li>
 *<li>Coords, to have a point class which doesn't own its data</li>
 *<li>Point, to have a point class which owns its data</li>
 *<li>Line (2D and 3D) equations</li>
 *<li>Matrix and SquareMatrix</li>
 *<li>Plane equations</li>
 *<li>Quaternion</li>
 *<li>SphericalVertex</li>
 *<li>Rectangle</li>
 *<li>Template math functions (such as exp, ln, ...)</li>
 *</ul>
 *\subsection misc Miscellaneous
 *<ul>
 *<li>BlockTracker and BlockTimer</li>
 *<li>Console and Logger</li>
 *<li>PreciseTimer</li>
 *<li>DynamicLibrary</li>
 *<li>Resource</li>
 *<li>DynamicLibrary</li>
 *</ul>
 *\section Installation
 *\subsection step1 Step 1 : Downloading the source
 *The project sources are available <a href="http://projets.developpez.com/attachments/download/736/Castor.zip">here</a> or on SourceForge GIT server : http://subversion.developpez.com/projets/Castor3D<br>
 *\subsection step2 Step 2 : Compilation
 *When sources are downloaded, you must use CMake (available <a href="http://www.cmake.org/cmake/resources/software.html">here</a>) to generate the project file most suitable to your system (Makefile, Code::Blocks, Visual Studio, ...)<br>
 *You'll need additional libraries :<br>
 *<ul>
 *<li>FreeImage</li>
 *<li>FreeType 2</li>
 *<li>libzip</li>
 *<li>boost::thread, if your compiler doesn't support std::thread</li>
 *</ul>
 *Once you downloaded, compiled and installed the dependencies, you can compile CastorUtils.
 *\~french
 *\mainpage Bibliothèque CastorUtils
 *\section intro_sec Introduction
 *Cette bibliothèque contient des classes générales aidant au développement du moteur Castor3D<br>
 *\section summ Résumé
 *\subsection data Données
 *<ul>
 *<li>Gestion de fichiers, binaires ou texte (BinaryFile et TextFile)</li>
 *<li>FileParser, un analayseur de brace file, avec son FileParserContext</li>
 *<li>Classes de chargement de données binaires ou texte (BinaryLoader et TextLoader)</li>
 *<li>ZipArchive<b>(0.8.0)</b></li>
 *</ul>
 *\subsection dsgn Concepts
 *<ul>
 *<li>Collection, un conteneur thread-safe</li>
 *<li>Factory</li>
 *<li>Named</li>
 *<li>NonCopyable</li>
 *<li>Unique</li>
 *</ul>
 *\subsection gfx Graphisme
 *<ul>
 *<li>Colour et ColourComponent</li>
 *<li>ContainerBox, CubeBox et SphereBox</li>
 *<li>Font, Glyph</li>
 *<li>Image</li>
 *<li>PixelFormat, Pixel et PxBuffer, pour les conversions de pixels</li>
 *<li>Position, Size</li>
 *</ul>
 *\subsection math Mathématiques
 *<ul>
 *<li>conversions d'angle</li>
 *<li>Coords, une classe de point non propriétaire de ses données</li>
 *<li>Point, une classe de point propriétaire de ses données</li>
 *<li>Equations de droites (2D et 3D)</li>
 *<li>Equations de plans</li>
 *<li>Matrix et SquareMatrix</li>
 *<li>Quaternion</li>
 *<li>SphericalVertex</li>
 *<li>Rectangle</li>
 *</ul>
 *\subsection misc Divers
 *<ul>
 *<li>BlockTracker et BlockTimer</li>
 *<li>Console et Logger</li>
 *<li>PreciseTimer</li>
 *<li>DynamicLibrary</li>
 *<li>Resource</li>
 *<li>DynamicLibrary</li>
 *</ul>
 *\section install_sec Installation
 *\subsection step1 1ère étape : Téléchargement des sources
 *Les sources sont disponibles <a href="http://projets.developpez.com/attachments/download/736/Castor.zip">ici</a> ou accessibles via SVN, sur le serveur de Développez.com : http://subversion.developpez.com/projets/Castor3D<br>
 *\subsection step2 2ème étape : Compilation
 *Une fois les sources téléchargées, vous devez utiliser CMake (disponible <a href="http://www.cmake.org/cmake/resources/software.html">ici</a>) pour générer le type de projet qui vous conviendra (Makefile, Code::Blocks, Visual Studio, ...)<br>
 *Vous remarquerez que vous avez besoin de bibliothèques supplémentaires :<br>
 *<ul>
 *<li>FreeImage</li>
 *<li>FreeType 2</li>
 *<li>boost::thread</li>
 *</ul>
 *Une fois les dépendances téléchargées, compilées et installées, vous n'avez plus qu'à compiler CastorUtils
 */

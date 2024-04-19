# Générer Castor3D

Castor3D utilise CMake pour générer les fichiers de projets.<br />

Vous devez donc lancer CMake pour générer les fichiers de projet pour la solution que vous préférez.<br />

La génération de projets s'appuie sur vcpkg pour la gestion des dépendances.<br />

La documentation de CastorUtils et Castor3D peut être générée via Doxygen.<br />

Les compilateurs testés sont les suivants :
- Visual Studio 2022
- gcc 12.0
- clang 14.0

## Options

La génération est configurable dans CMake, via les options suivantes :
- **PROJECTS_BINARIES_OUTPUT_DIR**

	Utilisé pour définir le dossier de base des binaires finaux.
	Dans ce dossier, les sous-dossiers bin, lib et share sont créés,
	et les binaires sont placés dans les sous-dossiers appropriés.
	Certains fichiers seront aussi copiés dans le sous-dossier
	share (les traductions, par exemple).
	Sur Windows, les DLL externes seront copiées dans le dossier bin,
	si elles sont trouvées.

- **PROJECTS_GENERATE_DOC**

	Utilisé pour activer/désactiver la création des projets de génération de documentation.

- **PROJECTS_UNITY_BUILD**

	Utilisé pour activer/désactiver l'utilisation des Jumbo/Unity builds
	lors de la compilation.

- **PROJECTS_USE_PRECOMPILED_HEADERS**

	Utilisé pour activer/désactiver l'utilisation des Precompiled Headers
	lors de la compilation.

- **PROJECTS_USE_PRETTY_PRINTING**

	Si cette option est activée, et si A-Style est trouvé, de nouveaux projets
	de test seront créés, permettant de lancer A-Style sur les sources associés
	à ces projets.

- **PROJECTS_WARNINGS_AS_ERRORS**

	Si cette option est activée, les warnings de compilation seront traités comme des erreurs.

- **CASTOR_BUILD_CASTOR3D**

	Cette option particulière vous permet de désactiver le build de Castor3D,
	et de tout projet en dépendant, résultant en la génération de CastorUtils
	et CastorUtilsTest uniquement.

- **CASTOR_BUILD_PLUGINS**

	Une autre option particulière, permettant de désactiver la génération des plugins.

- **CASTOR_C3D_NEEDS_GLSL**

	Permet d'activer le support de l'import de shaders en GLSL dans le moteur.

- **CASTOR_USE_FREEIMAGE**

	Permet de charger les images PNG via FreeImage.

- **CASTOR_USE_GLSLANG**

	Permet d'activer le support de l'import de shaders en GLSL dans le moteur.

- **CASTOR_USE_MESH_SHADERS**

	Permet d'activer l'utilisation des mesh shaders.

- **CASTOR_USE_TASK_SHADERS**

	Permet d'activer l'utilisation des task shaders.

- **CASTOR_BUILDGRP_DEMO**

	Active la génération des applications d'exemple.

- **CASTOR_BUILDGRP_GENERATOR**

	Active la génération des plugins de type Generator.

- **CASTOR_BUILDGRP_GENERIC**

	Active la génération des plugins de type Generic.

- **CASTOR_BUILDGRP_IMPORTER**

	Active la génération des plugins de type Importer.

- **CASTOR_BUILDGRP_INTEROP**

	Active la génération des bindings avec d'autres langages.

- **CASTOR_BUILDGRP_PARTICLES**

	Active la génération des plugins de type Particle.

- **CASTOR_BUILDGRP_POSTFX**

	Active la génération des plugins de type PostEffect.

- **CASTOR_BUILDGRP_SETUP**

	Active la génération des projets de setup.

- **CASTOR_BUILDGRP_TEST**

	Active la génération des applications de test.

- **CASTOR_BUILDGRP_TONEMAPS**

	Active la génération des plugins de type ToneMapping.
	(Le plugin LinearToneMapping sera tout de même généré, étant celui utilisé par défaut).

- **CASTOR_BUILDGRP_TOOL**

	Active la génération des applications utilitaires.

- **CASTOR_BUILD_DEMO_(NAME)**

	Active la génération de l'application d'exemple (NAME).

- **CASTOR_BUILD_GENERATOR_(NAME)**

	Active la génération du plugin Generator (NAME).

- **CASTOR_BUILD_GENERIC_(NAME)**

	Active la génération du plugin Generic (NAME).

- **CASTOR_BUILD_IMPORTER_(NAME)**

	Active la génération du plugin Importer (NAME).

- **CASTOR_BUILD_INTEROP_(NAME)**

	Active la génération du binding (NAME).

- **CASTOR_BUILD_POSTFX_(NAME)**

	Active la génération du plugin PostEffect (NAME).

- **CASTOR_BUILD_TONEMAP_(NAME)**

	Active la génération du plugin ToneMap (NAME).

- **CASTOR_BUILD_TEST_(NAME)**

	Active la génération de l'application de test (NAME).

- **CASTOR_BUILD_TOOL_(NAME)**

	Active la génération de l'application utilitaire (NAME).

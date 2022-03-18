# Générer Castor3D

Castor3D utilise CMake pour générer les fichiers de projets.<br />

Vous devez donc lancer CMake pour générer les fichiers de projet pour la solution que vous préférez.<br />

La documenation de CastorUtils et Castor3D peut être générée via Doxygen.<br />

Les compilateurs testés sont les suivants :
- Visual Studio 2022
- gcc 10.0
- clang 12.0

## Projets

### Projets Principaux

- CastorUtils: Dépend de zlib et de freetype.
- Castor3D: Dépend de CastorUtils, Ashes et ShaderWriter.
- GuiCommon: Dépend de Castor3D et wxWidgets (depuis la version 2.9.5, actuellement la version 3.1).
- CastorViewer: Dépend de GuiCommon et wxWidgets.

### Plug-ins

#### Importers

  - ASSIMP: Dépend de Castor3D et Assimp.
  - PLY: Dépend de Castor3D.

#### Dividers

  - Loop: Dépend de Castor3D.
  - Phong: Dépend de Castor3D.
  - PN-Triangles: Dépend de Castor3D.

#### PostEffects

  - Bloom: Dépend de Castor3D.
  - DrawEdges: Dépend de Castor3D.
  - FilmGrain: Dépend de Castor3D.
  - FXAA: Dépend de Castor3D.
  - GrayScale: Dépend de Castor3D.
  - LightStreaks: Dépend de Castor3D.
  - LinearMotionBlur: Dépend de Castor3D.
  - SMAA: Dépend de Castor3D.

#### Generators

  - DiamondSquareTerrain: Dépend de Castor3D.

#### Generic

  - CastorGUI: Dépend de Castor3D.
  - FFTOceanRendering: Dépend de Castor3D.
  - OceanRendering: Dépend de Castor3D.
  - ToonMaterial: Dépend de Castor3D.
  - WaterRendering: Dépend de Castor3D.

#### Particles

  - FireworksParticle: Dépend de Castor3D

#### ToneMappings

  - ACESToneMapping: Dépend de Castor3D.
  - LinearToneMapping: Dépend de Castor3D.
  - HaarmPieterDuikerToneMapping: Dépend de Castor3D.
  - HejlBurgessDawsonToneMapping: Dépend de Castor3D.
  - ReinhardToneMapping: Dépend de Castor3D.
  - Uncharted2ToneMapping: Dépend de Castor3D.

### Projets de rest

- CastorTest: Bibliothèque de base pour les tests, dépend de CastorUtils..
- CastorUtilsTest: Vous permet de lancer des tests unitaires et de performances pour CastorUtils, dépend de CastorUtils et CastorTest.
- Castor3DTest: Vous permet de lancer les tests unitaires de Castor3D, dépend de Castor3D et CastorTest.

### Autres

- CastorMeshConverter: Dépend de Castor3D et wxWidgets.
- CastorMeshUpgrader: Dépend de Castor3D et wxWidgets.
- CastorTestLauncher: Dépend de Castor3D.
- ImgConverter: Depends on wxWidgets.
- HeightMapToNormalMap: Dépend de Castor3D et wxWidgets.

## Options

Sur GNU/Linux le dossier par défaut d'installation est /usr/local.

Vous pouvez changer ce dossier en modifiant la valeur CMAKE_INSTALL_PREFIX.

Les applications d'exemple, d'outils et de tests sont installées dans install_dir/bin/

Les bibliothèques principales (CastorUtils and Castor3D) sont installées dans install_dir/lib/ sur GNU/Linux, et dans install_dir/bin/ sur Windows.

Les plug-ins sont installés dans install_dir/lib/Castor3D/

La génération est configurable dans CMake, via les options suivantes :
- **PROJECTS_BINARIES_OUTPUT_DIR**

    Utilisé pour définir le dossier de base des binaires finaux.
    Dans ce dossier, les sous-dossiers bin, lib et share sont créés,
	et les binaires sont placés dans les sous-dossiers appropriés.
    Certains fichiers seront aussi copiés dans le sous-dossier
	share (les traductions, par exemple).
    Sur Windows, les DLL externes seront copiées dans le dossier bin,
	si elles sont trouvées.

- **PROJECTS_USE_PRECOMPILED_HEADERS**

    Utilisé pour activer/désactiver l'utilisation des Precompiled Headers
	lors de la compilation.

- **PROJECTS_USE_PRETTY_PRINTING**

    Si cette option est activée, et si A-Style est trouvé, de nouveaux projets
	de test seront créés, permettant de lancer A-Style sur les sources associés
	à ces projets.

- **CASTOR_BUILD_CASTOR3D**

    Cette option particulière vous permet de désactiver le build de Castor3D,
	et de tout projet en dépendant, résultant en la génération de CastorUtils
	et CastorUtilsTest uniquement.

- **CASTOR_BUILD_PLUGINS**

    Une autre option particulière, permettant de désactiver la génération des plugins.

- **CASTOR_BUILDGRP_DIVIDER**

    Active la génération des plugins de type Divider.

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

- **CASTOR_BUILDGRP_SAMPLE**

    Active la génération des applications d'exemple.

- **CASTOR_BUILDGRP_SETUP**

    Active la génération des projets de setup.

- **CASTOR_BUILDGRP_TEST**

    Active la génération des applications de test.

- **CASTOR_BUILDGRP_TONEMAPS**

    Active la génération des plugins de type ToneMapping.
    (Le plugin LinearToneMapping sera tout de même généré, étant celui utilisé par défaut).

- **CASTOR_BUILDGRP_TOOL**

    Active la génération des applications utilitaires.

- **CASTOR_BUILD_DIVIDER_(NAME)**

    Active la génération du plugin Divider (NAME).

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

- **CASTOR_BUILD_SAMPLE_(NAME)**

    Active la génération de l'application d'exemple (NAME).

- **CASTOR_BUILD_TONEMAP_(NAME)**

    Active la génération du plugin ToneMap (NAME).

- **CASTOR_BUILD_TEST_(NAME)**

    Active la génération de l'application de test (NAME).
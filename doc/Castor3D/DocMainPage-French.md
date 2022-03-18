Castor3D	{#mainpage}
========

Castor3D est un moteur 3D multi-plateformes écrit en C++17.

Il fonctionne sur les plateformes GNU/Linux et Windows.

Il est basé sur l'API de rendu Ashes et supporte donc les API OpenGL, Direct3D 11 et Vulkan.

Le projet en lui-même est compilable sur les plateformes supportées en utilisant CMake (cf. [génération](Build-French.md)).

## Fonctionnalités

- Deferred rendering pour les objets opaques
- Weighted Blended rendering pour les objets transparents.
- Normal mapping.
- Shadow Mapping (avec choix entre Raw, PCF ou Variance Shadow Maps).
- Parallax Occlusion Mapping.
- Screen Space Ambiant Occlusion (en implémentant le [Scalable Ambiance Obscurance](https://casual-effects.com/research/McGuire2012SAO/index.html)).
- Reflection/Refraction Mapping.
- Rendu PBR (Metallic/Roughness et Specular/Glossiness) et rendu Phong/Blinn-Phong.
- Rendu HDR avec application de divers opérateurs de tone mapping.
- Screen Space Subsurface Scattering (sans la backlit transmittance pour l'instant).
- Volumetric Light Scattering pour la source directionnelle, si elle projette des ombres.
- Cascaded Shadow Maps pour la source lumineuse directionelle.
- Global Illumination, via les Light Propagation Volumes (layered ou pas) ou le Voxel Cone Tracing.
- Frustum culling.
- Graphe de scène.
- Graphe de rendu.
- Architecture modulaire, à base de plug-ins.
- Génération automatique de shaders, en fonction de la configuration du pipeline.
- Ecriture du code du shader directement depuis le code C++.
- Scènes décrites dans un format texte facilement compréhensible et extensible. ([Scene File](SceneFile-French.md))
- Rendu synchrone (timers) ou asynchrone (thread).

## Plugins implémentés

### Importers

- ASSIMP, utilisant la bibliothèque assimp pour l'import d'une grande variété de formats.
- PLY, permettant l'import de gros maillage PLY.

### Dividers

- PN-Triangles: Subdivision en utilisant les surfaces PN-Triangles.
- Phong: Subdivision en utilisant la tessellation de Phong.
- Loop: Subdivision en utilisant les Loop subdivision surface.

### PostEffect

- Bloom : une implémentation de HDR Bloom.
- DrawEdges : Détecte et dessine les bordures, basé sur les normales, la profondeur, et/ou l'ID d'objet.
- FilmGrain : Pour afficher un grain sur le rendu.
- GrayScale.
- LightStreaks (utilisant Kawase Light Streaks).
- FXAA : un antialiasing rapide de qualité basique.
- SMAA : un antialiasing temporel de bonne qualité.
- Linear Motion Blur.

### Generators

- DiamondSquareTerrain : Génération de terrains, utilisant l'algorithme diamond square pour générer des reliefs.

### Generic

- CastorGui : Etend les fichiers de scène pour ajouter et dessiner des IHM (boutons, zones de texte, ...) via les incrustations 2D.
- ToonMaterial : Un ensemble de matériaux toon, à combiner avec le plugin DrawEdges.
- FFTOceanRendering : Permet le rendu d'océan en se basant sur les transformées de Fourier.
- OceanRendering : Rendu basique d'océan, en spécifiant les propriétés des vages.
- WaterRendering : Rendu de surfaces planes d'eau, basé sur l'utilisation de normal maps.

### ToneMapping

- Linear : l'opérateur par défaut.
- Haarm Pieter Duiker.
- Hejl Burgess Dawson (aussi appelé Filmic).
- Reinhard.
- Uncharted 2.
- ACES.

## Autres applications

- CastorViewer : Visualiseur de scènes utilisant Castor3D.
- CastorMeshConverter : Convertisseur de divers formats de fichier de maillaige vers le format Castor3D.
- CastorMeshUpgrader : Met à jour les fichiers de maillage utilisant un format Castor3D antérieur à la dernière version.
- ImgConverter : Projet de conversion de tout type d'images vers XPM ou ICO.

## Screenshots

![Sponza, VCT](http://dragonjoker.github.io/Castor3D/img/Sponza-PBR-VCT-Small.png){width=800px}
![Cerberus](http://dragonjoker.github.io/Castor3D/img/Cerberus-PBR-Small.png){width=800px}
![Park, Comparison](http://dragonjoker.github.io/Castor3D/img/Park-Small.png){width=800px}
![Nyra, PBR](http://dragonjoker.github.io/Castor3D/img/Nyra-PBR-MR-Small.png){width=800px}
![SanMiguel, PBR](http://dragonjoker.github.io/Castor3D/img/SanMiguel-PBR-SG-Small.png){width=800px}
![SanMiguel, PBR, LPV](http://dragonjoker.github.io/Castor3D/img/SanMiguel-PBR-SG-LPV-Small.png){width=800px}
![Bistro, PBR, VCT](http://dragonjoker.github.io/Castor3D/img/Bistro-PBR-VCT-Small.png){width=800px}

Voir le [changelog complet](ChangeLog-French.md) pour plus de détails.

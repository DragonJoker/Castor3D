Bibliothèque CastorUtils	{#mainpage}
========================

## Introduction

Cette bibliothèque contient des classes générales aidant au développement du moteur Castor3D.

## Résumé

### Données

- Gestion de fichiers, binaires ou texte (BinaryFile et TextFile)
- FileParser, un analayseur de brace file, avec son FileParserContext
- Classes de chargement de données binaires ou texte (BinaryLoader et TextLoader)
- ZipArchive

### Concepts

- Collection, un conteneur thread-safe
- Factory
- Named
- NonCopyable
- Unique

### Graphisme

- Colour et ColourComponent
- ContainerBox, CubeBox et SphereBox
- Font, Glyph
- Image
- PixelFormat, Pixel et PxBuffer, pour les conversions de pixels
- Position, Size

### Mathématiques

- conversions d'angle
- Coords, une classe de point non propriétaire de ses données
- Point, une classe de point propriétaire de ses données
- Equations de droites (2D et 3D)
- Equations de plans
- Matrix et SquareMatrix
- Quaternion
- SphericalVertex
- Rectangle
- Fonctions de math template

### Divers

- BlockTracker et BlockTimer
- Console et Logger
- PreciseTimer
- DynamicLibrary
- Resource
- DynamicLibrary

## Installation

### 1ère étape : Téléchargement des sources

Les sources sont disponibles sur [GitHub](https://github.com/DragonJoker/Castor3D)

### 2ème étape : Compilation

Une fois les sources téléchargées, vous devez utiliser CMake (disponible [ici](http://www.cmake.org/cmake/resources/software.html)) pour générer le type de projet qui vous conviendra (Makefile, Code::Blocks, Visual Studio, ...)

Vous remarquerez que vous avez besoin de bibliothèques supplémentaires :
- FreeImage (optionnel)
- FreeType 2
- zlib

Une fois les dépendances téléchargées, compilées et installées, vous n'avez plus qu'à compiler CastorUtils

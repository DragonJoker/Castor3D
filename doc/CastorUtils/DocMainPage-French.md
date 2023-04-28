Bibliothèque CastorUtils	{#mainpage}
========================

## Introduction

Cette bibliothèque contient des classes générales aidant au développement du moteur Castor3D.

## Résumé

### Alignement

- alignedAlloc/alignedFree.
- AlignedMemoryAllocator/NonAlignedMemoryAllocator.

### Données

- Gestion de fichiers, binaires ou texte (BinaryFile et TextFile).
- FileParser, un analayseur de brace file, avec son FileParserContext et ses writers/parsers.
- Classes de chargement de données binaires ou texte (BinaryLoader et TextLoader).
- ZipArchive

### Concepts

- Implémentation d'ArrayView.
- Implémentation de BlockGuard.
- Concepts de ChangeTracked et GroupChangeTracked.
- Collection, un conteneur thread-safe.
- Factory.
- Named.
- NonCopyable.
- Unique.

### Graphisme

- RgbColour/RgbaColour et ColourComponent/HdrColourComponent.
- BoundingContainer, BoundingCube et BoundingSphere.
- Font, Glyph.
- Image ainsi que ses loaders/writers.
- PixelFormat, Pixel et PxBuffer, pour les conversions de pixels.
- Position, Size.

### Mathématiques

- Conversions d'angle.
- Conversions de longueurs.
- Point, une classe de point propriétaire de ses données.
- Coords, une classe de vue sur un point.
- Equations de droites (2D et 3D).
- Equations de plans.
- Matrix et SquareMatrix.
- Quaternion.
- SphericalVertex.
- Rectangle.
- Fonctions de math template.

### Divers

- BlockTracker et BlockTimer.
- Console et Logger.
- PreciseTimer.
- DynamicLibrary.
- Resource.

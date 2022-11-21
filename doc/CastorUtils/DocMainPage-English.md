CastorUtils Library	{#mainpage}
===================

## Introduction

This library contains some helper classes to ease the development of Castor3D engine.

## Summary

### Align

- alignedAlloc/alignedFree.
- AlignedMemoryAllocator/NonAlignedMemoryAllocator.

### Data

- File management base classes binary and text (BinaryFile and TextFile)
- FileParser, a brace file parser, along with its FileParserContext and base writers/parsers.
- Loader classes, binary and text (BinaryLoader and TextLoader)
- ZipArchive

### Design

- ArrayView implementation.
- BlockGuard implementation.
- ChangeTracked and GroupChangeTracked concepts.
- Collection, a thread-safe container.
- Factory.
- Named.
- NonCopyable.
- Unique.

### Graphics

- RgbColour/RgbaColour and ColourComponent/HdrColourComponent.
- BoundingContainer, BoundingCube and BoundingSphere.
- Font, Glyph.
- Image and loaders/writers.
- PixelFormat, Pixel and PxBuffer, to manage pixel conversions.
- Position, Size.

### Mathematics

- Angle conversions.
- Length conversions.
- Point, to have a point class which owns its data.
- Coords, a point view class.
- Line (2D and 3D) equations.
- Matrix and SquareMatrix.
- Plane equations.
- Quaternion.
- SphericalVertex.
- Rectangle.
- Template math functions.

### Miscellaneous

- BlockTracker and BlockTimer.
- Console and Logger.
- PreciseTimer.
- DynamicLibrary.
- Resource.

## Installation

### Step 1: Downloading the source

The project sources are available on [GitHub](https://github.com/DragonJoker/Castor3D).

### Step 2: Compilation

When sources are downloaded, you must use CMake (available [here](http://www.cmake.org/cmake/resources/software.html)) to generate the project file most suitable to your system (Makefile, Code::Blocks, Visual Studio, ...).

You'll need additional libraries :
- FreeImage (optional).
- FreeType 2.
- zlib.

Once you downloaded, compiled and installed the dependencies, you can compile CastorUtils.

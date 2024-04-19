# CastorUtils Library

## Align

- alignedAlloc/alignedFree.
- AlignedMemoryAllocator/NonAlignedMemoryAllocator.

## Data

- File management base classes binary and text (BinaryFile and TextFile)
- FileParser, a brace file parser, along with its FileParserContext and base writers/parsers.
- Loader classes, binary and text (BinaryLoader and TextLoader)
- ZipArchive

## Design

- ArrayView implementation.
- BlockGuard implementation.
- ChangeTracked and GroupChangeTracked concepts.
- Collection, a thread-safe container.
- Factory.
- Named.
- NonCopyable.
- Unique.

## Graphics

- RgbColour/RgbaColour and ColourComponent/HdrColourComponent.
- BoundingContainer, BoundingCube and BoundingSphere.
- Font, Glyph.
- Image and loaders/writers.
- PixelFormat, Pixel and PxBuffer, to manage pixel conversions.
- Position, Size.

## Mathematics

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

## Miscellaneous

- BlockTracker and BlockTimer.
- Console and Logger.
- PreciseTimer.
- DynamicLibrary.
- Resource.

/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_PRECOMPILED_HEADER_H___
#define ___C3D_PRECOMPILED_HEADER_H___

#include <config.hpp>

#if CASTOR_USE_PCH
#	include "Castor3DPrerequisites.hpp"

#	include <string>
#	include <vector>
#	include <map>
#	include <iostream>
#	include <algorithm>
#	include <list>
#	include <fstream>
#	include <algorithm>
#	include <stack>
#	include <typeinfo>
#	include <locale>
#	include <stdexcept>

#	include <cstdarg>
#	include <cmath>
#	include <ctime>
#	include <cstdint>

#	include <RequiredVersion.hpp>

#	include <GlslSource.hpp>

#	include <CastorUtils.hpp>
#	include <Config/MultiThreadConfig.hpp>
#	include <Data/BinaryFile.hpp>
#	include <Data/BinaryLoader.hpp>
#	include <Data/Path.hpp>
#	include <Data/TextFile.hpp>
#	include <Data/TextLoader.hpp>
#	include <Design/Collection.hpp>
#	include <Design/Factory.hpp>
#	include <Design/Named.hpp>
#	include <Design/NonCopyable.hpp>
#	include <Design/Resource.hpp>
#	include <Design/Templates.hpp>
#	include <Design/Unique.hpp>
#	include <Graphics/Colour.hpp>
#	include <Graphics/CubeBox.hpp>
#	include <Graphics/Font.hpp>
#	include <Graphics/Image.hpp>
#	include <Graphics/Pixel.hpp>
#	include <Graphics/Rectangle.hpp>
#	include <Graphics/SphereBox.hpp>
#	include <Log/Logger.hpp>
#	include <Math/Angle.hpp>
#	include <Math/Line2D.hpp>
#	include <Math/Line3D.hpp>
#	include <Math/Math.hpp>
#	include <Math/Matrix.hpp>
#	include <Math/PlaneEquation.hpp>
#	include <Math/Point.hpp>
#	include <Math/Quaternion.hpp>
#	include <Math/SphericalVertex.hpp>
#	include <Math/SquareMatrix.hpp>
#	include <Math/TransformationMatrix.hpp>
#	include <Miscellaneous/BlockTimer.hpp>
#	include <Miscellaneous/DynamicLibrary.hpp>
#	include <Miscellaneous/PreciseTimer.hpp>
#	include <Miscellaneous/StringUtils.hpp>
#	include <Miscellaneous/Utils.hpp>
#endif

#endif

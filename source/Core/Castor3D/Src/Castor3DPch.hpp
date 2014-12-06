/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
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

#	include <CastorUtils.hpp>
#	include <Angle.hpp>
#	include <Collection.hpp>
#	include <Colour.hpp>
#	include <CubeBox.hpp>
#	include <SphereBox.hpp>
#	include <DynamicLibrary.hpp>
#	include <Factory.hpp>
#	include <TextFile.hpp>
#	include <BinaryFile.hpp>
#	include <Font.hpp>
#	include <Image.hpp>
#	include <Line2D.hpp>
#	include <Line3D.hpp>
#	include <BinaryLoader.hpp>
#	include <TextLoader.hpp>
#	include <Logger.hpp>
#	include <Math.hpp>
#	include <Matrix.hpp>
#	include <SquareMatrix.hpp>
#	include <MultiThreadConfig.hpp>
#	include <Named.hpp>
#	include <NonCopyable.hpp>
#	include <Path.hpp>
#	include <Pixel.hpp>
#	include <PlaneEquation.hpp>
#	include <Point.hpp>
#	include <PreciseTimer.hpp>
#	include <Quaternion.hpp>
#	include <Resource.hpp>
#	include <SphericalVertex.hpp>
#	include <StringUtils.hpp>
#	include <Templates.hpp>
#	include <TransformationMatrix.hpp>
#	include <Utils.hpp>
#	include <Unique.hpp>

#	pragma warning( push )
#	pragma warning( disable:4311 )
#	pragma warning( disable:4312 )

#	include <Rectangle.hpp>

#	if defined( __linux__ )
#		include <GL/glx.h>
#	endif

#	pragma warning( pop )
#endif

#endif

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
#ifndef ___OBJ_IMPORTER_PREREQUISITES_H___
#define ___OBJ_IMPORTER_PREREQUISITES_H___

#include <Mesh/Importer.hpp>
#include <Mesh/FaceIndices.hpp>

#ifndef CASTOR_PLATFORM_WINDOWS
#	define C3D_Obj_API
#else
#	ifdef ObjImporter_EXPORTS
#		define C3D_Obj_API __declspec(dllexport)
#	else
#		define C3D_Obj_API __declspec(dllimport)
#	endif
#endif

namespace Obj
{
	struct stNORMAL;
	struct stVERTEX;
	struct stUV;
	struct stUVW;
	struct stGROUP;

	typedef std::vector< stVERTEX > VertexArray;
	typedef std::vector< stNORMAL > NormalArray;
	typedef std::vector< stUV > UvArray;
	typedef std::vector< stUVW > UvwArray;
	typedef std::vector< Castor3D::FaceIndices > FaceArray;
	typedef std::map< uint32_t, FaceArray > FaceArrayGrpMap;
	typedef std::map< uint32_t, uint32_t > UIntUIntMap;
	typedef std::vector< stGROUP * > GroupPtrArray;
	class ObjImporter;
}

#endif

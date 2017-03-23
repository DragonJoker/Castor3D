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
#ifndef ___OBJ_GROUP_H___
#define ___OBJ_GROUP_H___

#include "ObjImporterPrerequisites.hpp"

namespace Obj
{
	struct stNORMAL
	{
		Castor3D::real m_val[3];
	};
	struct stVERTEX
	{
		Castor3D::real m_val[3];
	};
	struct stUV
	{
		Castor3D::real m_val[2];
	};
	struct stUVW
	{
		Castor3D::real m_val[3];
	};
	struct stGROUP
	{
		Castor::String m_strName;
		Castor3D::MaterialSPtr m_pMaterial;
		UIntUIntMap m_mapVtxIndex;
		VertexArray m_arrayVtx;
		NormalArray m_arrayNml;
		UvArray m_arrayTex;
		VertexArray m_arraySubVtx;
		UvwArray m_arraySubTex;
		NormalArray m_arraySubNml;
		FaceArray m_arrayFaces;
		FaceArrayGrpMap m_mapIdx;
		uint32_t m_uiGroupId;
		bool m_bHasNormals;

		stGROUP()
			: m_uiGroupId( 0 )
			, m_bHasNormals( false )
		{
			m_mapIdx.insert( std::make_pair( 0, FaceArray() ) );
		}

		~stGROUP()
		{
			m_arrayVtx.clear();
			m_arrayNml.clear();
			m_arrayTex.clear();
			m_mapIdx.clear();
		}
	};
}

#endif

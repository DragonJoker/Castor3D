/*
See LICENSE file in root folder
*/
#ifndef ___OBJ_GROUP_H___
#define ___OBJ_GROUP_H___

#include "ObjImporterPrerequisites.hpp"

namespace Obj
{
	struct stNORMAL
	{
		float m_val[3];
	};
	struct stVERTEX
	{
		float m_val[3];
	};
	struct stUV
	{
		float m_val[2];
	};
	struct stUVW
	{
		float m_val[3];
	};
	struct stGROUP
	{
		castor::String m_strName;
		castor3d::MaterialSPtr m_pMaterial;
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

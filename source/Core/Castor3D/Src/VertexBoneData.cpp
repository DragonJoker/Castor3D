#include "VertexBoneData.hpp"

using namespace Castor;

namespace Castor3D
{
	stVERTEX_BONE_DATA::stVERTEX_BONE_DATA()
	{
		for ( int i = 0; i < C3D_MAX_BONES_PER_VERTEX; ++i )
		{
			m_ids[i] = 0;
			m_weights[i] = 0;
		}
	}

	void stVERTEX_BONE_DATA::AddBoneData( uint32_t p_uiBoneId, real p_rWeight )
	{
		bool l_bDone = false;

		for ( int i = 0; i < C3D_MAX_BONES_PER_VERTEX && !l_bDone; i++ )
		{
			if ( m_weights[i] == 0.0 )
			{
				m_ids[i] = p_uiBoneId;
				m_weights[i] = p_rWeight;
				l_bDone = true;
			}
		}

		CASTOR_ASSERT( l_bDone, "VertexBoneData - No more slot available" );
	}
}

#include "VertexBoneData.hpp"

using namespace Castor;

namespace Castor3D
{
	stVERTEX_BONE_DATA::stVERTEX_BONE_DATA()
		: m_ids {}
		, m_weights {}
	{
	}

	void stVERTEX_BONE_DATA::AddBoneData( uint32_t p_boneId, real p_weight )
	{
		bool l_done = false;

		for ( int i = 0; i < C3D_MAX_BONES_PER_VERTEX && !l_done; i++ )
		{
			if ( m_weights[i] == 0.0_r )
			{
				m_ids[i] = p_boneId;
				m_weights[i] = p_weight;
				l_done = true;
			}
		}

		//ENSURE( l_done );
	}
}

#include "VertexBoneData.hpp"

using namespace castor;

namespace castor3d
{
	VertexBoneData::VertexBoneData()
		: m_ids{}
		, m_weights{}
	{
	}

	void VertexBoneData::addBoneData( uint32_t p_boneId, real p_weight )
	{
		bool done = false;

		for ( int i = 0; i < C3D_MAX_BONES_PER_VERTEX && !done; i++ )
		{
			if ( m_weights[i] == 0.0_r )
			{
				m_ids[i] = p_boneId;
				m_weights[i] = p_weight;
				done = true;
			}
		}

		//ENSURE( done );
	}
}

#include "Castor3D/Model/Skeleton/VertexBoneData.hpp"

namespace castor3d
{
	void VertexBoneData::addBoneData( uint32_t boneId, float weight )
	{
		bool done = false;

		for ( uint32_t i = 0; i < MaxBonesPerVertex && !done; i++ )
		{
			if ( m_weights.data[i] == 0.0f )
			{
				m_ids.data[i] = boneId;
				m_weights.data[i] = weight;
				done = true;
			}
		}
	}
}

#include "Castor3D/Model/Skeleton/VertexBoneData.hpp"

using namespace castor;

namespace castor3d
{
	VertexBoneData::VertexBoneData()
	{
	}

	VertexBoneData::VertexBoneData( VertexBoneData const & rhs )
		: m_ids{ rhs.m_ids }
		, m_weights{ rhs.m_weights }
	{
	}

	VertexBoneData::VertexBoneData( VertexBoneData && rhs )noexcept
		: m_ids{ std::move( rhs.m_ids ) }
		, m_weights{ std::move( rhs.m_weights ) }
	{
	}

	VertexBoneData::~VertexBoneData()
	{
	}

	VertexBoneData & VertexBoneData::operator=( VertexBoneData const & rhs )
	{
		m_ids = rhs.m_ids;
		m_weights = rhs.m_weights;
		return *this;
	}

	VertexBoneData & VertexBoneData::operator=( VertexBoneData && rhs )noexcept
	{
		if ( this != &rhs )
		{
			m_ids = std::move( rhs.m_ids );
			m_weights = std::move( rhs.m_weights );
		}

		return *this;
	}

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

		//CU_Ensure( done );
	}
}

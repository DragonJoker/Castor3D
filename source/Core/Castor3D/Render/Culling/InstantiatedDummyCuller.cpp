#include "Castor3D/Render/Culling/InstantiatedDummyCuller.hpp"

namespace castor3d
{
	InstantiatedDummyCuller::InstantiatedDummyCuller( Scene & scene
		, uint32_t instanceCount )
		: SceneCuller{ scene, nullptr, instanceCount }
	{
	}

	void InstantiatedDummyCuller::doCullGeometries()
	{
		for ( size_t i = 0; i < size_t( RenderMode::eCount ); ++i )
		{
			m_allSubmeshes[i].copy( m_culledSubmeshes[i] );
		}
	}

	void InstantiatedDummyCuller::doCullBillboards()
	{
		for ( size_t i = 0; i < size_t( RenderMode::eCount ); ++i )
		{
			m_allBillboards[i].copy( m_culledBillboards[i] );
		}
	}
}

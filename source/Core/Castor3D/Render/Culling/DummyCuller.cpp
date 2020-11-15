#include "Castor3D/Render/Culling/DummyCuller.hpp"

namespace castor3d
{
	DummyCuller::DummyCuller( Scene & scene
		, Camera * camera )
		: SceneCuller{ scene, camera, 1u }
	{
	}

	void DummyCuller::doCullGeometries()
	{
		for ( size_t i = 0; i < size_t( RenderMode::eCount ); ++i )
		{
			m_allSubmeshes[i].copy( m_culledSubmeshes[i] );
		}
	}

	void DummyCuller::doCullBillboards()
	{
		for ( size_t i = 0; i < size_t( RenderMode::eCount ); ++i )
		{
			m_allBillboards[i].copy( m_culledBillboards[i] );
		}
	}
}

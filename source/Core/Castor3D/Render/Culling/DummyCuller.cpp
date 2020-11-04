#include "Castor3D/Render/Culling/DummyCuller.hpp"

namespace castor3d
{
	DummyCuller::DummyCuller( Scene & scene )
		: SceneCuller{ scene, nullptr, 1u }
	{
	}

	void DummyCuller::doCullGeometries()
	{
		m_allOpaqueSubmeshes.copy( m_culledOpaqueSubmeshes );
		m_allTransparentSubmeshes.copy( m_culledTransparentSubmeshes );
	}

	void DummyCuller::doCullBillboards()
	{
		m_allOpaqueBillboards.copy( m_culledOpaqueBillboards );
		m_allTransparentBillboards.copy( m_culledTransparentBillboards );
	}
}

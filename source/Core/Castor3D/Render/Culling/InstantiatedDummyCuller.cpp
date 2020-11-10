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
		m_allOpaqueSubmeshes.copy( m_culledOpaqueSubmeshes );
		m_allTransparentSubmeshes.copy( m_culledTransparentSubmeshes );
	}

	void InstantiatedDummyCuller::doCullBillboards()
	{
		m_allOpaqueBillboards.copy( m_culledOpaqueBillboards );
		m_allTransparentBillboards.copy( m_culledTransparentBillboards );
	}
}

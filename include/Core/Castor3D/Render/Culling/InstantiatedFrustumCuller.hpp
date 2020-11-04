/*
See LICENSE file in root folder
*/
#ifndef ___C3D_InstantiatedFrustumCuller_H___
#define ___C3D_InstantiatedFrustumCuller_H___

#include "Castor3D/Render/Culling/SceneCuller.hpp"

namespace castor3d
{
	class InstantiatedFrustumCuller
		: public SceneCuller
	{
	public:
		C3D_API InstantiatedFrustumCuller( Scene & scene
			, Camera & camera
			, uint32_t instancesCount );

		void setFrustums( std::vector< Frustum > frustums )
		{
			m_frustums = std::move( frustums );
		}

	private:
		void doCullGeometries()override;
		void doCullBillboards()override;

	private:
		std::vector< Frustum > m_frustums;
	};
}

#endif

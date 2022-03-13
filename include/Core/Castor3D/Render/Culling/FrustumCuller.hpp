/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FrustumCuller_H___
#define ___C3D_FrustumCuller_H___

#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Frustum.hpp"

namespace castor3d
{
	class FrustumCuller
		: public SceneCuller
	{
	public:
		C3D_API FrustumCuller( Scene & scene
			, Camera & camera );
		C3D_API explicit FrustumCuller( Camera & camera );
		C3D_API FrustumCuller( Scene & scene
			, Frustum & frustum );

		C3D_API void updateFrustum( castor::Matrix4x4f const & projection
			, castor::Matrix4x4f const & view );

	private:
		void doCullGeometries()override;
		void doCullBillboards()override;

		Frustum * m_frustum{};
	};
}

#endif

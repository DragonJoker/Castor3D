/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FrustumCuller_H___
#define ___C3D_FrustumCuller_H___

#include "Castor3D/Render/Culling/SceneCuller.hpp"

namespace castor3d
{
	class FrustumCuller
		: public SceneCuller
	{
	public:
		C3D_API FrustumCuller( Scene & scene
			, Camera & camera );

	private:
		void doCullGeometries()override;
		void doCullBillboards()override;
	};
}

#endif

/*
See LICENSE file in root folder
*/
#ifndef ___C3D_InstantiatedDummyCuller_H___
#define ___C3D_InstantiatedDummyCuller_H___

#include "Castor3D/Render/Culling/SceneCuller.hpp"

namespace castor3d
{
	class InstantiatedDummyCuller
		: public SceneCuller
	{
	public:
		C3D_API explicit InstantiatedDummyCuller( Scene & scene
			, uint32_t instanceCount );

	private:
		void doCullGeometries()override;
		void doCullBillboards()override;
	};
}

#endif

/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FrustumCuller_H___
#define ___C3D_FrustumCuller_H___

#include "Render/Culling/SceneCuller.hpp"

namespace castor3d
{
	/**
	*\~english
	*\brief
	*	Culls nodes against a frustum.
	*\~french
	*\brief
	*	Elimine les noeuds par rapport à un frustum.
	*/
	class FrustumCuller
		: public SceneCuller
	{
	public:
		C3D_API FrustumCuller( Scene const & scene
			, Camera & camera );

	private:
		void doCullGeometries()override;
		void doCullBillboards()override;
	};
}

#endif

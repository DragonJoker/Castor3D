/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DummyCuller_H___
#define ___C3D_DummyCuller_H___

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
	class DummyCuller
		: public SceneCuller
	{
	public:
		C3D_API explicit DummyCuller( Scene const & scene );

	private:
		void doCullGeometries()override;
		void doCullBillboards()override;
	};
}

#endif

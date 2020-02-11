/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayVisitor_H___
#define ___C3D_OverlayVisitor_H___

#include "OverlayModule.hpp"

namespace castor3d
{
	class OverlayVisitor
	{
	public:
		C3D_API virtual void visit( PanelOverlay const & overlay ) = 0;
		C3D_API virtual void visit( BorderPanelOverlay const & overlay ) = 0;
		C3D_API virtual void visit( TextOverlay const & overlay ) = 0;
	};
}

#endif

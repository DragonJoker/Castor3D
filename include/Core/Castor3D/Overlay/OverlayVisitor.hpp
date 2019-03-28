/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayVisitor_H___
#define ___C3D_OverlayVisitor_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\~english
	\brief		The overlay visitor base class.
	\~french
	\brief		La classe de base de visiteur d'incrustation.
	*/
	class OverlayVisitor
	{
	public:
		C3D_API virtual void visit( PanelOverlay const & overlay ) = 0;
		C3D_API virtual void visit( BorderPanelOverlay const & overlay ) = 0;
		C3D_API virtual void visit( TextOverlay const & overlay ) = 0;
	};
}

#endif

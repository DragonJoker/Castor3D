/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayFactory_H___
#define ___C3D_OverlayFactory_H___

#include "Castor3D/Overlay/OverlayCategory.hpp"

#include <CastorUtils/Design/Factory.hpp>

namespace c3d
{
	class OverlayFactory
		: public Factory< OverlayCategory
			, OverlayType
			, OverlayCategoryUPtr >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API OverlayFactory();
	};
}

#endif

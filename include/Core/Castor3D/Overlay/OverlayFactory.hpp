/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayFactory_H___
#define ___C3D_OverlayFactory_H___

#include "Castor3D/Overlay/OverlayCategory.hpp"

#include <CastorUtils/Design/Factory.hpp>

namespace castor3d
{
	class OverlayFactory
		: public castor::Factory< OverlayCategory, OverlayType >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API OverlayFactory();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~OverlayFactory();
	};
}

#endif

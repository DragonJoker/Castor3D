/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OVERLAY_FACTORY_H___
#define ___C3D_OVERLAY_FACTORY_H___

#include "OverlayCategory.hpp"

#include <Design/Factory.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date		24/11/2014
	\~english
	\brief		The mesh factory
	\~french
	\brief		La fabrique de maillages
	*/
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

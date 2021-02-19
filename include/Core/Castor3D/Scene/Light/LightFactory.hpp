/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LIGHT_FACTORY_H___
#define ___C3D_LIGHT_FACTORY_H___

#include "Castor3D/Scene/Light/Light.hpp"

#include <CastorUtils/Design/Factory.hpp>

namespace castor3d
{
	class LightFactory
		: public castor::Factory< LightCategory, LightType, LightCategoryUPtr, std::function< LightCategoryUPtr( Light & ) > >
	{
	public:
		/**
		*\~english
		*\brief
		*	Registers default light types.
		*\~french
		*\brief
		*	Enregistre les types par défaut.
		*/
		C3D_API LightFactory();
	};
}

#endif

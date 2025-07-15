/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LIGHT_FACTORY_H___
#define ___C3D_LIGHT_FACTORY_H___

#include "Castor3D/Scene/Light/Light.hpp"

#include <CastorUtils/Design/Factory.hpp>

namespace c3d
{
	class LightFactory
		: public Factory< LightCategory, LightType, LightCategoryUPtr, Function< LightCategoryUPtr( bool &, Function< void() > const & ) > >
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

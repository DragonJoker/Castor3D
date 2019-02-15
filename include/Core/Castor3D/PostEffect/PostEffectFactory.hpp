/*
See LICENSE file in root folder
*/
#ifndef ___C3D_POST_EFFECT_FACTORY_H___
#define ___C3D_POST_EFFECT_FACTORY_H___

#include "Castor3D/PostEffect/PostEffect.hpp"

#include <CastorUtils/Design/Factory.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		03/07/2016
	\~english
	\brief		Post effect factory.
	\~french
	\brief		La fabrique d'eefets post rendu.
	*/
	using PostEffectFactory = castor::Factory< PostEffect, castor::String, std::shared_ptr< PostEffect >, std::function< PostEffectSPtr( RenderTarget &, RenderSystem &, Parameters const & ) > >;
}

#endif

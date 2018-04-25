/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RENDER_SYSTEM_FACTORY_H___
#define ___C3D_RENDER_SYSTEM_FACTORY_H___

#include "RenderSystem.hpp"

#include <Design/Factory.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date		24/11/2014
	\~english
	\brief		Render technique factory
	\~french
	\brief		La fabrique de techniques de rendu
	*/
	using RenderSystemFactory = castor::Factory< RenderSystem
		, castor::String
		, RenderSystemUPtr
		, std::function< RenderSystemUPtr( Engine &, castor::String const & ) > >;
}

#endif

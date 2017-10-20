/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TONE_MAPPING_FACTORY_H___
#define ___C3D_TONE_MAPPING_FACTORY_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/Factory.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		11/03/2016
	\~english
	\brief		Tone mapping factory.
	\~french
	\brief		Fabrique de mappage de tons.
	*/
	using ToneMappingFactory = castor::Factory< ToneMapping, castor::String, ToneMappingSPtr, std::function< ToneMappingSPtr( Engine &, Parameters const & ) > >;
}

#endif

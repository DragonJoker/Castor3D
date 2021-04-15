/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextSpotlight_H___
#define ___C3D_TextSpotlight_H___

#include "Castor3D/Scene/Light/SpotLight.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::SpotLight >
		: public TextWriterT< castor3d::SpotLight >
	{
	public:
		C3D_API explicit TextWriter( castor::String const & tabs );
		C3D_API bool operator()( castor3d::SpotLight const & light
			, castor::StringStream & file );
	};
}

#endif

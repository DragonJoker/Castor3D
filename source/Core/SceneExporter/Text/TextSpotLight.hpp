/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextSpotlight_H___
#define ___CSE_TextSpotlight_H___

#include <Castor3D/Scene/Light/SpotLight.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::SpotLight >
		: public TextWriterT< castor3d::SpotLight >
	{
	public:
		explicit TextWriter( castor::String const & tabs );
		bool operator()( castor3d::SpotLight const & light
			, castor::StringStream & file )override;
	};
}

#endif

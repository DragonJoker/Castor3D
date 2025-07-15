/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextSpotlight_H___
#define ___CSE_TextSpotlight_H___

#include <Castor3D/Scene/Light/SpotLight.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< SpotLight >
		: public TextWriterT< SpotLight >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( SpotLight const & light
			, StringStream & file )override;
	};
}

#endif

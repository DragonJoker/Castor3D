/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextLightGroup_H___
#define ___CSE_TextLightGroup_H___

#include <Castor3D/Scene/Light/LightGroup.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::LightGroup >
		: public TextWriterT< castor3d::LightGroup >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::LightGroup const & lightGroup
			, castor::StringStream & file )override;
	};
}

#endif

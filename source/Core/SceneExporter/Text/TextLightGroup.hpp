/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextLightGroup_H___
#define ___CSE_TextLightGroup_H___

#include <Castor3D/Scene/Light/LightGroup.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< LightGroup >
		: public TextWriterT< LightGroup >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( LightGroup const & lightGroup
			, StringStream & file )override;
	};
}

#endif

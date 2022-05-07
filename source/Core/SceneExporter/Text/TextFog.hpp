/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextFog_H___
#define ___CSE_TextFog_H___

#include <Castor3D/Scene/Fog.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Fog >
		: public TextWriterT< castor3d::Fog >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::Fog const & config
			, castor::StringStream & file )override;
	};
}

#endif

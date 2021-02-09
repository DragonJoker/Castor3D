/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextFog_H___
#define ___C3D_TextFog_H___

#include "Castor3D/Scene/Fog.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Fog >
		: public TextWriterT< castor3d::Fog >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs );
		C3D_API bool operator()( castor3d::Fog const & config
			, TextFile & file )override;
	};
}

#endif

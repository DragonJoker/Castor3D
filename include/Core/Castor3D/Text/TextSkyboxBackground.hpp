/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextSkyboxBackground_H___
#define ___C3D_TextSkyboxBackground_H___

#include "Castor3D/Scene/Background/Skybox.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::SkyboxBackground >
		: public TextWriterT< castor3d::SkyboxBackground >
	{
	public:
		C3D_API explicit TextWriter( castor::String const & tabs );
		C3D_API bool operator()( castor3d::SkyboxBackground const & overlay
			, castor::TextFile & file );
	};
}

#endif

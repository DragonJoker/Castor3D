/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextRsmConfig_H___
#define ___C3D_TextRsmConfig_H___

#include "Castor3D/Render/Technique/Opaque/ReflectiveShadowMapGI/RsmConfig.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::RsmConfig >
		: public TextWriterT< castor3d::RsmConfig >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs );
		C3D_API bool operator()( castor3d::RsmConfig const & config
			, castor::StringStream & file )override;
	};
}

#endif

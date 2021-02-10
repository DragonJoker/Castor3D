/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextHdrConfig_H___
#define ___C3D_TextHdrConfig_H___

#include "Castor3D/Render/ToneMapping/HdrConfig.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::HdrConfig >
		: public TextWriterT< castor3d::HdrConfig >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs );
		C3D_API bool operator()( castor3d::HdrConfig const & obj
			, TextFile & file )override;
	};
}

#endif

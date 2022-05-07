/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextHdrConfig_H___
#define ___CSE_TextHdrConfig_H___

#include <Castor3D/Render/ToneMapping/HdrConfig.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::HdrConfig >
		: public TextWriterT< castor3d::HdrConfig >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::HdrConfig const & obj
			, castor::StringStream & file )override;
	};
}

#endif

/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextHdrConfig_H___
#define ___CSE_TextHdrConfig_H___

#include <Castor3D/Render/ToneMapping/HdrConfig.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< HdrConfig >
		: public TextWriterT< HdrConfig >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( HdrConfig const & obj
			, StringStream & file )override;
	};
}

#endif

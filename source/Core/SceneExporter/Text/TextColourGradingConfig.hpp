/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextColourGradingConfig_H___
#define ___CSE_TextColourGradingConfig_H___

#include <Castor3D/Render/ToneMapping/ColourGradingConfig.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::ColourGradingConfig >
		: public TextWriterT< castor3d::ColourGradingConfig >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::ColourGradingConfig const & config
			, StringStream & file )override;
	};
}

#endif

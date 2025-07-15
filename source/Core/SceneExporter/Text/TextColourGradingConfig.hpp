/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextColourGradingConfig_H___
#define ___CSE_TextColourGradingConfig_H___

#include <Castor3D/Render/ToneMapping/ColourGradingConfig.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< ColourGradingConfig >
		: public TextWriterT< ColourGradingConfig >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( ColourGradingConfig const & config
			, StringStream & file )override;
	};
}

#endif

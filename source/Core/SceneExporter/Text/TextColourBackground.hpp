/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextColourBackground_H___
#define ___CSE_TextColourBackground_H___

#include <Castor3D/Scene/Background/Colour.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::ColourBackground >
		: public TextWriterT< castor3d::ColourBackground >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::ColourBackground const & overlay
			, castor::StringStream & file )override;
	};
}

#endif

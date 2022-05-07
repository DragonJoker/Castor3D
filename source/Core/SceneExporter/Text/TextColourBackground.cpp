#include "TextColourBackground.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

#include <CastorUtils/Data/Text/TextRgbColour.hpp>

namespace castor
{
	using namespace castor3d;

	TextWriter< ColourBackground >::TextWriter( String const & tabs )
		: TextWriterT< ColourBackground >{ tabs }
	{
	}

	bool TextWriter< ColourBackground >::operator()( ColourBackground const & background
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing ColourBackground" ) << std::endl;
		return writeNamedSub( file, "background_colour", background.getColour() );
	}
}

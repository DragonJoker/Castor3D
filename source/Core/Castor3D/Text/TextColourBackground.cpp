#include "Castor3D/Text/TextColourBackground.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

#include <CastorUtils/Data/Text/TextRgbColour.hpp>

using namespace castor3d;

namespace castor
{
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

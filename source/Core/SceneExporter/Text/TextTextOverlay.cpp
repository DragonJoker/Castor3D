#include "TextTextOverlay.hpp"

#include "TextOverlayCategory.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Overlay/Overlay.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>

namespace castor
{
	using namespace castor3d;

	TextWriter< TextOverlay >::TextWriter( castor::String const & tabs )
		: TextWriterT< TextOverlay >{ tabs }
	{
	}

	bool TextWriter< TextOverlay >::operator()( TextOverlay const & overlay
		, castor::StringStream & file )
	{
		log::info << tabs() << cuT( "Writing TextOverlay " ) << overlay.getOverlayName() << std::endl;
		bool result = false;

		if ( auto block{ beginBlock( file, cuT( "text_overlay" ), overlay.getOverlayName() ) } )
		{
			result = writeName( file, cuT( "font" ), overlay.getFontTexture()->getFontName() )
				&& writeName( file, cuT( "text" ), string::stringCast< xchar >( overlay.getCaption() ) )
				&& write( file, cuT( "text_wrapping" ), getName( overlay.getTextWrappingMode() ) )
				&& writeOpt( file, cuT( "horizontal_align" ), getName( overlay.getHAlign() ), getName( HAlign::eLeft ) )
				&& writeOpt( file, cuT( "vertical_align" ), getName( overlay.getVAlign() ), getName( VAlign::eTop ) )
				&& write( file, cuT( "texturing_mode" ), getName( overlay.getTexturingMode() ) )
				&& write( file, cuT( "line_spacing_mode" ), getName( overlay.getLineSpacingMode() ) )
				&& writeSub< OverlayCategory >( file, overlay );
		}

		return result;
	}
}

#include "Castor3D/Text/TextTextOverlay.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Text/TextOverlayCategory.hpp"

#include <CastorUtils/Data/Text/TextPoint.hpp>

using namespace castor3d;

namespace castor
{
	TextWriter< TextOverlay >::TextWriter( castor::String const & tabs )
		: TextWriterT< TextOverlay >{ tabs }
	{
	}

	bool TextWriter< TextOverlay >::operator()( TextOverlay const & overlay
		, castor::TextFile & file )
	{
		log::info << tabs() << cuT( "Writing TextOverlay " ) << overlay.getOverlayName() << std::endl;
		bool result = false;

		if ( auto block = beginBlock( cuT( "text_overlay" ), overlay.getOverlayName(), file ) )
		{
			result = writeName( cuT( "font" ), overlay.getFontTexture()->getFontName(), file )
				&& writeName( cuT( "text" ), overlay.getCaption(), file )
				&& write( cuT( "text_wrapping" ), castor3d::getName( overlay.getTextWrappingMode() ), file )
				&& write( cuT( "vertical_align" ), castor3d::getName( overlay.getVAlign() ), file )
				&& write( cuT( "horizontal_align" ), castor3d::getName( overlay.getHAlign() ), file )
				&& write( cuT( "texturing_mode" ), castor3d::getName( overlay.getTexturingMode() ), file )
				&& write( cuT( "line_spacing_mode" ), castor3d::getName( overlay.getLineSpacingMode() ), file )
				&& write< OverlayCategory >( overlay, file );
		}

		return result;
	}
}

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

		if ( auto block{ beginBlock( file, cuT( "text_overlay" ), overlay.getOverlayName() ) } )
		{
			result = writeName( file, cuT( "font" ), overlay.getFontTexture()->getFontName() )
				&& writeName( file, cuT( "text" ), overlay.getCaption() )
				&& write( file, cuT( "text_wrapping" ), castor3d::getName( overlay.getTextWrappingMode() ) )
				&& write( file, cuT( "vertical_align" ), castor3d::getName( overlay.getVAlign() ) )
				&& write( file, cuT( "horizontal_align" ), castor3d::getName( overlay.getHAlign() ) )
				&& write( file, cuT( "texturing_mode" ), castor3d::getName( overlay.getTexturingMode() ) )
				&& write( file, cuT( "line_spacing_mode" ), castor3d::getName( overlay.getLineSpacingMode() ) )
				&& write< OverlayCategory >( file, overlay );
		}

		return result;
	}
}

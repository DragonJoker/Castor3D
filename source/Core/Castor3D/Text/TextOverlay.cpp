#include "Castor3D/Text/TextOverlay.hpp"

#include "Castor3D/Text/TextBorderPanelOverlay.hpp"
#include "Castor3D/Text/TextPanelOverlay.hpp"
#include "Castor3D/Text/TextTextOverlay.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< Overlay >::TextWriter( String const & tabs )
		: TextWriterT< Overlay >{ tabs }
	{
	}

	bool TextWriter< Overlay >::operator()( Overlay const & overlay
		, TextFile & file )
	{
		switch ( overlay.getType() )
		{
		case OverlayType::ePanel:
			return write( file, *overlay.getPanelOverlay() );
		case OverlayType::eBorderPanel:
			return write( file, *overlay.getBorderPanelOverlay() );
		case OverlayType::eText:
			return write( file, *overlay.getTextOverlay() );
		default:
			return false;
		}
	}
}

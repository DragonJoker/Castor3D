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
			return write( *overlay.getPanelOverlay(), file );
		case OverlayType::eBorderPanel:
			return write( *overlay.getBorderPanelOverlay(), file );
		case OverlayType::eText:
			return write( *overlay.getTextOverlay(), file );
		default:
			return false;
		}
	}
}

#include "TextOverlay.hpp"

#include "TextBorderPanelOverlay.hpp"
#include "TextPanelOverlay.hpp"
#include "TextTextOverlay.hpp"

namespace castor
{
	using namespace castor3d;

	TextWriter< Overlay >::TextWriter( String const & tabs )
		: TextWriterT< Overlay >{ tabs }
	{
	}

	bool TextWriter< Overlay >::operator()( Overlay const & overlay
		, StringStream & file )
	{
		switch ( overlay.getType() )
		{
		case OverlayType::ePanel:
			return writeSub( file, *overlay.getPanelOverlay() );
		case OverlayType::eBorderPanel:
			return writeSub( file, *overlay.getBorderPanelOverlay() );
		case OverlayType::eText:
			return writeSub( file, *overlay.getTextOverlay() );
		default:
			return false;
		}
	}
}

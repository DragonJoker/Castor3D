#include "Castor3D/Text/TextPanelOverlay.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Text/TextOverlayCategory.hpp"

#include <CastorUtils/Data/Text/TextPoint.hpp>


using namespace castor3d;

namespace castor
{
	TextWriter< PanelOverlay >::TextWriter( String const & tabs )
		: TextWriterT< PanelOverlay >{ tabs }
	{
	}

	bool TextWriter< PanelOverlay >::operator()( PanelOverlay const & overlay, TextFile & file )
	{
		log::info << tabs() << cuT( "Writing PanelOverlay " ) << overlay.getOverlayName() << std::endl;
		bool result = false;

		if ( auto block = beginBlock( cuT( "panel_overlay" ), overlay.getOverlay().getName(), file ) )
		{
			result = write< OverlayCategory >( overlay, file );
		}

		return result;
	}
}

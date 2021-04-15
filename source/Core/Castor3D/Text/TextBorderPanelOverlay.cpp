#include "Castor3D/Text/TextBorderPanelOverlay.hpp"

#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Text/TextOverlayCategory.hpp"

#include <CastorUtils/Data/Text/TextPoint.hpp>

using namespace castor3d;

namespace castor
{
	TextWriter< BorderPanelOverlay >::TextWriter( String const & tabs )
		: TextWriterT< BorderPanelOverlay >{ tabs }
	{
	}

	bool TextWriter< BorderPanelOverlay >::operator()( BorderPanelOverlay const & overlay
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing BorderPanelOverlay " ) << overlay.getOverlayName() << std::endl;
		bool result = false;

		if ( auto block{ beginBlock( file, cuT( "border_panel_overlay" ), overlay.getOverlay().getName() ) } )
		{
			result = writeNamedSub( file, cuT( "border_size" ), overlay.getBorderSize() );

			if ( result && overlay.getBorderMaterial() )
			{
				result = writeName( file, cuT( "border_material" ), overlay.getBorderMaterial()->getName() );
			}

			if ( result )
			{
				result = writeSub< OverlayCategory >( file, overlay );
			}
		}

		return result;
	}
}

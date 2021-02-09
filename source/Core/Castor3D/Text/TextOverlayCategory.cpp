#include "Castor3D/Text/TextOverlayCategory.hpp"

#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Text/TextBorderPanelOverlay.hpp"
#include "Castor3D/Text/TextPanelOverlay.hpp"
#include "Castor3D/Text/TextTextOverlay.hpp"

#include <CastorUtils/Data/Text/TextPoint.hpp>
#include <CastorUtils/Data/Text/TextPosition.hpp>
#include <CastorUtils/Data/Text/TextSize.hpp>

using namespace castor3d;

namespace castor
{
	TextWriter< OverlayCategory >::TextWriter( String const & tabs )
		: TextWriterT< OverlayCategory >{ tabs }
	{
	}

	bool TextWriter< OverlayCategory >::operator()( OverlayCategory const & overlay
		, TextFile & file )
	{
		bool result = write( cuT( "position" ), overlay.getPosition(), file )
			&& write( cuT( "size" ), overlay.getSize(), file );

		if ( result && overlay.getMaterial() )
		{
			result = writeName( cuT( "material" ), overlay.getMaterial()->getName(), file );
		}

		for ( auto overlay : overlay.getOverlay() )
		{
			switch ( overlay->getType() )
			{
			case OverlayType::ePanel:
				result = result && write( *overlay->getPanelOverlay(), file );
				break;

			case OverlayType::eBorderPanel:
				result = result && write( *overlay->getBorderPanelOverlay(), file );
				break;

			case OverlayType::eText:
				result = result && write( *overlay->getTextOverlay(), file );
				break;

			default:
				result = false;
			}
		}

		return result;
	}
}

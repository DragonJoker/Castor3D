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
		bool result = writeNamedSub( file, cuT( "position" ), overlay.getPosition() )
			&& writeNamedSub( file, cuT( "size" ), overlay.getSize() );

		if ( result && overlay.getMaterial() )
		{
			result = writeName( file, cuT( "material" ), overlay.getMaterial()->getName() );
		}

		for ( auto overlay : overlay.getOverlay() )
		{
			switch ( overlay->getType() )
			{
			case OverlayType::ePanel:
				result = result && writeSub( file, *overlay->getPanelOverlay() );
				break;

			case OverlayType::eBorderPanel:
				result = result && writeSub( file, *overlay->getBorderPanelOverlay() );
				break;

			case OverlayType::eText:
				result = result && writeSub( file, *overlay->getTextOverlay() );
				break;

			default:
				result = false;
			}
		}

		return result;
	}
}

#include "TextOverlayCategory.hpp"

#include "TextBorderPanelOverlay.hpp"
#include "TextPanelOverlay.hpp"
#include "TextTextOverlay.hpp"

#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Overlay/Overlay.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>
#include <CastorUtils/Data/Text/TextPosition.hpp>
#include <CastorUtils/Data/Text/TextSize.hpp>

namespace castor
{
	using namespace castor3d;

	TextWriter< OverlayCategory >::TextWriter( String const & tabs )
		: TextWriterT< OverlayCategory >{ tabs }
	{
	}

	bool TextWriter< OverlayCategory >::operator()( OverlayCategory const & overlay
		, StringStream & file )
	{
		bool result = writeNamedSub( file, cuT( "position" ), overlay.getRelativePosition() )
			&& writeNamedSub( file, cuT( "size" ), overlay.getRelativeSize() );

		if ( result && overlay.getMaterial() )
		{
			result = writeName( file, cuT( "material" ), overlay.getMaterial()->getName() );
		}

		for ( auto child : overlay.getOverlay() )
		{
			switch ( child->getType() )
			{
			case OverlayType::ePanel:
				result = result && writeSub( file, *child->getPanelOverlay() );
				break;

			case OverlayType::eBorderPanel:
				result = result && writeSub( file, *child->getBorderPanelOverlay() );
				break;

			case OverlayType::eText:
				result = result && writeSub( file, *child->getTextOverlay() );
				break;

			default:
				result = false;
			}
		}

		return result;
	}
}

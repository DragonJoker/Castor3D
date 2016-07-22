#include "OverlayFactory.hpp"

#include "PanelOverlay.hpp"
#include "BorderPanelOverlay.hpp"
#include "TextOverlay.hpp"

using namespace Castor;

namespace Castor3D
{
	OverlayFactory::OverlayFactory()
		: Factory< OverlayCategory, eOVERLAY_TYPE >()
	{
		Register( eOVERLAY_TYPE_PANEL, &PanelOverlay::Create );
		Register( eOVERLAY_TYPE_BORDER_PANEL, &BorderPanelOverlay::Create );
		Register( eOVERLAY_TYPE_TEXT, &TextOverlay::Create );
	}

	OverlayFactory::~OverlayFactory()
	{
	}
}

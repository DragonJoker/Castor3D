#include "OverlayFactory.hpp"

#include "PanelOverlay.hpp"
#include "BorderPanelOverlay.hpp"
#include "TextOverlay.hpp"

using namespace Castor;

namespace Castor3D
{
	OverlayFactory::OverlayFactory()
		: Factory< OverlayCategory, OverlayType >()
	{
		Register( OverlayType::ePanel, &PanelOverlay::Create );
		Register( OverlayType::eBorderPanel, &BorderPanelOverlay::Create );
		Register( OverlayType::eText, &TextOverlay::Create );
	}

	OverlayFactory::~OverlayFactory()
	{
	}
}

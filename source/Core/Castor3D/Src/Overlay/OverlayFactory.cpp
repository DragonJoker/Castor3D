#include "OverlayFactory.hpp"

#include "PanelOverlay.hpp"
#include "BorderPanelOverlay.hpp"
#include "TextOverlay.hpp"

using namespace castor;

namespace castor3d
{
	OverlayFactory::OverlayFactory()
		: Factory< OverlayCategory, OverlayType >()
	{
		registerType( OverlayType::ePanel, &PanelOverlay::create );
		registerType( OverlayType::eBorderPanel, &BorderPanelOverlay::create );
		registerType( OverlayType::eText, &TextOverlay::create );
	}

	OverlayFactory::~OverlayFactory()
	{
	}
}

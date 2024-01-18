#include "Castor3D/Overlay/OverlayFactory.hpp"

#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"

CU_ImplementSmartPtr( castor3d, OverlayFactory )

namespace castor3d
{
	OverlayFactory::OverlayFactory()
		: Factory< OverlayCategory, OverlayType, OverlayCategoryUPtr >()
	{
		registerType( OverlayType::ePanel, &PanelOverlay::create );
		registerType( OverlayType::eBorderPanel, &BorderPanelOverlay::create );
		registerType( OverlayType::eText, &TextOverlay::create );
	}
}

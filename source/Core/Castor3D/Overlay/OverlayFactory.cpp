#include "Castor3D/Overlay/OverlayFactory.hpp"

#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"

CU_ImplementCUSmartPtr( castor3d, OverlayFactory )

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

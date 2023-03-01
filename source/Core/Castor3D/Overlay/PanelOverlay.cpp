#include "Castor3D/Overlay/PanelOverlay.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Render/Overlays/OverlayRenderer.hpp"

CU_ImplementCUSmartPtr( castor3d, PanelOverlay )

namespace castor3d
{
	PanelOverlay::PanelOverlay()
		: OverlayCategory( OverlayType::ePanel )
	{
	}

	OverlayCategorySPtr PanelOverlay::create()
	{
		return std::make_shared< PanelOverlay >();
	}

	void PanelOverlay::accept( OverlayVisitor & visitor )const
	{
		visitor.visit( *this );
	}

	uint32_t PanelOverlay::fillBuffer( castor::Size const & renderSize
		, Vertex * buffer
		, bool secondary )const
	{
		return 6u;
	}

	void PanelOverlay::doUpdate( OverlayRenderer const & renderer )
	{
	}
}

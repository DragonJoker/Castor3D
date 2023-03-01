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
		double w = double( std::max( 1u, m_refSize.getWidth() ) );
		double h = double( std::max( 1u, m_refSize.getHeight() ) );
		auto absoluteSize = getAbsoluteSize( m_refSize );

		double l = 0.0;
		double t = 0.0;
		double r = double( absoluteSize.getWidth() );
		double b = double( absoluteSize.getHeight() );

		OverlayCategory::Vertex lt{ castor::Point2f{ l / w, t / h }, castor::Point2f{ float( m_uv[0] ), float( m_uv[3] ) } };
		OverlayCategory::Vertex lb{ castor::Point2f{ l / w, b / h }, castor::Point2f{ float( m_uv[0] ), float( m_uv[1] ) } };
		OverlayCategory::Vertex rb{ castor::Point2f{ r / w, b / h }, castor::Point2f{ float( m_uv[2] ), float( m_uv[1] ) } };
		OverlayCategory::Vertex rt{ castor::Point2f{ r / w, t / h }, castor::Point2f{ float( m_uv[2] ), float( m_uv[3] ) } };

		uint32_t index = 0;

		buffer[index++] = lt;
		buffer[index++] = lb;
		buffer[index++] = rb;

		buffer[index++] = lt;
		buffer[index++] = rb;
		buffer[index++] = rt;

		return index;
	}

	void PanelOverlay::doUpdate( OverlayRenderer const & renderer )
	{
		m_refSize = renderer.getSize();
	}
}

#include "Castor3D/Overlay/PanelOverlay.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Overlay/OverlayRenderer.hpp"
#include "Castor3D/Overlay/Overlay.hpp"

CU_ImplementCUSmartPtr( castor3d, PanelOverlay )

using namespace castor;

namespace castor3d
{
	PanelOverlay::PanelOverlay()
		: OverlayCategory( OverlayType::ePanel )
	{
		m_arrayVtx.resize( 6 );
	}

	PanelOverlay::~PanelOverlay()
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

	void PanelOverlay::doUpdateBuffer( Size const & size )
	{
		Size absoluteSize = getAbsoluteSize( size );

		int32_t l = 0;
		int32_t t = 0;
		int32_t r = absoluteSize.getWidth();
		int32_t b = absoluteSize.getHeight();

		OverlayCategory::Vertex lt = { Point2f{ float( l ) / size.getWidth(), float( t ) / size.getHeight() }, Point2f{ float( m_uv[0] ), float( m_uv[3] ) } };
		OverlayCategory::Vertex lb = { Point2f{ float( l ) / size.getWidth(), float( b ) / size.getHeight() }, Point2f{ float( m_uv[0] ), float( m_uv[1] ) } };
		OverlayCategory::Vertex rb = { Point2f{ float( r ) / size.getWidth(), float( b ) / size.getHeight() }, Point2f{ float( m_uv[2] ), float( m_uv[1] ) } };
		OverlayCategory::Vertex rt = { Point2f{ float( r ) / size.getWidth(), float( t ) / size.getHeight() }, Point2f{ float( m_uv[2] ), float( m_uv[3] ) } };

		m_arrayVtx[0] = lt;
		m_arrayVtx[1] = lb;
		m_arrayVtx[2] = rb;
		m_arrayVtx[3] = lt;
		m_arrayVtx[4] = rb;
		m_arrayVtx[5] = rt;
	}
}

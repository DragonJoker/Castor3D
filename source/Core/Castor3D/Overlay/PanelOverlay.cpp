#include "Castor3D/Overlay/PanelOverlay.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Overlay/Overlay.hpp"

CU_ImplementCUSmartPtr( castor3d, PanelOverlay )

namespace castor3d
{
	PanelOverlay::PanelOverlay()
		: OverlayCategory( OverlayType::ePanel )
	{
		m_arrayVtx.resize( 6 );
	}

	OverlayCategorySPtr PanelOverlay::create()
	{
		return std::make_shared< PanelOverlay >();
	}

	void PanelOverlay::accept( OverlayVisitor & visitor )const
	{
		visitor.visit( *this );
	}

	void PanelOverlay::doUpdateBuffer( castor::Size const & size )
	{
		castor::Size absoluteSize = getAbsoluteSize( size );

		int32_t l = 0;
		int32_t t = 0;
		auto r = int32_t( absoluteSize.getWidth() );
		auto b = int32_t( absoluteSize.getHeight() );

		OverlayCategory::Vertex lt = { castor::Point2f{ float( l ) / float( size.getWidth() ), float( t ) / float( size.getHeight() ) }
			, castor::Point2f{ float( m_uv[0] ), float( m_uv[3] ) } };
		OverlayCategory::Vertex lb = { castor::Point2f{ float( l ) / float( size.getWidth() ), float( b ) / float( size.getHeight() ) }
			, castor::Point2f{ float( m_uv[0] ), float( m_uv[1] ) } };
		OverlayCategory::Vertex rb = { castor::Point2f{ float( r ) / float( size.getWidth() ), float( b ) / float( size.getHeight() ) }
			, castor::Point2f{ float( m_uv[2] ), float( m_uv[1] ) } };
		OverlayCategory::Vertex rt = { castor::Point2f{ float( r ) / float( size.getWidth() ), float( t ) / float( size.getHeight() ) }
			, castor::Point2f{ float( m_uv[2] ), float( m_uv[3] ) } };

		m_arrayVtx[0] = lt;
		m_arrayVtx[1] = lb;
		m_arrayVtx[2] = rb;
		m_arrayVtx[3] = lt;
		m_arrayVtx[4] = rb;
		m_arrayVtx[5] = rt;
	}
}

#include "Castor3D/Overlay/PanelOverlay.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Overlay/Overlay.hpp"

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

	uint32_t PanelOverlay::fillBuffer( Vertex * buffer
		, bool secondary )const
	{
		auto absoluteSize = getAbsoluteSize( m_refSize );

		int32_t l = 0;
		int32_t t = 0;
		auto r = int32_t( absoluteSize.getWidth() );
		auto b = int32_t( absoluteSize.getHeight() );

		OverlayCategory::Vertex lt = { castor::Point2f{ float( l ) / float( m_refSize.getWidth() ), float( t ) / float( m_refSize.getHeight() ) }
		, castor::Point2f{ float( m_uv[0] ), float( m_uv[3] ) } };
		OverlayCategory::Vertex lb = { castor::Point2f{ float( l ) / float( m_refSize.getWidth() ), float( b ) / float( m_refSize.getHeight() ) }
		, castor::Point2f{ float( m_uv[0] ), float( m_uv[1] ) } };
		OverlayCategory::Vertex rb = { castor::Point2f{ float( r ) / float( m_refSize.getWidth() ), float( b ) / float( m_refSize.getHeight() ) }
		, castor::Point2f{ float( m_uv[2] ), float( m_uv[1] ) } };
		OverlayCategory::Vertex rt = { castor::Point2f{ float( r ) / float( m_refSize.getWidth() ), float( t ) / float( m_refSize.getHeight() ) }
		, castor::Point2f{ float( m_uv[2] ), float( m_uv[3] ) } };

		uint32_t index = 0;
		buffer[index++] = lt;
		buffer[index++] = lb;
		buffer[index++] = rb;
		buffer[index++] = lt;
		buffer[index++] = rb;
		buffer[index++] = rt;
		return index;
	}

	void PanelOverlay::doUpdateBuffer( castor::Size const & size )
	{
		m_refSize = size;
	}
}

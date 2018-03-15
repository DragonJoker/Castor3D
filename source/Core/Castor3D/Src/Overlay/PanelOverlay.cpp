#include "PanelOverlay.hpp"

#include "OverlayRenderer.hpp"
#include "Overlay.hpp"

using namespace castor;

namespace castor3d
{
	PanelOverlay::TextWriter::TextWriter( String const & tabs, PanelOverlay const * category )
		: OverlayCategory::TextWriter{ tabs }
		, m_category{ category }
	{
	}

	bool PanelOverlay::TextWriter::operator()( PanelOverlay const & overlay, TextFile & file )
	{
		Logger::logInfo( m_tabs + cuT( "Writing PanelOverlay " ) + overlay.getOverlayName() );
		bool result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "panel_overlay \"" ) + overlay.getOverlay().getName() + cuT( "\"\n" ) ) > 0
						&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		OverlayCategory::TextWriter::checkError( result, "PanelOverlay name" );

		if ( result )
		{
			result = OverlayCategory::TextWriter{ m_tabs }( overlay, file );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	bool PanelOverlay::TextWriter::writeInto( castor::TextFile & file )
	{
		return ( *this )( *m_category, file );
	}

	//*************************************************************************************************

	PanelOverlay::PanelOverlay()
		:	OverlayCategory( OverlayType::ePanel )
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

	void PanelOverlay::doRender( OverlayRenderer & renderer )
	{
		renderer.drawPanel( *this );
	}

	void PanelOverlay::doUpdateBuffer( Size const & size )
	{
		Size absoluteSize = getAbsoluteSize( size );

		int32_t centerL = 0;
		int32_t centerT = 0;
		int32_t centerR = absoluteSize.getWidth();
		int32_t centerB = absoluteSize.getHeight();

		OverlayCategory::Vertex vertex0 = { Point2f{ float( centerL ) / size.getWidth(), float( centerT ) / size.getHeight() }, Point2f{ real( m_uv[0] ), real( m_uv[3] ) } };
		OverlayCategory::Vertex vertex1 = { Point2f{ float( centerL ) / size.getWidth(), float( centerB ) / size.getHeight() }, Point2f{ real( m_uv[0] ), real( m_uv[1] ) } };
		OverlayCategory::Vertex vertex2 = { Point2f{ float( centerR ) / size.getWidth(), float( centerB ) / size.getHeight() }, Point2f{ real( m_uv[2] ), real( m_uv[1] ) } };
		OverlayCategory::Vertex vertex3 = { Point2f{ float( centerL ) / size.getWidth(), float( centerT ) / size.getHeight() }, Point2f{ real( m_uv[0] ), real( m_uv[3] ) } };
		OverlayCategory::Vertex vertex4 = { Point2f{ float( centerR ) / size.getWidth(), float( centerB ) / size.getHeight() }, Point2f{ real( m_uv[2] ), real( m_uv[1] ) } };
		OverlayCategory::Vertex vertex5 = { Point2f{ float( centerR ) / size.getWidth(), float( centerT ) / size.getHeight() }, Point2f{ real( m_uv[2] ), real( m_uv[3] ) } };

		m_arrayVtx[0] = vertex0;
		m_arrayVtx[1] = vertex1;
		m_arrayVtx[2] = vertex2;
		m_arrayVtx[3] = vertex3;
		m_arrayVtx[4] = vertex4;
		m_arrayVtx[5] = vertex5;
	}
}

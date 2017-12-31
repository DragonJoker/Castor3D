#include "PanelOverlay.hpp"

#include "OverlayRenderer.hpp"
#include "Overlay.hpp"

using namespace castor;

namespace castor3d
{
	PanelOverlay::TextWriter::TextWriter( String const & p_tabs, PanelOverlay const * p_category )
		: OverlayCategory::TextWriter{ p_tabs }
		, m_category{ p_category }
	{
	}

	bool PanelOverlay::TextWriter::operator()( PanelOverlay const & p_overlay, TextFile & p_file )
	{
		Logger::logInfo( m_tabs + cuT( "Writing PanelOverlay " ) + p_overlay.getOverlayName() );
		bool result = p_file.writeText( cuT( "\n" ) + m_tabs + cuT( "panel_overlay \"" ) + p_overlay.getOverlay().getName() + cuT( "\"\n" ) ) > 0
						&& p_file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		OverlayCategory::TextWriter::checkError( result, "PanelOverlay name" );

		if ( result )
		{
			result = OverlayCategory::TextWriter{ m_tabs }( p_overlay, p_file );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	bool PanelOverlay::TextWriter::writeInto( castor::TextFile & p_file )
	{
		return ( *this )( *m_category, p_file );
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

	void PanelOverlay::doRender( OverlayRendererSPtr p_renderer )
	{
		p_renderer->drawPanel( *this );
	}

	void PanelOverlay::doUpdateBuffer( Size const & p_size )
	{
		Position pos = getAbsolutePosition( p_size );
		Size size = getAbsoluteSize( p_size );

		int32_t centerL = 0;
		int32_t centerT = 0;
		int32_t centerR = size.getWidth();
		int32_t centerB = size.getHeight();

		OverlayCategory::Vertex vertex0 = { { float( centerL ) / p_size.getWidth(), float( centerT ) / p_size.getHeight() }, { real( m_uv[0] ), real( m_uv[3] ) } };
		OverlayCategory::Vertex vertex1 = { { float( centerL ) / p_size.getWidth(), float( centerB ) / p_size.getHeight() }, { real( m_uv[0] ), real( m_uv[1] ) } };
		OverlayCategory::Vertex vertex2 = { { float( centerR ) / p_size.getWidth(), float( centerB ) / p_size.getHeight() }, { real( m_uv[2] ), real( m_uv[1] ) } };
		OverlayCategory::Vertex vertex3 = { { float( centerL ) / p_size.getWidth(), float( centerT ) / p_size.getHeight() }, { real( m_uv[0] ), real( m_uv[3] ) } };
		OverlayCategory::Vertex vertex4 = { { float( centerR ) / p_size.getWidth(), float( centerB ) / p_size.getHeight() }, { real( m_uv[2] ), real( m_uv[1] ) } };
		OverlayCategory::Vertex vertex5 = { { float( centerR ) / p_size.getWidth(), float( centerT ) / p_size.getHeight() }, { real( m_uv[2] ), real( m_uv[3] ) } };

		m_arrayVtx[0] = vertex0;
		m_arrayVtx[1] = vertex1;
		m_arrayVtx[2] = vertex2;
		m_arrayVtx[3] = vertex3;
		m_arrayVtx[4] = vertex4;
		m_arrayVtx[5] = vertex5;
	}
}

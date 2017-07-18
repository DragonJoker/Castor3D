#include "PanelOverlay.hpp"

#include "OverlayRenderer.hpp"
#include "Overlay.hpp"

using namespace Castor;

namespace Castor3D
{
	PanelOverlay::TextWriter::TextWriter( String const & p_tabs, PanelOverlay const * p_category )
		: OverlayCategory::TextWriter{ p_tabs }
		, m_category{ p_category }
	{
	}

	bool PanelOverlay::TextWriter::operator()( PanelOverlay const & p_overlay, TextFile & p_file )
	{
		Logger::LogInfo( m_tabs + cuT( "Writing PanelOverlay " ) + p_overlay.GetOverlayName() );
		bool result = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "panel_overlay \"" ) + p_overlay.GetOverlay().GetName() + cuT( "\"\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
		OverlayCategory::TextWriter::CheckError( result, "PanelOverlay name" );

		if ( result )
		{
			result = OverlayCategory::TextWriter{ m_tabs }( p_overlay, p_file );
		}

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	bool PanelOverlay::TextWriter::WriteInto( Castor::TextFile & p_file )
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

	OverlayCategorySPtr PanelOverlay::Create()
	{
		return std::make_shared< PanelOverlay >();
	}

	void PanelOverlay::DoRender( OverlayRendererSPtr p_renderer )
	{
		p_renderer->DrawPanel( *this );
	}

	void PanelOverlay::DoUpdateBuffer( Size const & p_size )
	{
		Position pos = GetAbsolutePosition( p_size );
		Size size = GetAbsoluteSize( p_size );

		int32_t centerL = 0;
		int32_t centerT = 0;
		int32_t centerR = size.width();
		int32_t centerB = size.height();

		OverlayCategory::Vertex vertex0 = { { centerL, centerT }, { real( m_uv[0] ), real( m_uv[3] ) } };
		OverlayCategory::Vertex vertex1 = { { centerL, centerB }, { real( m_uv[0] ), real( m_uv[1] ) } };
		OverlayCategory::Vertex vertex2 = { { centerR, centerB }, { real( m_uv[2] ), real( m_uv[1] ) } };
		OverlayCategory::Vertex vertex3 = { { centerL, centerT }, { real( m_uv[0] ), real( m_uv[3] ) } };
		OverlayCategory::Vertex vertex4 = { { centerR, centerB }, { real( m_uv[2] ), real( m_uv[1] ) } };
		OverlayCategory::Vertex vertex5 = { { centerR, centerT }, { real( m_uv[2] ), real( m_uv[3] ) } };

		m_arrayVtx[0] = vertex0;
		m_arrayVtx[1] = vertex1;
		m_arrayVtx[2] = vertex2;
		m_arrayVtx[3] = vertex3;
		m_arrayVtx[4] = vertex4;
		m_arrayVtx[5] = vertex5;
	}
}

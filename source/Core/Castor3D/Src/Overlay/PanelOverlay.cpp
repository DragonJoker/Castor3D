#include "PanelOverlay.hpp"
#include "OverlayRenderer.hpp"
#include "Overlay.hpp"

using namespace Castor;

namespace Castor3D
{
	bool PanelOverlay::TextLoader::operator()( PanelOverlay const & p_overlay, TextFile & p_file )
	{
		String l_strTabs;
		OverlaySPtr l_pParent = p_overlay.GetOverlay().GetParent();

		while ( l_pParent )
		{
			l_strTabs += cuT( '\t' );
			l_pParent = l_pParent->GetParent();
		}

		bool l_return = p_file.WriteText( l_strTabs + cuT( "panel_overlay " ) + p_overlay.GetOverlay().GetName() + cuT( "\n" ) + l_strTabs + cuT( "{\n" ) ) > 0;

		if ( l_return )
		{
			l_return = Overlay::TextLoader()( p_overlay.GetOverlay(), p_file );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( l_strTabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	PanelOverlay::BinaryParser::BinaryParser( Path const & p_path )
		:	OverlayCategory::BinaryParser( p_path )
	{
	}

	bool PanelOverlay::BinaryParser::Fill( PanelOverlay const & p_obj, BinaryChunk & p_chunk )const
	{
		return true;
	}

	bool PanelOverlay::BinaryParser::Parse( PanelOverlay & p_obj, BinaryChunk & p_chunk )const
	{
		return true;
	}

	//*************************************************************************************************

	PanelOverlay::PanelOverlay()
		:	OverlayCategory( eOVERLAY_TYPE_PANEL )
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
		Position l_pos = GetAbsolutePosition( p_size );
		Size l_size = GetAbsoluteSize( p_size );

		int32_t l_centerL = l_pos.x();
		int32_t l_centerT = l_pos.y();
		int32_t l_centerR = l_pos.x() + l_size.width();
		int32_t l_centerB = l_pos.y() + l_size.height();

		OverlayCategory::Vertex l_vertex0 = { { l_centerL, l_centerT }, { real( m_uv[0] ), real( m_uv[3] ) } };
		OverlayCategory::Vertex l_vertex1 = { { l_centerL, l_centerB }, { real( m_uv[0] ), real( m_uv[1] ) } };
		OverlayCategory::Vertex l_vertex2 = { { l_centerR, l_centerB }, { real( m_uv[2] ), real( m_uv[1] ) } };
		OverlayCategory::Vertex l_vertex3 = { { l_centerL, l_centerT }, { real( m_uv[0] ), real( m_uv[3] ) } };
		OverlayCategory::Vertex l_vertex4 = { { l_centerR, l_centerB }, { real( m_uv[2] ), real( m_uv[1] ) } };
		OverlayCategory::Vertex l_vertex5 = { { l_centerR, l_centerT }, { real( m_uv[2] ), real( m_uv[3] ) } };

		m_arrayVtx[0] = l_vertex0;
		m_arrayVtx[1] = l_vertex1;
		m_arrayVtx[2] = l_vertex2;
		m_arrayVtx[3] = l_vertex3;
		m_arrayVtx[4] = l_vertex4;
		m_arrayVtx[5] = l_vertex5;
	}
}

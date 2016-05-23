#include "BorderPanelOverlay.hpp"

#include "Overlay.hpp"
#include "OverlayManager.hpp"
#include "OverlayRenderer.hpp"
#include "MaterialManager.hpp"
#include "Engine.hpp"

using namespace Castor;

namespace Castor3D
{
	BorderPanelOverlay::TextLoader::TextLoader( String const & p_tabs, File::eENCODING_MODE p_encodingMode )
		: OverlayCategory::TextLoader( p_tabs, p_encodingMode )
	{
	}

	bool BorderPanelOverlay::TextLoader::operator()( BorderPanelOverlay const & p_overlay, TextFile & p_file )
	{
		bool l_return = p_file.WriteText( m_tabs + cuT( "border_panel_overlay " ) + p_overlay.GetOverlay().GetName() + cuT( "\n" ) + m_tabs + cuT( "{\n" ) ) > 0;

		if ( l_return )
		{
			l_return = p_file.Print( 1024, cuT( "%S\tborder_size " ), m_tabs.c_str() ) > 0;
		}

		if ( l_return )
		{
			l_return = Point4d::TextLoader( m_tabs )( p_overlay.GetBorderSize(), p_file );
		}

		if ( l_return && p_overlay.GetBorderMaterial() )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tborder_material " ) + p_overlay.GetBorderMaterial()->GetName() ) > 0;
		}

		if ( l_return )
		{
			l_return = Overlay::TextLoader( m_tabs )( p_overlay.GetOverlay(), p_file );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	BorderPanelOverlay::BorderPanelOverlay()
		: OverlayCategory( eOVERLAY_TYPE_BORDER_PANEL )
		, m_borderOuterUv( 0, 0, 1, 1 )
		, m_borderInnerUv( 0.33, 0.33, 0.66, 0.66 )
		, m_borderChanged( true )
	{
		m_arrayVtx.resize( 6 );
		m_arrayVtxBorder.resize( 48 );
	}

	BorderPanelOverlay::~BorderPanelOverlay()
	{
	}

	OverlayCategorySPtr BorderPanelOverlay::Create()
	{
		return std::make_shared< BorderPanelOverlay >();
	}

	void BorderPanelOverlay::SetBorderMaterial( MaterialSPtr p_pMaterial )
	{
		m_pBorderMaterial = p_pMaterial;

		if ( p_pMaterial )
		{
			m_strBorderMatName = p_pMaterial->GetName();
		}
		else
		{
			m_strBorderMatName = cuEmptyString;
		}
	}

	void BorderPanelOverlay::DoUpdateSize()
	{
		OverlayCategory::DoUpdateSize();
		OverlayRendererSPtr l_renderer = GetOverlay().GetOverlayManager().GetRenderer();

		if ( l_renderer )
		{
			if ( IsSizeChanged() || IsChanged() || l_renderer->IsSizeChanged() )
			{
				OverlaySPtr l_parent = GetOverlay().GetParent();
				Size l_sz = l_renderer->GetSize();
				Point2d l_totalSize( l_sz.width(), l_sz.height() );

				if ( l_parent )
				{
					Point2d l_parentSize = l_parent->GetAbsoluteSize();
					l_totalSize[0] = l_parentSize[0] * l_totalSize[0];
					l_totalSize[1] = l_parentSize[1] * l_totalSize[1];
				}

				Rectangle l_sizes = GetBorderPixelSize();
				Point4d l_ptSizes = GetBorderSize();
				bool l_changed = m_borderChanged;

				if ( l_sizes.left() )
				{
					l_changed = !Castor::Policy< double >::equals( l_ptSizes[0], l_sizes.left() / l_totalSize[0] );
					l_ptSizes[0] = l_sizes.left() / l_totalSize[0];
				}

				if ( l_sizes.top() )
				{
					l_changed = !Castor::Policy< double >::equals( l_ptSizes[1], l_sizes.top() / l_totalSize[1] );
					l_ptSizes[1] = l_sizes.top() / l_totalSize[1];
				}

				if ( l_sizes.right() )
				{
					l_changed = !Castor::Policy< double >::equals( l_ptSizes[2], l_sizes.right() / l_totalSize[0] );
					l_ptSizes[2] = l_sizes.right() / l_totalSize[0];
				}

				if ( l_sizes.bottom() )
				{
					l_changed = !Castor::Policy< double >::equals( l_ptSizes[3], l_sizes.bottom() / l_totalSize[1] );
					l_ptSizes[3] = l_sizes.bottom() / l_totalSize[1];
				}

				if ( l_changed )
				{
					SetBorderSize( l_ptSizes );
				}
			}
		}
	}

	Rectangle BorderPanelOverlay::GetAbsoluteBorderSize( Castor::Size const & p_size )const
	{
		Point4d l_size = GetAbsoluteBorderSize();

		return Rectangle(
				   int32_t( l_size[0] * p_size.width() ),
				   int32_t( l_size[1] * p_size.height() ),
				   int32_t( l_size[2] * p_size.width() ),
				   int32_t( l_size[3] * p_size.height() )
			   );
	}

	Point4d BorderPanelOverlay::GetAbsoluteBorderSize()const
	{
		Point4d l_size = GetBorderSize();
		OverlaySPtr l_parent = GetOverlay().GetParent();

		if ( l_parent )
		{
			Point2d l_parentSize = l_parent->GetAbsoluteSize();
			l_size[0] *= l_parentSize[0];
			l_size[1] *= l_parentSize[1];
			l_size[2] *= l_parentSize[0];
			l_size[3] *= l_parentSize[1];
		}

		return l_size;
	}

	void BorderPanelOverlay::DoRender( OverlayRendererSPtr p_renderer )
	{
		p_renderer->DrawBorderPanel( *this );
	}

	void BorderPanelOverlay::DoUpdateBuffer( Size const & p_size )
	{
		Position l_pos = GetAbsolutePosition( p_size );
		Size l_size = GetAbsoluteSize( p_size );
		Rectangle l_sizes = GetAbsoluteBorderSize( p_size );

		int32_t l_centerL = l_pos.x();
		int32_t l_centerT = l_pos.y();
		int32_t l_centerR = l_pos.x() + l_size.width();
		int32_t l_centerB = l_pos.y() + l_size.height();

		if ( m_borderPosition == eBORDER_POSITION_INTERNAL )
		{
			l_centerL += l_sizes.left();
			l_centerT += l_sizes.top();
			l_centerR -= l_sizes.right();
			l_centerB -= l_sizes.bottom();
		}
		else if ( m_borderPosition == eBORDER_POSITION_MIDDLE )
		{
			l_centerL += l_sizes.left() / 2;
			l_centerT += l_sizes.top() / 2;
			l_centerR -= l_sizes.right() / 2;
			l_centerB -= l_sizes.bottom() / 2;
		}

		int32_t l_borderL = l_centerL - l_sizes.left();
		int32_t l_borderT = l_centerT - l_sizes.top();
		int32_t l_borderR = l_centerR + l_sizes.right();
		int32_t l_borderB = l_centerB + l_sizes.bottom();

		real l_borderUvLL = real( m_borderOuterUv[0] );
		real l_borderUvTT = real( m_borderOuterUv[1] );
		real l_borderUvML = real( m_borderInnerUv[0] );
		real l_borderUvMT = real( m_borderInnerUv[1] );
		real l_borderUvMR = real( m_borderInnerUv[2] );
		real l_borderUvMB = real( m_borderInnerUv[3] );
		real l_borderUvRR = real( m_borderOuterUv[2] );
		real l_borderUvBB = real( m_borderOuterUv[3] );

		// Center
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

		// Corner Top Left
		uint32_t l_index = 0;
		OverlayCategory::Vertex l_cornerTL0 = { { l_borderL, l_borderT }, { l_borderUvLL, l_borderUvTT } };
		OverlayCategory::Vertex l_cornerTL1 = { { l_borderL, l_centerT }, { l_borderUvLL, l_borderUvMT } };
		OverlayCategory::Vertex l_cornerTL2 = { { l_centerL, l_centerT }, { l_borderUvML, l_borderUvMT } };
		OverlayCategory::Vertex l_cornerTL3 = { { l_borderL, l_borderT }, { l_borderUvLL, l_borderUvTT } };
		OverlayCategory::Vertex l_cornerTL4 = { { l_centerL, l_centerT }, { l_borderUvML, l_borderUvMT } };
		OverlayCategory::Vertex l_cornerTL5 = { { l_centerL, l_borderT }, { l_borderUvML, l_borderUvTT } };
		m_arrayVtxBorder[l_index++] = l_cornerTL0;
		m_arrayVtxBorder[l_index++] = l_cornerTL1;
		m_arrayVtxBorder[l_index++] = l_cornerTL2;
		m_arrayVtxBorder[l_index++] = l_cornerTL3;
		m_arrayVtxBorder[l_index++] = l_cornerTL4;
		m_arrayVtxBorder[l_index++] = l_cornerTL5;

		// Border Top
		OverlayCategory::Vertex l_borderT0 = { { l_centerL, l_borderT }, { l_borderUvML, l_borderUvTT } };
		OverlayCategory::Vertex l_borderT1 = { { l_centerL, l_centerT }, { l_borderUvML, l_borderUvMT } };
		OverlayCategory::Vertex l_borderT2 = { { l_centerR, l_centerT }, { l_borderUvMR, l_borderUvMT } };
		OverlayCategory::Vertex l_borderT3 = { { l_centerL, l_borderT }, { l_borderUvML, l_borderUvTT } };
		OverlayCategory::Vertex l_borderT4 = { { l_centerR, l_centerT }, { l_borderUvMR, l_borderUvMT } };
		OverlayCategory::Vertex l_borderT5 = { { l_centerR, l_borderT }, { l_borderUvMR, l_borderUvTT } };
		m_arrayVtxBorder[l_index++] = l_borderT0;
		m_arrayVtxBorder[l_index++] = l_borderT1;
		m_arrayVtxBorder[l_index++] = l_borderT2;
		m_arrayVtxBorder[l_index++] = l_borderT3;
		m_arrayVtxBorder[l_index++] = l_borderT4;
		m_arrayVtxBorder[l_index++] = l_borderT5;

		// Corner Top Right
		OverlayCategory::Vertex l_cornerTR0 = { { l_centerR, l_borderT }, { l_borderUvMR, l_borderUvTT } };
		OverlayCategory::Vertex l_cornerTR1 = { { l_centerR, l_centerT }, { l_borderUvMR, l_borderUvMT } };
		OverlayCategory::Vertex l_cornerTR2 = { { l_borderR, l_centerT }, { l_borderUvRR, l_borderUvMT } };
		OverlayCategory::Vertex l_cornerTR3 = { { l_centerR, l_borderT }, { l_borderUvMR, l_borderUvTT } };
		OverlayCategory::Vertex l_cornerTR4 = { { l_borderR, l_centerT }, { l_borderUvRR, l_borderUvMT } };
		OverlayCategory::Vertex l_cornerTR5 = { { l_borderR, l_borderT }, { l_borderUvRR, l_borderUvTT } };
		m_arrayVtxBorder[l_index++] = l_cornerTR0;
		m_arrayVtxBorder[l_index++] = l_cornerTR1;
		m_arrayVtxBorder[l_index++] = l_cornerTR2;
		m_arrayVtxBorder[l_index++] = l_cornerTR3;
		m_arrayVtxBorder[l_index++] = l_cornerTR4;
		m_arrayVtxBorder[l_index++] = l_cornerTR5;

		// Border Left
		OverlayCategory::Vertex l_borderL0 = { { l_borderL, l_centerT }, { l_borderUvLL, l_borderUvMT } };
		OverlayCategory::Vertex l_borderL1 = { { l_borderL, l_centerB }, { l_borderUvLL, l_borderUvMB } };
		OverlayCategory::Vertex l_borderL2 = { { l_centerL, l_centerB }, { l_borderUvML, l_borderUvMB } };
		OverlayCategory::Vertex l_borderL3 = { { l_borderL, l_centerT }, { l_borderUvLL, l_borderUvMT } };
		OverlayCategory::Vertex l_borderL4 = { { l_centerL, l_centerB }, { l_borderUvML, l_borderUvMB } };
		OverlayCategory::Vertex l_borderL5 = { { l_centerL, l_centerT }, { l_borderUvML, l_borderUvMT } };
		m_arrayVtxBorder[l_index++] = l_borderL0;
		m_arrayVtxBorder[l_index++] = l_borderL1;
		m_arrayVtxBorder[l_index++] = l_borderL2;
		m_arrayVtxBorder[l_index++] = l_borderL3;
		m_arrayVtxBorder[l_index++] = l_borderL4;
		m_arrayVtxBorder[l_index++] = l_borderL5;

		// Border Right
		OverlayCategory::Vertex l_borderR0 = { { l_centerR, l_centerT }, { l_borderUvMR, l_borderUvMT } };
		OverlayCategory::Vertex l_borderR1 = { { l_centerR, l_centerB }, { l_borderUvMR, l_borderUvMB } };
		OverlayCategory::Vertex l_borderR2 = { { l_borderR, l_centerB }, { l_borderUvRR, l_borderUvMB } };
		OverlayCategory::Vertex l_borderR3 = { { l_centerR, l_centerT }, { l_borderUvMR, l_borderUvMT } };
		OverlayCategory::Vertex l_borderR4 = { { l_borderR, l_centerB }, { l_borderUvRR, l_borderUvMB } };
		OverlayCategory::Vertex l_borderR5 = { { l_borderR, l_centerT }, { l_borderUvRR, l_borderUvMT } };
		m_arrayVtxBorder[l_index++] = l_borderR0;
		m_arrayVtxBorder[l_index++] = l_borderR1;
		m_arrayVtxBorder[l_index++] = l_borderR2;
		m_arrayVtxBorder[l_index++] = l_borderR3;
		m_arrayVtxBorder[l_index++] = l_borderR4;
		m_arrayVtxBorder[l_index++] = l_borderR5;

		// Corner Bottom Left
		OverlayCategory::Vertex l_cornerBL0 = { { l_borderL, l_centerB }, { l_borderUvLL, l_borderUvMB } };
		OverlayCategory::Vertex l_cornerBL1 = { { l_borderL, l_borderB }, { l_borderUvLL, l_borderUvBB } };
		OverlayCategory::Vertex l_cornerBL2 = { { l_centerL, l_borderB }, { l_borderUvML, l_borderUvBB } };
		OverlayCategory::Vertex l_cornerBL3 = { { l_borderL, l_centerB }, { l_borderUvLL, l_borderUvMB } };
		OverlayCategory::Vertex l_cornerBL4 = { { l_centerL, l_borderB }, { l_borderUvML, l_borderUvBB } };
		OverlayCategory::Vertex l_cornerBL5 = { { l_centerL, l_centerB }, { l_borderUvML, l_borderUvMB } };
		m_arrayVtxBorder[l_index++] = l_cornerBL0;
		m_arrayVtxBorder[l_index++] = l_cornerBL1;
		m_arrayVtxBorder[l_index++] = l_cornerBL2;
		m_arrayVtxBorder[l_index++] = l_cornerBL3;
		m_arrayVtxBorder[l_index++] = l_cornerBL4;
		m_arrayVtxBorder[l_index++] = l_cornerBL5;

		// Border Bottom
		OverlayCategory::Vertex l_borderB0 = { { l_centerL, l_centerB }, { l_borderUvML, l_borderUvMB } };
		OverlayCategory::Vertex l_borderB1 = { { l_centerL, l_borderB }, { l_borderUvML, l_borderUvBB } };
		OverlayCategory::Vertex l_borderB2 = { { l_centerR, l_borderB }, { l_borderUvMR, l_borderUvBB } };
		OverlayCategory::Vertex l_borderB3 = { { l_centerL, l_centerB }, { l_borderUvML, l_borderUvMB } };
		OverlayCategory::Vertex l_borderB4 = { { l_centerR, l_borderB }, { l_borderUvMR, l_borderUvBB } };
		OverlayCategory::Vertex l_borderB5 = { { l_centerR, l_centerB }, { l_borderUvMR, l_borderUvMB } };
		m_arrayVtxBorder[l_index++] = l_borderB0;
		m_arrayVtxBorder[l_index++] = l_borderB1;
		m_arrayVtxBorder[l_index++] = l_borderB2;
		m_arrayVtxBorder[l_index++] = l_borderB3;
		m_arrayVtxBorder[l_index++] = l_borderB4;
		m_arrayVtxBorder[l_index++] = l_borderB5;

		// Corner Bottom Right
		OverlayCategory::Vertex l_cornerBR0 = { { l_centerR, l_centerB }, { l_borderUvMR, l_borderUvMB } };
		OverlayCategory::Vertex l_cornerBR1 = { { l_centerR, l_borderB }, { l_borderUvMR, l_borderUvBB } };
		OverlayCategory::Vertex l_cornerBR2 = { { l_borderR, l_borderB }, { l_borderUvRR, l_borderUvBB } };
		OverlayCategory::Vertex l_cornerBR3 = { { l_centerR, l_centerB }, { l_borderUvMR, l_borderUvMB } };
		OverlayCategory::Vertex l_cornerBR4 = { { l_borderR, l_borderB }, { l_borderUvRR, l_borderUvBB } };
		OverlayCategory::Vertex l_cornerBR5 = { { l_borderR, l_centerB }, { l_borderUvRR, l_borderUvMB } };
		m_arrayVtxBorder[l_index++] = l_cornerBR0;
		m_arrayVtxBorder[l_index++] = l_cornerBR1;
		m_arrayVtxBorder[l_index++] = l_cornerBR2;
		m_arrayVtxBorder[l_index++] = l_cornerBR3;
		m_arrayVtxBorder[l_index++] = l_cornerBR4;
		m_arrayVtxBorder[l_index++] = l_cornerBR5;
	}
}

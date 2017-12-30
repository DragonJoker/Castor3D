#include "BorderPanelOverlay.hpp"

#include "Overlay.hpp"
#include "OverlayRenderer.hpp"
#include "Engine.hpp"

#include "Material/Material.hpp"

using namespace castor;

namespace castor3d
{
	BorderPanelOverlay::TextWriter::TextWriter( String const & p_tabs, BorderPanelOverlay const * p_category )
		: OverlayCategory::TextWriter{ p_tabs }
		, m_category{ p_category }
	{
	}

	bool BorderPanelOverlay::TextWriter::operator()( BorderPanelOverlay const & p_overlay, TextFile & p_file )
	{
		Logger::logInfo( m_tabs + cuT( "Writing BorderPanelOverlay " ) + p_overlay.getOverlayName() );
		bool result = p_file.writeText( cuT( "\n" ) + m_tabs + cuT( "border_panel_overlay \"" ) + p_overlay.getOverlay().getName() + cuT( "\"\n" ) ) > 0
						&& p_file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		OverlayCategory::TextWriter::checkError( result, "BorderPanelOverlay name" );

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\tborder_size " ) ) > 0
					   && Point4d::TextWriter{ String{} }( p_overlay.getBorderSize(), p_file )
					   && p_file.writeText( cuT( "\n" ) ) > 0;
			OverlayCategory::TextWriter::checkError( result, "BorderPanelOverlay borders size" );
		}

		if ( result && p_overlay.getBorderMaterial() )
		{
			result = p_file.writeText( m_tabs + cuT( "\tborder_material \"" ) + p_overlay.getBorderMaterial()->getName() + cuT( "\"\n" ) ) > 0;
			OverlayCategory::TextWriter::checkError( result, "BorderPanelOverlay borders material" );
		}

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

	bool BorderPanelOverlay::TextWriter::writeInto( castor::TextFile & p_file )
	{
		return ( *this )( *m_category, p_file );
	}

	//*************************************************************************************************

	BorderPanelOverlay::BorderPanelOverlay()
		: OverlayCategory( OverlayType::eBorderPanel )
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

	OverlayCategorySPtr BorderPanelOverlay::create()
	{
		return std::make_shared< BorderPanelOverlay >();
	}

	void BorderPanelOverlay::setBorderMaterial( MaterialSPtr p_material )
	{
		m_pBorderMaterial = p_material;

		if ( p_material )
		{
			m_strBorderMatName = p_material->getName();
		}
		else
		{
			m_strBorderMatName = cuEmptyString;
		}
	}

	void BorderPanelOverlay::doUpdateSize()
	{
		OverlayCategory::doUpdateSize();
		OverlayRendererSPtr renderer = getOverlay().getEngine()->getOverlayCache().getRenderer();

		if ( renderer )
		{
			if ( isSizeChanged() || isChanged() || renderer->isSizeChanged() )
			{
				OverlaySPtr parent = getOverlay().getParent();
				Size sz = renderer->getSize();
				Point2d totalSize( sz.getWidth(), sz.getHeight() );

				if ( parent )
				{
					Point2d parentSize = parent->getAbsoluteSize();
					totalSize[0] = parentSize[0] * totalSize[0];
					totalSize[1] = parentSize[1] * totalSize[1];
				}

				Rectangle sizes = getBorderPixelSize();
				Point4d ptSizes = getBorderSize();
				bool changed = m_borderChanged;

				if ( sizes.left() )
				{
					changed |= ptSizes[0] != double( sizes.left() ) / totalSize[0];
					ptSizes[0] = sizes.left() / totalSize[0];
				}

				if ( sizes.top() )
				{
					changed |= ptSizes[1] != double(  sizes.top() ) / totalSize[1];
					ptSizes[1] = sizes.top() / totalSize[1];
				}

				if ( sizes.right() )
				{
					changed |= ptSizes[2] != double( sizes.right() ) / totalSize[0];
					ptSizes[2] = sizes.right() / totalSize[0];
				}

				if ( sizes.bottom() )
				{
					changed |= ptSizes[3] != double( sizes.bottom() ) / totalSize[1];
					ptSizes[3] = sizes.bottom() / totalSize[1];
				}

				if ( changed )
				{
					setBorderSize( ptSizes );
				}
			}
		}
	}

	Rectangle BorderPanelOverlay::getAbsoluteBorderSize( castor::Size const & p_size )const
	{
		Point4d size = getAbsoluteBorderSize();

		return Rectangle(
				   int32_t( size[0] * p_size.getWidth() ),
				   int32_t( size[1] * p_size.getHeight() ),
				   int32_t( size[2] * p_size.getWidth() ),
				   int32_t( size[3] * p_size.getHeight() )
			   );
	}

	Point4d BorderPanelOverlay::getAbsoluteBorderSize()const
	{
		Point4d size = getBorderSize();
		OverlaySPtr parent = getOverlay().getParent();

		if ( parent )
		{
			Point2d parentSize = parent->getAbsoluteSize();
			size[0] *= parentSize[0];
			size[1] *= parentSize[1];
			size[2] *= parentSize[0];
			size[3] *= parentSize[1];
		}

		return size;
	}

	void BorderPanelOverlay::doRender( OverlayRendererSPtr p_renderer )
	{
		p_renderer->drawBorderPanel( *this );
	}

	void BorderPanelOverlay::doUpdateBuffer( Size const & p_size )
	{
		Position pos = getAbsolutePosition( p_size );
		Size size = getAbsoluteSize( p_size );
		Rectangle sizes = getAbsoluteBorderSize( p_size );

		int32_t centerL = 0;
		int32_t centerT = 0;
		int32_t centerR = size.getWidth();
		int32_t centerB = size.getHeight();

		if ( m_borderPosition == BorderPosition::eInternal )
		{
			centerL += sizes.left();
			centerT += sizes.top();
			centerR -= sizes.right();
			centerB -= sizes.bottom();
		}
		else if ( m_borderPosition == BorderPosition::eMiddle )
		{
			centerL += sizes.left() / 2;
			centerT += sizes.top() / 2;
			centerR -= sizes.right() / 2;
			centerB -= sizes.bottom() / 2;
		}

		int32_t borderL = centerL - sizes.left();
		int32_t borderT = centerT - sizes.top();
		int32_t borderR = centerR + sizes.right();
		int32_t borderB = centerB + sizes.bottom();

		real borderUvLL = real( m_borderOuterUv[0] );
		real borderUvTT = real( m_borderOuterUv[1] );
		real borderUvML = real( m_borderInnerUv[0] );
		real borderUvMT = real( m_borderInnerUv[1] );
		real borderUvMR = real( m_borderInnerUv[2] );
		real borderUvMB = real( m_borderInnerUv[3] );
		real borderUvRR = real( m_borderOuterUv[2] );
		real borderUvBB = real( m_borderOuterUv[3] );

		// Center
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

		// Corner Top Left
		uint32_t index = 0;
		OverlayCategory::Vertex cornerTL0 = { { float( borderL ) / p_size.getWidth(), float( borderT ) / p_size.getHeight() }, { borderUvLL, borderUvTT } };
		OverlayCategory::Vertex cornerTL1 = { { float( borderL ) / p_size.getWidth(), float( centerT ) / p_size.getHeight() }, { borderUvLL, borderUvMT } };
		OverlayCategory::Vertex cornerTL2 = { { float( centerL ) / p_size.getWidth(), float( centerT ) / p_size.getHeight() }, { borderUvML, borderUvMT } };
		OverlayCategory::Vertex cornerTL3 = { { float( borderL ) / p_size.getWidth(), float( borderT ) / p_size.getHeight() }, { borderUvLL, borderUvTT } };
		OverlayCategory::Vertex cornerTL4 = { { float( centerL ) / p_size.getWidth(), float( centerT ) / p_size.getHeight() }, { borderUvML, borderUvMT } };
		OverlayCategory::Vertex cornerTL5 = { { float( centerL ) / p_size.getWidth(), float( borderT ) / p_size.getHeight() }, { borderUvML, borderUvTT } };
		m_arrayVtxBorder[index++] = cornerTL0;
		m_arrayVtxBorder[index++] = cornerTL1;
		m_arrayVtxBorder[index++] = cornerTL2;
		m_arrayVtxBorder[index++] = cornerTL3;
		m_arrayVtxBorder[index++] = cornerTL4;
		m_arrayVtxBorder[index++] = cornerTL5;

		// Border Top
		OverlayCategory::Vertex borderT0 = { { float( centerL ) / p_size.getWidth(), float( borderT ) / p_size.getHeight() }, { borderUvML, borderUvTT } };
		OverlayCategory::Vertex borderT1 = { { float( centerL ) / p_size.getWidth(), float( centerT ) / p_size.getHeight() }, { borderUvML, borderUvMT } };
		OverlayCategory::Vertex borderT2 = { { float( centerR ) / p_size.getWidth(), float( centerT ) / p_size.getHeight() }, { borderUvMR, borderUvMT } };
		OverlayCategory::Vertex borderT3 = { { float( centerL ) / p_size.getWidth(), float( borderT ) / p_size.getHeight() }, { borderUvML, borderUvTT } };
		OverlayCategory::Vertex borderT4 = { { float( centerR ) / p_size.getWidth(), float( centerT ) / p_size.getHeight() }, { borderUvMR, borderUvMT } };
		OverlayCategory::Vertex borderT5 = { { float( centerR ) / p_size.getWidth(), float( borderT ) / p_size.getHeight() }, { borderUvMR, borderUvTT } };
		m_arrayVtxBorder[index++] = borderT0;
		m_arrayVtxBorder[index++] = borderT1;
		m_arrayVtxBorder[index++] = borderT2;
		m_arrayVtxBorder[index++] = borderT3;
		m_arrayVtxBorder[index++] = borderT4;
		m_arrayVtxBorder[index++] = borderT5;

		// Corner Top Right
		OverlayCategory::Vertex cornerTR0 = { { float( centerR ) / p_size.getWidth(), float( borderT ) / p_size.getHeight() }, { borderUvMR, borderUvTT } };
		OverlayCategory::Vertex cornerTR1 = { { float( centerR ) / p_size.getWidth(), float( centerT ) / p_size.getHeight() }, { borderUvMR, borderUvMT } };
		OverlayCategory::Vertex cornerTR2 = { { float( borderR ) / p_size.getWidth(), float( centerT ) / p_size.getHeight() }, { borderUvRR, borderUvMT } };
		OverlayCategory::Vertex cornerTR3 = { { float( centerR ) / p_size.getWidth(), float( borderT ) / p_size.getHeight() }, { borderUvMR, borderUvTT } };
		OverlayCategory::Vertex cornerTR4 = { { float( borderR ) / p_size.getWidth(), float( centerT ) / p_size.getHeight() }, { borderUvRR, borderUvMT } };
		OverlayCategory::Vertex cornerTR5 = { { float( borderR ) / p_size.getWidth(), float( borderT ) / p_size.getHeight() }, { borderUvRR, borderUvTT } };
		m_arrayVtxBorder[index++] = cornerTR0;
		m_arrayVtxBorder[index++] = cornerTR1;
		m_arrayVtxBorder[index++] = cornerTR2;
		m_arrayVtxBorder[index++] = cornerTR3;
		m_arrayVtxBorder[index++] = cornerTR4;
		m_arrayVtxBorder[index++] = cornerTR5;

		// Border Left
		OverlayCategory::Vertex borderL0 = { { float( borderL ) / p_size.getWidth(), float( centerT ) / p_size.getHeight() }, { borderUvLL, borderUvMT } };
		OverlayCategory::Vertex borderL1 = { { float( borderL ) / p_size.getWidth(), float( centerB ) / p_size.getHeight() }, { borderUvLL, borderUvMB } };
		OverlayCategory::Vertex borderL2 = { { float( centerL ) / p_size.getWidth(), float( centerB ) / p_size.getHeight() }, { borderUvML, borderUvMB } };
		OverlayCategory::Vertex borderL3 = { { float( borderL ) / p_size.getWidth(), float( centerT ) / p_size.getHeight() }, { borderUvLL, borderUvMT } };
		OverlayCategory::Vertex borderL4 = { { float( centerL ) / p_size.getWidth(), float( centerB ) / p_size.getHeight() }, { borderUvML, borderUvMB } };
		OverlayCategory::Vertex borderL5 = { { float( centerL ) / p_size.getWidth(), float( centerT ) / p_size.getHeight() }, { borderUvML, borderUvMT } };
		m_arrayVtxBorder[index++] = borderL0;
		m_arrayVtxBorder[index++] = borderL1;
		m_arrayVtxBorder[index++] = borderL2;
		m_arrayVtxBorder[index++] = borderL3;
		m_arrayVtxBorder[index++] = borderL4;
		m_arrayVtxBorder[index++] = borderL5;

		// Border Right
		OverlayCategory::Vertex borderR0 = { { float( centerR ) / p_size.getWidth(), float( centerT ) / p_size.getHeight() }, { borderUvMR, borderUvMT } };
		OverlayCategory::Vertex borderR1 = { { float( centerR ) / p_size.getWidth(), float( centerB ) / p_size.getHeight() }, { borderUvMR, borderUvMB } };
		OverlayCategory::Vertex borderR2 = { { float( borderR ) / p_size.getWidth(), float( centerB ) / p_size.getHeight() }, { borderUvRR, borderUvMB } };
		OverlayCategory::Vertex borderR3 = { { float( centerR ) / p_size.getWidth(), float( centerT ) / p_size.getHeight() }, { borderUvMR, borderUvMT } };
		OverlayCategory::Vertex borderR4 = { { float( borderR ) / p_size.getWidth(), float( centerB ) / p_size.getHeight() }, { borderUvRR, borderUvMB } };
		OverlayCategory::Vertex borderR5 = { { float( borderR ) / p_size.getWidth(), float( centerT ) / p_size.getHeight() }, { borderUvRR, borderUvMT } };
		m_arrayVtxBorder[index++] = borderR0;
		m_arrayVtxBorder[index++] = borderR1;
		m_arrayVtxBorder[index++] = borderR2;
		m_arrayVtxBorder[index++] = borderR3;
		m_arrayVtxBorder[index++] = borderR4;
		m_arrayVtxBorder[index++] = borderR5;

		// Corner Bottom Left
		OverlayCategory::Vertex cornerBL0 = { { float( borderL ) / p_size.getWidth(), float( centerB ) / p_size.getHeight() }, { borderUvLL, borderUvMB } };
		OverlayCategory::Vertex cornerBL1 = { { float( borderL ) / p_size.getWidth(), float( borderB ) / p_size.getHeight() }, { borderUvLL, borderUvBB } };
		OverlayCategory::Vertex cornerBL2 = { { float( centerL ) / p_size.getWidth(), float( borderB ) / p_size.getHeight() }, { borderUvML, borderUvBB } };
		OverlayCategory::Vertex cornerBL3 = { { float( borderL ) / p_size.getWidth(), float( centerB ) / p_size.getHeight() }, { borderUvLL, borderUvMB } };
		OverlayCategory::Vertex cornerBL4 = { { float( centerL ) / p_size.getWidth(), float( borderB ) / p_size.getHeight() }, { borderUvML, borderUvBB } };
		OverlayCategory::Vertex cornerBL5 = { { float( centerL ) / p_size.getWidth(), float( centerB ) / p_size.getHeight() }, { borderUvML, borderUvMB } };
		m_arrayVtxBorder[index++] = cornerBL0;
		m_arrayVtxBorder[index++] = cornerBL1;
		m_arrayVtxBorder[index++] = cornerBL2;
		m_arrayVtxBorder[index++] = cornerBL3;
		m_arrayVtxBorder[index++] = cornerBL4;
		m_arrayVtxBorder[index++] = cornerBL5;

		// Border Bottom
		OverlayCategory::Vertex borderB0 = { { float( centerL ) / p_size.getWidth(), float( centerB ) / p_size.getHeight() }, { borderUvML, borderUvMB } };
		OverlayCategory::Vertex borderB1 = { { float( centerL ) / p_size.getWidth(), float( borderB ) / p_size.getHeight() }, { borderUvML, borderUvBB } };
		OverlayCategory::Vertex borderB2 = { { float( centerR ) / p_size.getWidth(), float( borderB ) / p_size.getHeight() }, { borderUvMR, borderUvBB } };
		OverlayCategory::Vertex borderB3 = { { float( centerL ) / p_size.getWidth(), float( centerB ) / p_size.getHeight() }, { borderUvML, borderUvMB } };
		OverlayCategory::Vertex borderB4 = { { float( centerR ) / p_size.getWidth(), float( borderB ) / p_size.getHeight() }, { borderUvMR, borderUvBB } };
		OverlayCategory::Vertex borderB5 = { { float( centerR ) / p_size.getWidth(), float( centerB ) / p_size.getHeight() }, { borderUvMR, borderUvMB } };
		m_arrayVtxBorder[index++] = borderB0;
		m_arrayVtxBorder[index++] = borderB1;
		m_arrayVtxBorder[index++] = borderB2;
		m_arrayVtxBorder[index++] = borderB3;
		m_arrayVtxBorder[index++] = borderB4;
		m_arrayVtxBorder[index++] = borderB5;

		// Corner Bottom Right
		OverlayCategory::Vertex cornerBR0 = { { float( centerR ) / p_size.getWidth(), float( centerB ) / p_size.getHeight() }, { borderUvMR, borderUvMB } };
		OverlayCategory::Vertex cornerBR1 = { { float( centerR ) / p_size.getWidth(), float( borderB ) / p_size.getHeight() }, { borderUvMR, borderUvBB } };
		OverlayCategory::Vertex cornerBR2 = { { float( borderR ) / p_size.getWidth(), float( borderB ) / p_size.getHeight() }, { borderUvRR, borderUvBB } };
		OverlayCategory::Vertex cornerBR3 = { { float( centerR ) / p_size.getWidth(), float( centerB ) / p_size.getHeight() }, { borderUvMR, borderUvMB } };
		OverlayCategory::Vertex cornerBR4 = { { float( borderR ) / p_size.getWidth(), float( borderB ) / p_size.getHeight() }, { borderUvRR, borderUvBB } };
		OverlayCategory::Vertex cornerBR5 = { { float( borderR ) / p_size.getWidth(), float( centerB ) / p_size.getHeight() }, { borderUvRR, borderUvMB } };
		m_arrayVtxBorder[index++] = cornerBR0;
		m_arrayVtxBorder[index++] = cornerBR1;
		m_arrayVtxBorder[index++] = cornerBR2;
		m_arrayVtxBorder[index++] = cornerBR3;
		m_arrayVtxBorder[index++] = cornerBR4;
		m_arrayVtxBorder[index++] = cornerBR5;
	}
}

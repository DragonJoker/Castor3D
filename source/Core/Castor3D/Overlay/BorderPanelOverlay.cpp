#include "Castor3D/Overlay/BorderPanelOverlay.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/OverlayRenderer.hpp"

CU_ImplementCUSmartPtr( castor3d, BorderPanelOverlay )

using namespace castor;

namespace castor3d
{
	BorderPanelOverlay::TextWriter::TextWriter( String const & tabs, BorderPanelOverlay const * category )
		: OverlayCategory::TextWriter{ tabs }
		, m_category{ category }
	{
	}

	bool BorderPanelOverlay::TextWriter::operator()( BorderPanelOverlay const & overlay, TextFile & file )
	{
		log::info << m_tabs << cuT( "Writing BorderPanelOverlay " ) << overlay.getOverlayName() << std::endl;
		bool result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "border_panel_overlay \"" ) + overlay.getOverlay().getName() + cuT( "\"\n" ) ) > 0
						&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		OverlayCategory::TextWriter::checkError( result, "BorderPanelOverlay name" );

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tborder_size " ) ) > 0
					   && Point4d::TextWriter{ String{} }( overlay.getBorderSize(), file )
					   && file.writeText( cuT( "\n" ) ) > 0;
			OverlayCategory::TextWriter::checkError( result, "BorderPanelOverlay borders size" );
		}

		if ( result && overlay.getBorderMaterial() )
		{
			result = file.writeText( m_tabs + cuT( "\tborder_material \"" ) + overlay.getBorderMaterial()->getName() + cuT( "\"\n" ) ) > 0;
			OverlayCategory::TextWriter::checkError( result, "BorderPanelOverlay borders material" );
		}

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

	bool BorderPanelOverlay::TextWriter::writeInto( castor::TextFile & file )
	{
		return ( *this )( *m_category, file );
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

	void BorderPanelOverlay::accept( OverlayVisitor & visitor )const
	{
		visitor.visit( *this );
	}

	void BorderPanelOverlay::setBorderMaterial( MaterialSPtr material )
	{
		m_pBorderMaterial = material;

		if ( material )
		{
			m_strBorderMatName = material->getName();
		}
		else
		{
			m_strBorderMatName = cuEmptyString;
		}
	}

	void BorderPanelOverlay::doUpdateSize( OverlayRenderer const & renderer )
	{
		OverlayCategory::doUpdateSize( renderer );

		if ( isSizeChanged() || isChanged() || renderer.isSizeChanged() )
		{
			OverlaySPtr parent = getOverlay().getParent();
			Size sz = renderer.getSize();
			Point2d totalSize( sz.getWidth(), sz.getHeight() );

			if ( parent )
			{
				Point2d parentSize = parent->getAbsoluteSize();
				totalSize[0] = parentSize[0] * totalSize[0];
				totalSize[1] = parentSize[1] * totalSize[1];
			}

			castor::Rectangle sizes = getBorderPixelSize();
			Point4d ptSizes = getBorderSize();
			bool changed = m_borderChanged;

			if ( sizes.left() )
			{
				changed = changed || ( ptSizes[0] != double( sizes.left() ) / totalSize[0] );
				ptSizes[0] = sizes.left() / totalSize[0];
			}

			if ( sizes.top() )
			{
				changed = changed || ( ptSizes[1] != double(  sizes.top() ) / totalSize[1] );
				ptSizes[1] = sizes.top() / totalSize[1];
			}

			if ( sizes.right() )
			{
				changed = changed || ( ptSizes[2] != double( sizes.right() ) / totalSize[0] );
				ptSizes[2] = sizes.right() / totalSize[0];
			}

			if ( sizes.bottom() )
			{
				changed = changed || ( ptSizes[3] != double( sizes.bottom() ) / totalSize[1] );
				ptSizes[3] = sizes.bottom() / totalSize[1];
			}

			if ( changed )
			{
				setBorderSize( ptSizes );
			}
		}
	}

	castor::Rectangle BorderPanelOverlay::getAbsoluteBorderSize( castor::Size const & size )const
	{
		Point4d absoluteSize = getAbsoluteBorderSize();

		return castor::Rectangle(
				   int32_t( absoluteSize[0] * size.getWidth() ),
				   int32_t( absoluteSize[1] * size.getHeight() ),
				   int32_t( absoluteSize[2] * size.getWidth() ),
				   int32_t( absoluteSize[3] * size.getHeight() )
			   );
	}

	Point4d BorderPanelOverlay::getAbsoluteBorderSize()const
	{
		Point4d absoluteSize = getBorderSize();
		OverlaySPtr parent = getOverlay().getParent();

		if ( parent )
		{
			Point2d parentSize = parent->getAbsoluteSize();
			absoluteSize[0] *= parentSize[0];
			absoluteSize[1] *= parentSize[1];
			absoluteSize[2] *= parentSize[0];
			absoluteSize[3] *= parentSize[1];
		}

		return absoluteSize;
	}

	void BorderPanelOverlay::doUpdateBuffer( Size const & size )
	{
		Size absoluteSize = getAbsoluteSize( size );
		castor::Rectangle absoluteBorderSize = getAbsoluteBorderSize( size );

		int32_t centerL = 0;
		int32_t centerT = 0;
		int32_t centerR = absoluteSize.getWidth();
		int32_t centerB = absoluteSize.getHeight();

		if ( m_borderPosition == BorderPosition::eInternal )
		{
			centerL += absoluteBorderSize.left();
			centerT += absoluteBorderSize.top();
			centerR -= absoluteBorderSize.right();
			centerB -= absoluteBorderSize.bottom();
		}
		else if ( m_borderPosition == BorderPosition::eMiddle )
		{
			centerL += absoluteBorderSize.left() / 2;
			centerT += absoluteBorderSize.top() / 2;
			centerR -= absoluteBorderSize.right() / 2;
			centerB -= absoluteBorderSize.bottom() / 2;
		}

		int32_t borderL = centerL - absoluteBorderSize.left();
		int32_t borderT = centerT - absoluteBorderSize.top();
		int32_t borderR = centerR + absoluteBorderSize.right();
		int32_t borderB = centerB + absoluteBorderSize.bottom();

		auto borderUvLL = float( m_borderOuterUv[0] );
		auto borderUvTT = float( m_borderOuterUv[1] );
		auto borderUvML = float( m_borderInnerUv[0] );
		auto borderUvMT = float( m_borderInnerUv[1] );
		auto borderUvMR = float( m_borderInnerUv[2] );
		auto borderUvMB = float( m_borderInnerUv[3] );
		auto borderUvRR = float( m_borderOuterUv[2] );
		auto borderUvBB = float( m_borderOuterUv[3] );

		// Center
		OverlayCategory::Vertex vertex0 = { Point2f{ float( centerL ) / size.getWidth(), float( centerT ) / size.getHeight() }, Point2f{ float( m_uv[0] ), float( m_uv[3] ) } };
		OverlayCategory::Vertex vertex1 = { Point2f{ float( centerL ) / size.getWidth(), float( centerB ) / size.getHeight() }, Point2f{ float( m_uv[0] ), float( m_uv[1] ) } };
		OverlayCategory::Vertex vertex2 = { Point2f{ float( centerR ) / size.getWidth(), float( centerB ) / size.getHeight() }, Point2f{ float( m_uv[2] ), float( m_uv[1] ) } };
		OverlayCategory::Vertex vertex3 = { Point2f{ float( centerL ) / size.getWidth(), float( centerT ) / size.getHeight() }, Point2f{ float( m_uv[0] ), float( m_uv[3] ) } };
		OverlayCategory::Vertex vertex4 = { Point2f{ float( centerR ) / size.getWidth(), float( centerB ) / size.getHeight() }, Point2f{ float( m_uv[2] ), float( m_uv[1] ) } };
		OverlayCategory::Vertex vertex5 = { Point2f{ float( centerR ) / size.getWidth(), float( centerT ) / size.getHeight() }, Point2f{ float( m_uv[2] ), float( m_uv[3] ) } };
		m_arrayVtx[0] = vertex0;
		m_arrayVtx[1] = vertex1;
		m_arrayVtx[2] = vertex2;
		m_arrayVtx[3] = vertex3;
		m_arrayVtx[4] = vertex4;
		m_arrayVtx[5] = vertex5;

		// Corner Top Left
		uint32_t index = 0;
		OverlayCategory::Vertex cornerTL0 = { Point2f{ float( borderL ) / size.getWidth(), float( borderT ) / size.getHeight() }, Point2f{ borderUvLL, borderUvTT } };
		OverlayCategory::Vertex cornerTL1 = { Point2f{ float( borderL ) / size.getWidth(), float( centerT ) / size.getHeight() }, Point2f{ borderUvLL, borderUvMT } };
		OverlayCategory::Vertex cornerTL2 = { Point2f{ float( centerL ) / size.getWidth(), float( centerT ) / size.getHeight() }, Point2f{ borderUvML, borderUvMT } };
		OverlayCategory::Vertex cornerTL3 = { Point2f{ float( borderL ) / size.getWidth(), float( borderT ) / size.getHeight() }, Point2f{ borderUvLL, borderUvTT } };
		OverlayCategory::Vertex cornerTL4 = { Point2f{ float( centerL ) / size.getWidth(), float( centerT ) / size.getHeight() }, Point2f{ borderUvML, borderUvMT } };
		OverlayCategory::Vertex cornerTL5 = { Point2f{ float( centerL ) / size.getWidth(), float( borderT ) / size.getHeight() }, Point2f{ borderUvML, borderUvTT } };
		m_arrayVtxBorder[index++] = cornerTL0;
		m_arrayVtxBorder[index++] = cornerTL1;
		m_arrayVtxBorder[index++] = cornerTL2;
		m_arrayVtxBorder[index++] = cornerTL3;
		m_arrayVtxBorder[index++] = cornerTL4;
		m_arrayVtxBorder[index++] = cornerTL5;

		// Border Top
		OverlayCategory::Vertex borderT0 = { Point2f{ float( centerL ) / size.getWidth(), float( borderT ) / size.getHeight() }, Point2f{ borderUvML, borderUvTT } };
		OverlayCategory::Vertex borderT1 = { Point2f{ float( centerL ) / size.getWidth(), float( centerT ) / size.getHeight() }, Point2f{ borderUvML, borderUvMT } };
		OverlayCategory::Vertex borderT2 = { Point2f{ float( centerR ) / size.getWidth(), float( centerT ) / size.getHeight() }, Point2f{ borderUvMR, borderUvMT } };
		OverlayCategory::Vertex borderT3 = { Point2f{ float( centerL ) / size.getWidth(), float( borderT ) / size.getHeight() }, Point2f{ borderUvML, borderUvTT } };
		OverlayCategory::Vertex borderT4 = { Point2f{ float( centerR ) / size.getWidth(), float( centerT ) / size.getHeight() }, Point2f{ borderUvMR, borderUvMT } };
		OverlayCategory::Vertex borderT5 = { Point2f{ float( centerR ) / size.getWidth(), float( borderT ) / size.getHeight() }, Point2f{ borderUvMR, borderUvTT } };
		m_arrayVtxBorder[index++] = borderT0;
		m_arrayVtxBorder[index++] = borderT1;
		m_arrayVtxBorder[index++] = borderT2;
		m_arrayVtxBorder[index++] = borderT3;
		m_arrayVtxBorder[index++] = borderT4;
		m_arrayVtxBorder[index++] = borderT5;

		// Corner Top Right
		OverlayCategory::Vertex cornerTR0 = { Point2f{ float( centerR ) / size.getWidth(), float( borderT ) / size.getHeight() }, Point2f{ borderUvMR, borderUvTT } };
		OverlayCategory::Vertex cornerTR1 = { Point2f{ float( centerR ) / size.getWidth(), float( centerT ) / size.getHeight() }, Point2f{ borderUvMR, borderUvMT } };
		OverlayCategory::Vertex cornerTR2 = { Point2f{ float( borderR ) / size.getWidth(), float( centerT ) / size.getHeight() }, Point2f{ borderUvRR, borderUvMT } };
		OverlayCategory::Vertex cornerTR3 = { Point2f{ float( centerR ) / size.getWidth(), float( borderT ) / size.getHeight() }, Point2f{ borderUvMR, borderUvTT } };
		OverlayCategory::Vertex cornerTR4 = { Point2f{ float( borderR ) / size.getWidth(), float( centerT ) / size.getHeight() }, Point2f{ borderUvRR, borderUvMT } };
		OverlayCategory::Vertex cornerTR5 = { Point2f{ float( borderR ) / size.getWidth(), float( borderT ) / size.getHeight() }, Point2f{ borderUvRR, borderUvTT } };
		m_arrayVtxBorder[index++] = cornerTR0;
		m_arrayVtxBorder[index++] = cornerTR1;
		m_arrayVtxBorder[index++] = cornerTR2;
		m_arrayVtxBorder[index++] = cornerTR3;
		m_arrayVtxBorder[index++] = cornerTR4;
		m_arrayVtxBorder[index++] = cornerTR5;

		// Border Left
		OverlayCategory::Vertex borderL0 = { Point2f{ float( borderL ) / size.getWidth(), float( centerT ) / size.getHeight() }, Point2f{ borderUvLL, borderUvMT } };
		OverlayCategory::Vertex borderL1 = { Point2f{ float( borderL ) / size.getWidth(), float( centerB ) / size.getHeight() }, Point2f{ borderUvLL, borderUvMB } };
		OverlayCategory::Vertex borderL2 = { Point2f{ float( centerL ) / size.getWidth(), float( centerB ) / size.getHeight() }, Point2f{ borderUvML, borderUvMB } };
		OverlayCategory::Vertex borderL3 = { Point2f{ float( borderL ) / size.getWidth(), float( centerT ) / size.getHeight() }, Point2f{ borderUvLL, borderUvMT } };
		OverlayCategory::Vertex borderL4 = { Point2f{ float( centerL ) / size.getWidth(), float( centerB ) / size.getHeight() }, Point2f{ borderUvML, borderUvMB } };
		OverlayCategory::Vertex borderL5 = { Point2f{ float( centerL ) / size.getWidth(), float( centerT ) / size.getHeight() }, Point2f{ borderUvML, borderUvMT } };
		m_arrayVtxBorder[index++] = borderL0;
		m_arrayVtxBorder[index++] = borderL1;
		m_arrayVtxBorder[index++] = borderL2;
		m_arrayVtxBorder[index++] = borderL3;
		m_arrayVtxBorder[index++] = borderL4;
		m_arrayVtxBorder[index++] = borderL5;

		// Border Right
		OverlayCategory::Vertex borderR0 = { Point2f{ float( centerR ) / size.getWidth(), float( centerT ) / size.getHeight() }, Point2f{ borderUvMR, borderUvMT } };
		OverlayCategory::Vertex borderR1 = { Point2f{ float( centerR ) / size.getWidth(), float( centerB ) / size.getHeight() }, Point2f{ borderUvMR, borderUvMB } };
		OverlayCategory::Vertex borderR2 = { Point2f{ float( borderR ) / size.getWidth(), float( centerB ) / size.getHeight() }, Point2f{ borderUvRR, borderUvMB } };
		OverlayCategory::Vertex borderR3 = { Point2f{ float( centerR ) / size.getWidth(), float( centerT ) / size.getHeight() }, Point2f{ borderUvMR, borderUvMT } };
		OverlayCategory::Vertex borderR4 = { Point2f{ float( borderR ) / size.getWidth(), float( centerB ) / size.getHeight() }, Point2f{ borderUvRR, borderUvMB } };
		OverlayCategory::Vertex borderR5 = { Point2f{ float( borderR ) / size.getWidth(), float( centerT ) / size.getHeight() }, Point2f{ borderUvRR, borderUvMT } };
		m_arrayVtxBorder[index++] = borderR0;
		m_arrayVtxBorder[index++] = borderR1;
		m_arrayVtxBorder[index++] = borderR2;
		m_arrayVtxBorder[index++] = borderR3;
		m_arrayVtxBorder[index++] = borderR4;
		m_arrayVtxBorder[index++] = borderR5;

		// Corner Bottom Left
		OverlayCategory::Vertex cornerBL0 = { Point2f{ float( borderL ) / size.getWidth(), float( centerB ) / size.getHeight() }, Point2f{ borderUvLL, borderUvMB } };
		OverlayCategory::Vertex cornerBL1 = { Point2f{ float( borderL ) / size.getWidth(), float( borderB ) / size.getHeight() }, Point2f{ borderUvLL, borderUvBB } };
		OverlayCategory::Vertex cornerBL2 = { Point2f{ float( centerL ) / size.getWidth(), float( borderB ) / size.getHeight() }, Point2f{ borderUvML, borderUvBB } };
		OverlayCategory::Vertex cornerBL3 = { Point2f{ float( borderL ) / size.getWidth(), float( centerB ) / size.getHeight() }, Point2f{ borderUvLL, borderUvMB } };
		OverlayCategory::Vertex cornerBL4 = { Point2f{ float( centerL ) / size.getWidth(), float( borderB ) / size.getHeight() }, Point2f{ borderUvML, borderUvBB } };
		OverlayCategory::Vertex cornerBL5 = { Point2f{ float( centerL ) / size.getWidth(), float( centerB ) / size.getHeight() }, Point2f{ borderUvML, borderUvMB } };
		m_arrayVtxBorder[index++] = cornerBL0;
		m_arrayVtxBorder[index++] = cornerBL1;
		m_arrayVtxBorder[index++] = cornerBL2;
		m_arrayVtxBorder[index++] = cornerBL3;
		m_arrayVtxBorder[index++] = cornerBL4;
		m_arrayVtxBorder[index++] = cornerBL5;

		// Border Bottom
		OverlayCategory::Vertex borderB0 = { Point2f{ float( centerL ) / size.getWidth(), float( centerB ) / size.getHeight() }, Point2f{ borderUvML, borderUvMB } };
		OverlayCategory::Vertex borderB1 = { Point2f{ float( centerL ) / size.getWidth(), float( borderB ) / size.getHeight() }, Point2f{ borderUvML, borderUvBB } };
		OverlayCategory::Vertex borderB2 = { Point2f{ float( centerR ) / size.getWidth(), float( borderB ) / size.getHeight() }, Point2f{ borderUvMR, borderUvBB } };
		OverlayCategory::Vertex borderB3 = { Point2f{ float( centerL ) / size.getWidth(), float( centerB ) / size.getHeight() }, Point2f{ borderUvML, borderUvMB } };
		OverlayCategory::Vertex borderB4 = { Point2f{ float( centerR ) / size.getWidth(), float( borderB ) / size.getHeight() }, Point2f{ borderUvMR, borderUvBB } };
		OverlayCategory::Vertex borderB5 = { Point2f{ float( centerR ) / size.getWidth(), float( centerB ) / size.getHeight() }, Point2f{ borderUvMR, borderUvMB } };
		m_arrayVtxBorder[index++] = borderB0;
		m_arrayVtxBorder[index++] = borderB1;
		m_arrayVtxBorder[index++] = borderB2;
		m_arrayVtxBorder[index++] = borderB3;
		m_arrayVtxBorder[index++] = borderB4;
		m_arrayVtxBorder[index++] = borderB5;

		// Corner Bottom Right
		OverlayCategory::Vertex cornerBR0 = { Point2f{ float( centerR ) / size.getWidth(), float( centerB ) / size.getHeight() }, Point2f{ borderUvMR, borderUvMB } };
		OverlayCategory::Vertex cornerBR1 = { Point2f{ float( centerR ) / size.getWidth(), float( borderB ) / size.getHeight() }, Point2f{ borderUvMR, borderUvBB } };
		OverlayCategory::Vertex cornerBR2 = { Point2f{ float( borderR ) / size.getWidth(), float( borderB ) / size.getHeight() }, Point2f{ borderUvRR, borderUvBB } };
		OverlayCategory::Vertex cornerBR3 = { Point2f{ float( centerR ) / size.getWidth(), float( centerB ) / size.getHeight() }, Point2f{ borderUvMR, borderUvMB } };
		OverlayCategory::Vertex cornerBR4 = { Point2f{ float( borderR ) / size.getWidth(), float( borderB ) / size.getHeight() }, Point2f{ borderUvRR, borderUvBB } };
		OverlayCategory::Vertex cornerBR5 = { Point2f{ float( borderR ) / size.getWidth(), float( centerB ) / size.getHeight() }, Point2f{ borderUvRR, borderUvMB } };
		m_arrayVtxBorder[index++] = cornerBR0;
		m_arrayVtxBorder[index++] = cornerBR1;
		m_arrayVtxBorder[index++] = cornerBR2;
		m_arrayVtxBorder[index++] = cornerBR3;
		m_arrayVtxBorder[index++] = cornerBR4;
		m_arrayVtxBorder[index++] = cornerBR5;
	}
}

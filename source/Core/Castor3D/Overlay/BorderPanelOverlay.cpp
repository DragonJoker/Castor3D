#include "Castor3D/Overlay/BorderPanelOverlay.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Render/Overlays/OverlayRenderer.hpp"

CU_ImplementCUSmartPtr( castor3d, BorderPanelOverlay )

namespace castor3d
{
	BorderPanelOverlay::BorderPanelOverlay()
		: OverlayCategory( OverlayType::eBorderPanel )
		, m_borderOuterUv( 0, 0, 1, 1 )
		, m_borderInnerUv( 0.33, 0.33, 0.66, 0.66 )
		, m_borderChanged( true )
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

	uint32_t BorderPanelOverlay::fillBuffer( castor::Size const & renderSize
		, Vertex * buffer
		, bool secondary )const
	{
		double w = double( std::max( 1u, renderSize.getWidth() ) );
		double h = double( std::max( 1u, renderSize.getHeight() ) );
		auto ratio = getRenderRatio( renderSize );
		w *= ratio->x;
		h *= ratio->y;
		castor::Size absoluteSize = getAbsoluteSize( renderSize );
		castor::Rectangle absoluteBorderSize = getAbsoluteBorderSize( renderSize );

		double centerL = 0;
		double centerT = 0;
		auto centerR = double( absoluteSize.getWidth() );
		auto centerB = double( absoluteSize.getHeight() );

		if ( m_borderPosition == BorderPosition::eInternal )
		{
			centerL += double( absoluteBorderSize.left() );
			centerT += double( absoluteBorderSize.top() );
			centerR -= double( absoluteBorderSize.right() );
			centerB -= double( absoluteBorderSize.bottom() );
		}
		else if ( m_borderPosition == BorderPosition::eMiddle )
		{
			centerL += double( absoluteBorderSize.left() / 2 );
			centerT += double( absoluteBorderSize.top() / 2 );
			centerR -= double( absoluteBorderSize.right() / 2 );
			centerB -= double( absoluteBorderSize.bottom() / 2 );
		}

		if ( !secondary )
		{
			uint32_t index = 0;

			// Center
			OverlayCategory::Vertex vertex0{ castor::Point2f{ centerL / w, centerT / h }, castor::Point2f{ m_uv[0], m_uv[3] } };
			OverlayCategory::Vertex vertex1{ castor::Point2f{ centerL / w, centerB / h }, castor::Point2f{ m_uv[0], m_uv[1] } };
			OverlayCategory::Vertex vertex2{ castor::Point2f{ centerR / w, centerB / h }, castor::Point2f{ m_uv[2], m_uv[1] } };
			OverlayCategory::Vertex vertex3{ castor::Point2f{ centerL / w, centerT / h }, castor::Point2f{ m_uv[0], m_uv[3] } };
			OverlayCategory::Vertex vertex4{ castor::Point2f{ centerR / w, centerB / h }, castor::Point2f{ m_uv[2], m_uv[1] } };
			OverlayCategory::Vertex vertex5{ castor::Point2f{ centerR / w, centerT / h }, castor::Point2f{ m_uv[2], m_uv[3] } };
			buffer[index++] = vertex0;
			buffer[index++] = vertex1;
			buffer[index++] = vertex2;
			buffer[index++] = vertex3;
			buffer[index++] = vertex4;
			buffer[index++] = vertex5;

			return index;
		}

		uint32_t index = 0;
		double borderL = centerL - absoluteBorderSize.left();
		double borderT = centerT - absoluteBorderSize.top();
		double borderR = centerR + absoluteBorderSize.right();
		double borderB = centerB + absoluteBorderSize.bottom();

		auto borderUvLL = double( m_borderOuterUv[0] );
		auto borderUvTT = double( m_borderOuterUv[1] );
		auto borderUvML = double( m_borderInnerUv[0] );
		auto borderUvMT = double( m_borderInnerUv[1] );
		auto borderUvMR = double( m_borderInnerUv[2] );
		auto borderUvMB = double( m_borderInnerUv[3] );
		auto borderUvRR = double( m_borderOuterUv[2] );
		auto borderUvBB = double( m_borderOuterUv[3] );

		// Corner Top Left
		OverlayCategory::Vertex cornerTL0{ castor::Point2f{ borderL / w, borderT / h }, castor::Point2f{ borderUvLL, borderUvTT } };
		OverlayCategory::Vertex cornerTL1{ castor::Point2f{ borderL / w, centerT / h }, castor::Point2f{ borderUvLL, borderUvMT } };
		OverlayCategory::Vertex cornerTL2{ castor::Point2f{ centerL / w, centerT / h }, castor::Point2f{ borderUvML, borderUvMT } };
		OverlayCategory::Vertex cornerTL3{ castor::Point2f{ borderL / w, borderT / h }, castor::Point2f{ borderUvLL, borderUvTT } };
		OverlayCategory::Vertex cornerTL4{ castor::Point2f{ centerL / w, centerT / h }, castor::Point2f{ borderUvML, borderUvMT } };
		OverlayCategory::Vertex cornerTL5{ castor::Point2f{ centerL / w, borderT / h }, castor::Point2f{ borderUvML, borderUvTT } };
		buffer[index++] = cornerTL0;
		buffer[index++] = cornerTL1;
		buffer[index++] = cornerTL2;
		buffer[index++] = cornerTL3;
		buffer[index++] = cornerTL4;
		buffer[index++] = cornerTL5;

		// Border Top
		OverlayCategory::Vertex borderT0{ castor::Point2f{ centerL / w, borderT / h }, castor::Point2f{ borderUvML, borderUvTT } };
		OverlayCategory::Vertex borderT1{ castor::Point2f{ centerL / w, centerT / h }, castor::Point2f{ borderUvML, borderUvMT } };
		OverlayCategory::Vertex borderT2{ castor::Point2f{ centerR / w, centerT / h }, castor::Point2f{ borderUvMR, borderUvMT } };
		OverlayCategory::Vertex borderT3{ castor::Point2f{ centerL / w, borderT / h }, castor::Point2f{ borderUvML, borderUvTT } };
		OverlayCategory::Vertex borderT4{ castor::Point2f{ centerR / w, centerT / h }, castor::Point2f{ borderUvMR, borderUvMT } };
		OverlayCategory::Vertex borderT5{ castor::Point2f{ centerR / w, borderT / h }, castor::Point2f{ borderUvMR, borderUvTT } };
		buffer[index++] = borderT0;
		buffer[index++] = borderT1;
		buffer[index++] = borderT2;
		buffer[index++] = borderT3;
		buffer[index++] = borderT4;
		buffer[index++] = borderT5;

		// Corner Top Right
		OverlayCategory::Vertex cornerTR0{ castor::Point2f{ centerR / w, borderT / h }, castor::Point2f{ borderUvMR, borderUvTT } };
		OverlayCategory::Vertex cornerTR1{ castor::Point2f{ centerR / w, centerT / h }, castor::Point2f{ borderUvMR, borderUvMT } };
		OverlayCategory::Vertex cornerTR2{ castor::Point2f{ borderR / w, centerT / h }, castor::Point2f{ borderUvRR, borderUvMT } };
		OverlayCategory::Vertex cornerTR3{ castor::Point2f{ centerR / w, borderT / h }, castor::Point2f{ borderUvMR, borderUvTT } };
		OverlayCategory::Vertex cornerTR4{ castor::Point2f{ borderR / w, centerT / h }, castor::Point2f{ borderUvRR, borderUvMT } };
		OverlayCategory::Vertex cornerTR5{ castor::Point2f{ borderR / w, borderT / h }, castor::Point2f{ borderUvRR, borderUvTT } };
		buffer[index++] = cornerTR0;
		buffer[index++] = cornerTR1;
		buffer[index++] = cornerTR2;
		buffer[index++] = cornerTR3;
		buffer[index++] = cornerTR4;
		buffer[index++] = cornerTR5;

		// Border Left
		OverlayCategory::Vertex borderL0{ castor::Point2f{ borderL / w, centerT / h }, castor::Point2f{ borderUvLL, borderUvMT } };
		OverlayCategory::Vertex borderL1{ castor::Point2f{ borderL / w, centerB / h }, castor::Point2f{ borderUvLL, borderUvMB } };
		OverlayCategory::Vertex borderL2{ castor::Point2f{ centerL / w, centerB / h }, castor::Point2f{ borderUvML, borderUvMB } };
		OverlayCategory::Vertex borderL3{ castor::Point2f{ borderL / w, centerT / h }, castor::Point2f{ borderUvLL, borderUvMT } };
		OverlayCategory::Vertex borderL4{ castor::Point2f{ centerL / w, centerB / h }, castor::Point2f{ borderUvML, borderUvMB } };
		OverlayCategory::Vertex borderL5{ castor::Point2f{ centerL / w, centerT / h }, castor::Point2f{ borderUvML, borderUvMT } };
		buffer[index++] = borderL0;
		buffer[index++] = borderL1;
		buffer[index++] = borderL2;
		buffer[index++] = borderL3;
		buffer[index++] = borderL4;
		buffer[index++] = borderL5;

		// Border Right
		OverlayCategory::Vertex borderR0{ castor::Point2f{ centerR / w, centerT / h }, castor::Point2f{ borderUvMR, borderUvMT } };
		OverlayCategory::Vertex borderR1{ castor::Point2f{ centerR / w, centerB / h }, castor::Point2f{ borderUvMR, borderUvMB } };
		OverlayCategory::Vertex borderR2{ castor::Point2f{ borderR / w, centerB / h }, castor::Point2f{ borderUvRR, borderUvMB } };
		OverlayCategory::Vertex borderR3{ castor::Point2f{ centerR / w, centerT / h }, castor::Point2f{ borderUvMR, borderUvMT } };
		OverlayCategory::Vertex borderR4{ castor::Point2f{ borderR / w, centerB / h }, castor::Point2f{ borderUvRR, borderUvMB } };
		OverlayCategory::Vertex borderR5{ castor::Point2f{ borderR / w, centerT / h }, castor::Point2f{ borderUvRR, borderUvMT } };
		buffer[index++] = borderR0;
		buffer[index++] = borderR1;
		buffer[index++] = borderR2;
		buffer[index++] = borderR3;
		buffer[index++] = borderR4;
		buffer[index++] = borderR5;

		// Corner Bottom Left
		OverlayCategory::Vertex cornerBL0{ castor::Point2f{ borderL / w, centerB / h }, castor::Point2f{ borderUvLL, borderUvMB } };
		OverlayCategory::Vertex cornerBL1{ castor::Point2f{ borderL / w, borderB / h }, castor::Point2f{ borderUvLL, borderUvBB } };
		OverlayCategory::Vertex cornerBL2{ castor::Point2f{ centerL / w, borderB / h }, castor::Point2f{ borderUvML, borderUvBB } };
		OverlayCategory::Vertex cornerBL3{ castor::Point2f{ borderL / w, centerB / h }, castor::Point2f{ borderUvLL, borderUvMB } };
		OverlayCategory::Vertex cornerBL4{ castor::Point2f{ centerL / w, borderB / h }, castor::Point2f{ borderUvML, borderUvBB } };
		OverlayCategory::Vertex cornerBL5{ castor::Point2f{ centerL / w, centerB / h }, castor::Point2f{ borderUvML, borderUvMB } };
		buffer[index++] = cornerBL0;
		buffer[index++] = cornerBL1;
		buffer[index++] = cornerBL2;
		buffer[index++] = cornerBL3;
		buffer[index++] = cornerBL4;
		buffer[index++] = cornerBL5;

		// Border Bottom
		OverlayCategory::Vertex borderB0{ castor::Point2f{ centerL / w, centerB / h }, castor::Point2f{ borderUvML, borderUvMB } };
		OverlayCategory::Vertex borderB1{ castor::Point2f{ centerL / w, borderB / h }, castor::Point2f{ borderUvML, borderUvBB } };
		OverlayCategory::Vertex borderB2{ castor::Point2f{ centerR / w, borderB / h }, castor::Point2f{ borderUvMR, borderUvBB } };
		OverlayCategory::Vertex borderB3{ castor::Point2f{ centerL / w, centerB / h }, castor::Point2f{ borderUvML, borderUvMB } };
		OverlayCategory::Vertex borderB4{ castor::Point2f{ centerR / w, borderB / h }, castor::Point2f{ borderUvMR, borderUvBB } };
		OverlayCategory::Vertex borderB5{ castor::Point2f{ centerR / w, centerB / h }, castor::Point2f{ borderUvMR, borderUvMB } };
		buffer[index++] = borderB0;
		buffer[index++] = borderB1;
		buffer[index++] = borderB2;
		buffer[index++] = borderB3;
		buffer[index++] = borderB4;
		buffer[index++] = borderB5;

		// Corner Bottom Right
		OverlayCategory::Vertex cornerBR0{ castor::Point2f{ centerR / w, centerB / h }, castor::Point2f{ borderUvMR, borderUvMB } };
		OverlayCategory::Vertex cornerBR1{ castor::Point2f{ centerR / w, borderB / h }, castor::Point2f{ borderUvMR, borderUvBB } };
		OverlayCategory::Vertex cornerBR2{ castor::Point2f{ borderR / w, borderB / h }, castor::Point2f{ borderUvRR, borderUvBB } };
		OverlayCategory::Vertex cornerBR3{ castor::Point2f{ centerR / w, centerB / h }, castor::Point2f{ borderUvMR, borderUvMB } };
		OverlayCategory::Vertex cornerBR4{ castor::Point2f{ borderR / w, borderB / h }, castor::Point2f{ borderUvRR, borderUvBB } };
		OverlayCategory::Vertex cornerBR5{ castor::Point2f{ borderR / w, centerB / h }, castor::Point2f{ borderUvRR, borderUvMB } };
		buffer[index++] = cornerBR0;
		buffer[index++] = cornerBR1;
		buffer[index++] = cornerBR2;
		buffer[index++] = cornerBR3;
		buffer[index++] = cornerBR4;
		buffer[index++] = cornerBR5;

		return index;
	}

	void BorderPanelOverlay::setBorderMaterial( MaterialRPtr material )
	{
		m_pBorderMaterial = material;
	}

	void BorderPanelOverlay::doUpdateSize( OverlayRenderer const & renderer )
	{
		OverlayCategory::doUpdateSize( renderer );

		if ( isSizeChanged() || isChanged() || renderer.isSizeChanged() )
		{
			auto parent = getOverlay().getParent();
			auto sz = renderer.getSize();
			castor::Point2d totalSize( sz.getWidth(), sz.getHeight() );

			if ( parent )
			{
				auto parentSize = parent->getAbsoluteSize();
				totalSize[0] = parentSize[0] * totalSize[0];
				totalSize[1] = parentSize[1] * totalSize[1];
			}

			auto sizes = getBorderPixelSize();
			auto ptSizes = getBorderSize();
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
		auto absoluteSize = getAbsoluteBorderSize();

		return castor::Rectangle(
				   int32_t( absoluteSize[0] * size.getWidth() ),
				   int32_t( absoluteSize[1] * size.getHeight() ),
				   int32_t( absoluteSize[2] * size.getWidth() ),
				   int32_t( absoluteSize[3] * size.getHeight() )
			   );
	}

	castor::Point4d BorderPanelOverlay::getAbsoluteBorderSize()const
	{
		castor::Point4d absoluteSize = getBorderSize();
		auto parent = getOverlay().getParent();

		if ( parent )
		{
			castor::Point2d parentSize = parent->getAbsoluteSize();
			absoluteSize[0] *= parentSize[0];
			absoluteSize[1] *= parentSize[1];
			absoluteSize[2] *= parentSize[0];
			absoluteSize[3] *= parentSize[1];
		}

		return absoluteSize;
	}

	void BorderPanelOverlay::doUpdate( OverlayRenderer const & renderer )
	{
		m_borderChanged = false;
	}
}

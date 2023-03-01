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
		if ( secondary )
		{
			return 48u;
		}

		return 6u;
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

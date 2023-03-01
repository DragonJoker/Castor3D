#include "Castor3D/Overlay/OverlayCategory.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Render/Overlays/OverlayRenderer.hpp"

CU_ImplementCUSmartPtr( castor3d, OverlayCategory )

namespace castor3d
{
	OverlayCategory::OverlayCategory( OverlayType type )
		: m_type( type )
	{
	}

	OverlayCategory::~OverlayCategory()
	{
	}

	void OverlayCategory::update( OverlayRenderer const & renderer )
	{
		if ( getOverlay().isVisible() )
		{
			if ( isPositionChanged() || renderer.isSizeChanged() )
			{
				doUpdatePosition( renderer );
			}

			if ( isSizeChanged() || renderer.isSizeChanged() )
			{
				doUpdateSize( renderer );
			}

			if ( isChanged() || isSizeChanged() || renderer.isSizeChanged() )
			{
				doUpdate( renderer );
			}

			m_computeSize = renderer.getSize();
			m_positionChanged = false;
			m_sizeChanged = false;
		}
	}

	void OverlayCategory::setMaterial( MaterialRPtr material )
	{
		m_material = material;
	}

	castor::String const & OverlayCategory::getOverlayName()const
	{
		return m_overlay->getName();
	}

	castor::Position OverlayCategory::getAbsolutePosition( castor::Size const & size )const
	{
		// TODO: Bug here
		castor::Point2d position = getAbsolutePosition();
		return castor::Position{ int32_t( size.getWidth() * position[0] )
			, int32_t( size.getHeight() * position[1] ) };
	}

	castor::Size OverlayCategory::getAbsoluteSize( castor::Size const & size )const
	{
		castor::Point2d absoluteSize = getAbsoluteSize();
		return castor::Size{ uint32_t( size.getWidth() * absoluteSize[0] )
			, uint32_t( size.getHeight() * absoluteSize[1] ) };
	}

	castor::Point2d OverlayCategory::getRenderRatio( castor::Size const & size )const
	{
		castor::Point2d result{ 1, 1 };

		if ( m_computeSize.getWidth() != 0 )
		{
			result[0] = double( m_computeSize.getWidth() ) / double( size.getWidth() );
		}

		if ( m_computeSize.getHeight() != 0 )
		{
			result[1] = double( m_computeSize.getHeight() ) / double( size.getHeight() );
		}

		return result;
	}

	castor::Point2d OverlayCategory::getAbsolutePosition()const
	{
		castor::Point2d position = getRelativePosition();

		if ( auto parent = getOverlay().getParent() )
		{
			position *= parent->getAbsoluteSize();
			position += parent->getAbsolutePosition();
		}

		return position;
	}

	castor::Point2d OverlayCategory::getAbsoluteSize()const
	{
		castor::Point2d size = getRelativeSize();

		if ( auto parent = getOverlay().getParent() )
		{
			size *= parent->getAbsoluteSize();
		}

		return size;
	}

	bool OverlayCategory::isSizeChanged()const
	{
		bool changed = m_sizeChanged;
		auto parent = getOverlay().getParent();

		if ( !changed && parent )
		{
			changed = parent->isSizeChanged();
		}

		return changed;
	}

	bool OverlayCategory::isPositionChanged()const
	{
		bool changed = m_positionChanged;
		auto parent = getOverlay().getParent();

		if ( !changed && parent )
		{
			changed = parent->isPositionChanged();
		}

		return changed;
	}

	castor::Point2d OverlayCategory::doGetTotalSize( OverlayRenderer const & renderer )const
	{
		auto parent = getOverlay().getParent();
		castor::Size renderSize = renderer.getSize();
		castor::Point2d totalSize( renderSize.getWidth(), renderSize.getHeight() );

		if ( parent )
		{
			castor::Point2d parentSize = parent->getAbsoluteSize();
			totalSize[0] = parentSize[0] * totalSize[0];
			totalSize[1] = parentSize[1] * totalSize[1];
		}

		return totalSize;
	}

	void OverlayCategory::doUpdatePosition( OverlayRenderer const & renderer )
	{
		if ( isPositionChanged() || renderer.isSizeChanged() )
		{
			castor::Size renderSize = renderer.getSize();
			castor::Point2d parentSize = doGetTotalSize( renderer );
			bool changed = m_positionChanged;
			castor::Position pxPos = getPixelPosition();
			castor::Point2d relPos = getRelativePosition();

			if ( pxPos.x() )
			{
				auto v = double( pxPos.x() ) / parentSize->x;
				changed = changed || ( relPos->x != v );
				relPos->x = v;
				m_computeSize[0] = renderSize[0];
			}

			if ( pxPos.y() )
			{
				auto v = double( pxPos.y() ) / parentSize->y;
				changed = changed || ( relPos->y != v );
				relPos->y = v;
				m_computeSize[1] = renderSize[1];
			}

			if ( changed )
			{
				setRelativePosition( relPos );
			}
		}
	}

	void OverlayCategory::doUpdateSize( OverlayRenderer const & renderer )
	{
		if ( isSizeChanged() || renderer.isSizeChanged() )
		{
			castor::Size renderSize = renderer.getSize();
			castor::Point2d parentSize = doGetTotalSize( renderer );
			bool changed = m_sizeChanged;
			castor::Size pxSize = getPixelSize();
			castor::Point2d relSize = getRelativeSize();

			if ( pxSize.getWidth() )
			{
				auto v = double( pxSize.getWidth() ) / parentSize->x;
				changed = changed || ( relSize->x != v );
				relSize->x = v;
				m_computeSize[0] = renderSize[0];
			}

			if ( pxSize.getHeight() )
			{
				auto v = double( pxSize.getHeight() ) / parentSize->y;
				changed = changed || ( relSize->y != v );
				relSize->y = v;
				m_computeSize[1] = renderSize[1];
			}

			if ( changed )
			{
				setRelativeSize( relSize );
			}
		}
	}
}

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
			updatePosition( renderer );
			updateSize( renderer );
			updateScissor( renderer );

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

	castor::Size OverlayCategory::computePixelSize()const
	{
		auto result = getPixelSize();
		auto absolute = getAbsoluteSize( m_computeSize );

		if ( result.getWidth() == 0 )
		{
			result[0] = absolute[0];
		}

		if ( result.getHeight() == 0 )
		{
			result[1] = absolute[1];
		}

		return result;
	}

	castor::Position OverlayCategory::computePixelPosition()const
	{
		auto result = getPixelPosition();
		auto absolute = getAbsolutePosition( m_computeSize );

		if ( result.x() == 0 )
		{
			result[0] = absolute[0];
		}

		if ( result.y() == 0 )
		{
			result[1] = absolute[1];
		}

		return result;
	}

	VkRect2D OverlayCategory::computeScissor( castor::Size const & size )const
	{
		return { VkOffset2D{ std::max( 0, int32_t( std::ceil( size.getWidth() * m_scissorOffset[0] ) ) )
				, std::max( 0, int32_t( std::ceil( size.getHeight() * m_scissorOffset[1] ) ) ) }
			, VkExtent2D{ std::max( 1u, uint32_t( std::ceil( size.getWidth() * m_scissorExtent[0] ) ) )
				, std::max( 1u, uint32_t( std::ceil( size.getHeight() * m_scissorExtent[1] ) ) ) } };
	}

	void OverlayCategory::notifyPositionChanged()noexcept
	{
		if ( m_overlay )
		{
			for ( auto child : *m_overlay )
			{
				child->getCategory()->m_positionChanged = true;
			}
		}
	}

	void OverlayCategory::notifySizeChanged()noexcept
	{
		if ( m_overlay )
		{
			for ( auto child : *m_overlay )
			{
				child->getCategory()->m_sizeChanged = true;
			}
		}
	}

	void OverlayCategory::updatePosition( OverlayRenderer const & renderer )
	{
		if ( isPositionChanged() || renderer.isSizeChanged() )
		{
			castor::Size renderSize = renderer.getSize();
			castor::Point2d parentSize = doGetParentSize() * renderSize;
			bool changed = m_positionChanged;
			castor::Position pxPos = getPixelPosition();
			castor::Point2d relPos = getRelativePosition();

			if ( pxPos.x() )
			{
				auto v = double( pxPos.x() ) / parentSize->x;
				changed = changed || ( relPos->x != v );
				relPos->x = v;
				m_computeSize[0] = renderSize->x;
			}

			if ( pxPos.y() )
			{
				auto v = double( pxPos.y() ) / parentSize->y;
				changed = changed || ( relPos->y != v );
				relPos->y = v;
				m_computeSize[1] = renderSize->y;
			}

			if ( changed )
			{
				setRelativePosition( relPos );
			}
		}

		doUpdatePosition( renderer );
	}

	void OverlayCategory::updateSize( OverlayRenderer const & renderer )
	{
		if ( isSizeChanged() || renderer.isSizeChanged() )
		{
			castor::Size renderSize = renderer.getSize();
			castor::Point2d parentSize = doGetParentSize() * renderSize;
			bool changed = m_sizeChanged;
			castor::Size pxSize = getPixelSize();
			castor::Point2d relSize = getRelativeSize();

			if ( pxSize.getWidth() )
			{
				auto v = double( pxSize.getWidth() ) / parentSize->x;
				changed = changed || ( relSize->x != v );
				relSize->x = v;
				m_computeSize[0] = renderSize->x;
			}

			if ( pxSize.getHeight() )
			{
				auto v = double( pxSize.getHeight() ) / parentSize->y;
				changed = changed || ( relSize->y != v );
				relSize->y = v;
				m_computeSize[1] = renderSize->y;
			}

			if ( changed )
			{
				setRelativeSize( relSize );
			}
		}

		doUpdateSize( renderer );
	}

	void OverlayCategory::updateScissor( OverlayRenderer const & renderer )
	{
		if ( isChanged() || isPositionChanged() || isSizeChanged() || renderer.isSizeChanged() )
		{
			m_scissorOffset = getAbsolutePosition();
			m_scissorExtent = getAbsoluteSize();
			doUpdateScissor();
			m_scissorExtent[0] = std::min( m_scissorExtent[0], 1.0 - m_scissorOffset[0] );
			m_scissorExtent[1] = std::min( m_scissorExtent[1], 1.0 - m_scissorOffset[1] );
		}
	}

	castor::Point2d OverlayCategory::doGetParentSize()const
	{
		castor::Point2d result( 1.0, 1.0 );

		if ( auto parent = getOverlay().getParent() )
		{
			result = parent->getAbsoluteSize();
		}

		return result;
	}
}

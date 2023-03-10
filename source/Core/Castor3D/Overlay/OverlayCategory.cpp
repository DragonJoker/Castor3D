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
	namespace ovrlcat
	{
		static castor::Point4d intersect( castor::Point4d const & lhs
			, castor::Point4d const & rhs )
		{
			castor::Point2d start{ std::max( lhs->x, rhs->x ), std::max( lhs->y, rhs->y ) };
			castor::Point2d end{ std::min( lhs->z, rhs->z ), std::min( lhs->w, rhs->w ) };

			if ( ( start->x > lhs->z ) || ( start->y > lhs->w ) )
			{
				return castor::Point4d{};
			}

			return castor::Point4d{ start->x, start->y, end->x, end->y };
		}
	}

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
			updateClientArea();

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
		auto position = getAbsolutePosition();
		return castor::Position{ int32_t( double( size->x ) * position->x )
			, int32_t( double( size->y ) * position->y ) };
	}

	castor::Size OverlayCategory::getAbsoluteSize( castor::Size const & size )const
	{
		if ( m_pxSize )
		{
			return *m_pxSize;
		}

		castor::Point2d absoluteSize = getAbsoluteSize();
		return castor::Size{ uint32_t( size->x * absoluteSize->x )
			, uint32_t( size->y * absoluteSize->y ) };
	}

	castor::Point2d OverlayCategory::getRenderRatio( castor::Size const & size )const
	{
		castor::Point2d result{ 1, 1 };

		if ( m_computeSize->x != 0 )
		{
			result->x = double( m_computeSize->x ) / double( size->x );
		}

		if ( m_computeSize->y != 0 )
		{
			result->y = double( m_computeSize->y ) / double( size->y );
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
		if ( m_pxSize )
		{
			return *m_pxSize;
		}

		auto parentSize = getOverlay().getParent()
			? getOverlay().getParent()->getAbsoluteSize( m_computeSize )
			: m_computeSize;
		return castor::Size{ uint32_t( double( parentSize->x ) * m_relSize->x )
			, uint32_t( double( parentSize->y ) * m_relSize->y ) };
	}

	castor::Position OverlayCategory::computePixelPosition()const
	{
		if ( m_pxPosition )
		{
			return *m_pxPosition;
		}

		auto parentSize = getOverlay().getParent()
			? getOverlay().getParent()->getAbsoluteSize( m_computeSize )
			: m_computeSize;
		return castor::Position{ int32_t( double( parentSize->x ) * m_relPosition->x )
			, int32_t( double( parentSize->y ) * m_relPosition->y ) };
	}

	castor::Point4d OverlayCategory::computeClientArea()const
	{
		castor::Point4d result = m_clientArea;
		doUpdateClientArea( result );
		auto overlay = &getOverlay();

		while ( auto parent = overlay->getParent() )
		{
			result = ovrlcat::intersect( result, parent->getCategory()->getClientArea() );
			overlay = parent;
		}

		return result;
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
		if ( m_pxPosition
			&& ( isPositionChanged() || renderer.isSizeChanged() ) )
		{
			castor::Size renderSize = renderer.getSize();
			castor::Point2d parentSize = getParentSize() * renderSize;
			bool changed = m_positionChanged;
			auto & pxPos = *m_pxPosition;
			castor::Point2d relPos = getRelativePosition();

			if ( pxPos.x() )
			{
				auto v = double( pxPos.x() ) / parentSize->x;
				changed = changed || ( relPos->x != v );
				relPos->x = v;
			}

			if ( pxPos.y() )
			{
				auto v = double( pxPos.y() ) / parentSize->y;
				changed = changed || ( relPos->y != v );
				relPos->y = v;
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
		if ( m_pxSize
			&& ( isSizeChanged() || renderer.isSizeChanged() ) )
		{
			castor::Size renderSize = renderer.getSize();
			castor::Point2d parentSize = getParentSize() * renderSize;
			bool changed = m_sizeChanged;
			auto & pxSize = *m_pxSize;
			castor::Point2d relSize = getRelativeSize();

			if ( pxSize->x )
			{
				auto v = double( pxSize->x ) / parentSize->x;
				changed = changed || ( relSize->x != v );
				relSize->x = v;
			}

			if ( pxSize->y )
			{
				auto v = double( pxSize->y ) / parentSize->y;
				changed = changed || ( relSize->y != v );
				relSize->y = v;
			}

			if ( changed )
			{
				setRelativeSize( relSize );
			}
		}

		doUpdateSize( renderer );
	}

	void OverlayCategory::updateClientArea()
	{
		if ( isPositionChanged() || isSizeChanged() )
		{
			auto pos = getAbsolutePosition();
			auto dim = getAbsoluteSize();
			m_clientArea = { pos->x, pos->y, pos->x + dim->x, pos->y + dim->y };
		}
	}

	castor::Point2d OverlayCategory::getParentSize()const
	{
		castor::Point2d result( 1.0, 1.0 );

		if ( auto parent = getOverlay().getParent() )
		{
			result = parent->getAbsoluteSize();
		}

		return result;
	}
}

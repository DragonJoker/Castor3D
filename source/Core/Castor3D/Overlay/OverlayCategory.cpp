#include "Castor3D/Overlay/OverlayCategory.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/OverlayRenderer.hpp"
#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"

CU_ImplementCUSmartPtr( castor3d, OverlayCategory )

using namespace castor;

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
				doUpdateBuffer( renderer.getSize() );
			}

			m_positionChanged = false;
			m_sizeChanged = false;
		}
	}

	void OverlayCategory::setMaterial( MaterialRPtr material )
	{
		m_pMaterial = material;

		if ( material )
		{
			m_strMatName = material->getName();
		}
		else
		{
			m_strMatName = cuEmptyString;
		}
	}

	String const & OverlayCategory::getOverlayName()const
	{
		return m_pOverlay->getName();
	}

	Position OverlayCategory::getAbsolutePosition( castor::Size const & size )const
	{
		// TODO: Bug here
		Point2d position = getAbsolutePosition();
		return Position{ int32_t( size.getWidth() * position[0] )
			, int32_t( size.getHeight() * position[1] ) };
	}

	Size OverlayCategory::getAbsoluteSize( castor::Size const & size )const
	{
		Point2d absoluteSize = getAbsoluteSize();
		return Size{ uint32_t( size.getWidth() * absoluteSize[0] )
			, uint32_t( size.getHeight() * absoluteSize[1] ) };
	}

	Point2f OverlayCategory::getRenderRatio( Size const & size )const
	{
		Point2f result{ 1, 1 };

		if ( m_computeSize.getWidth() != 0 )
		{
			result[0] = float( m_computeSize.getWidth() ) / float( size.getWidth() );
		}

		if ( m_computeSize.getHeight() != 0 )
		{
			result[1] = float( m_computeSize.getHeight() ) / float( size.getHeight() );
		}

		return result;
	}

	Point2d OverlayCategory::getAbsolutePosition()const
	{
		Point2d position = getPosition();

		if ( auto parent = getOverlay().getParent() )
		{
			position *= parent->getAbsoluteSize();
			position += parent->getAbsolutePosition();
		}

		return position;
	}

	Point2d OverlayCategory::getAbsoluteSize()const
	{
		Point2d size = getSize();

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

	Point2d OverlayCategory::doGetTotalSize( OverlayRenderer const & renderer )const
	{
		auto parent = getOverlay().getParent();
		Size renderSize = renderer.getSize();
		Point2d totalSize( renderSize.getWidth(), renderSize.getHeight() );

		if ( parent )
		{
			Point2d parentSize = parent->getAbsoluteSize();
			totalSize[0] = parentSize[0] * totalSize[0];
			totalSize[1] = parentSize[1] * totalSize[1];
		}

		return totalSize;
	}

	void OverlayCategory::doUpdatePosition( OverlayRenderer const & renderer )
	{
		if ( isPositionChanged() || renderer.isSizeChanged() )
		{
			Size renderSize = renderer.getSize();
			Point2d totalSize = doGetTotalSize( renderer );
			bool changed = m_positionChanged;
			Position pos = getPixelPosition();
			Point2d ptPos = getPosition();

			if ( pos.x() )
			{
				changed = changed || ( ptPos[0] != double( pos.x() ) / totalSize[0] );
				ptPos[0] = pos.x() / totalSize[0];
				m_computeSize[0] = renderSize[0];
			}

			if ( pos.y() )
			{
				changed = changed || ( ptPos[1] != double( pos.y() ) / totalSize[1] );
				ptPos[1] = pos.y() / totalSize[1];
				m_computeSize[1] = renderSize[1];
			}

			if ( changed )
			{
				setPosition( ptPos );
			}
		}
	}

	void OverlayCategory::doUpdateSize( OverlayRenderer const & renderer )
	{
		if ( isSizeChanged() || renderer.isSizeChanged() )
		{
			Size renderSize = renderer.getSize();
			Point2d totalSize = doGetTotalSize( renderer );
			bool changed = m_sizeChanged;
			Size size = getPixelSize();
			Point2d ptSize = getSize();

			if ( size.getWidth() )
			{
				changed = changed || ( ptSize[0] != double( size.getWidth() ) / totalSize[0] );
				ptSize[0] = size.getWidth() / totalSize[0];
				m_computeSize[0] = renderSize[0];
			}

			if ( size.getHeight() )
			{
				changed = changed || ( ptSize[1] != double( size.getHeight() ) / totalSize[1] );
				ptSize[1] = size.getHeight() / totalSize[1];
				m_computeSize[1] = renderSize[1];
			}

			if ( changed )
			{
				setSize( ptSize );
			}
		}
	}
}

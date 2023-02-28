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
			m_strMatName = castor::cuEmptyString;
		}
	}

	castor::String const & OverlayCategory::getOverlayName()const
	{
		return m_pOverlay->getName();
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

	castor::Point2f OverlayCategory::getRenderRatio( castor::Size const & size )const
	{
		castor::Point2f result{ 1, 1 };

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

	castor::Point2d OverlayCategory::getAbsolutePosition()const
	{
		castor::Point2d position = getPosition();

		if ( auto parent = getOverlay().getParent() )
		{
			position *= parent->getAbsoluteSize();
			position += parent->getAbsolutePosition();
		}

		return position;
	}

	castor::Point2d OverlayCategory::getAbsoluteSize()const
	{
		castor::Point2d size = getSize();

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
			castor::Point2d totalSize = doGetTotalSize( renderer );
			bool changed = m_positionChanged;
			castor::Position pos = getPixelPosition();
			castor::Point2d ptPos = getPosition();

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
			castor::Size renderSize = renderer.getSize();
			castor::Point2d totalSize = doGetTotalSize( renderer );
			bool changed = m_sizeChanged;
			castor::Size size = getPixelSize();
			castor::Point2d ptSize = getSize();

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

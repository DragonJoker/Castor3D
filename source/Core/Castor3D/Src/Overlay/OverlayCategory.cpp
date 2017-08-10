#include "OverlayCategory.hpp"

#include "Engine.hpp"

#include "BorderPanelOverlay.hpp"
#include "OverlayRenderer.hpp"
#include "PanelOverlay.hpp"

#include "Material/Material.hpp"

using namespace castor;

namespace castor3d
{
	OverlayCategory::TextWriter::TextWriter( String const & p_tabs )
		: castor::TextWriter< OverlayCategory >{ p_tabs }
	{
	}

	bool OverlayCategory::TextWriter::operator()( OverlayCategory const & p_overlay, TextFile & p_file )
	{
		bool result = p_file.writeText( m_tabs + cuT( "\tposition " ) ) > 0
						&& Point2d::TextWriter{ String{} }( p_overlay.getPosition(), p_file )
						&& p_file.writeText( cuT( "\n" ) ) > 0;
		castor::TextWriter< OverlayCategory >::checkError( result, "OverlayCategory position" );

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\tsize " ) ) > 0
					   && Point2d::TextWriter{ String{} }( p_overlay.getSize(), p_file )
					   && p_file.writeText( cuT( "\n" ) ) > 0;
			castor::TextWriter< OverlayCategory >::checkError( result, "OverlayCategory size" );
		}

		if ( result && p_overlay.getMaterial() )
		{
			result = p_file.writeText( m_tabs + cuT( "\tmaterial \"" ) + p_overlay.getMaterial()->getName() + cuT( "\"\n" ) ) > 0;
			castor::TextWriter< OverlayCategory >::checkError( result, "OverlayCategory material" );
		}

		for ( auto overlay : p_overlay.getOverlay() )
		{
			switch ( overlay->getType() )
			{
			case OverlayType::ePanel:
				result &= PanelOverlay::TextWriter( m_tabs + cuT( "\t" ) )( *overlay->getPanelOverlay(), p_file );
				break;

			case OverlayType::eBorderPanel:
				result &= BorderPanelOverlay::TextWriter( m_tabs + cuT( "\t" ) )( *overlay->getBorderPanelOverlay(), p_file );
				break;

			case OverlayType::eText:
				result &= TextOverlay::TextWriter( m_tabs + cuT( "\t" ) )( *overlay->getTextOverlay(), p_file );
				break;

			default:
				result = false;
			}
		}

		return result;
	}

	//*************************************************************************************************

	OverlayCategory::OverlayCategory( OverlayType p_type )
		: m_type( p_type )
	{
	}

	OverlayCategory::~OverlayCategory()
	{
	}

	void OverlayCategory::update()
	{
		OverlayRendererSPtr renderer = getOverlay().getEngine()->getOverlayCache().getRenderer();

		if ( renderer )
		{
			if ( isPositionChanged() || renderer->isSizeChanged() )
			{
				doUpdatePosition();
			}

			if ( isSizeChanged() || renderer->isSizeChanged() )
			{
				doUpdateSize();
			}

			if ( isChanged() || isSizeChanged() || renderer->isSizeChanged() )
			{
				doUpdate();
				doUpdateBuffer( renderer->getSize() );
			}

			m_positionChanged = false;
			m_sizeChanged = false;
		}
	}

	void OverlayCategory::render()
	{
		doRender( getOverlay().getEngine()->getOverlayCache().getRenderer() );
	}

	void OverlayCategory::setMaterial( MaterialSPtr p_material )
	{
		m_pMaterial = p_material;

		if ( p_material )
		{
			m_strMatName = p_material->getName();
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

	Position OverlayCategory::getAbsolutePosition( castor::Size const & p_size )const
	{
		Point2d position = getAbsolutePosition();
		return Position( int32_t( p_size.getWidth() * position[0] ), int32_t( p_size.getHeight() * position[1] ) );
	}

	Size OverlayCategory::getAbsoluteSize( castor::Size const & p_size )const
	{
		Point2d size = getAbsoluteSize();
		return Size( uint32_t( p_size.getWidth() * size[0] ), uint32_t( p_size.getHeight() * size[1] ) );
	}

	Point2d OverlayCategory::getAbsolutePosition()const
	{
		Point2d position = getPosition();
		OverlaySPtr parent = getOverlay().getParent();

		if ( parent )
		{
			position *= parent->getAbsoluteSize();
			position += parent->getAbsolutePosition();
		}

		return position;
	}

	Point2d OverlayCategory::getAbsoluteSize()const
	{
		Point2d size = getSize();
		OverlaySPtr parent = getOverlay().getParent();

		if ( parent )
		{
			size *= parent->getAbsoluteSize();
		}

		return size;
	}

	bool OverlayCategory::isSizeChanged()const
	{
		bool changed = m_sizeChanged;
		OverlaySPtr parent = getOverlay().getParent();

		if ( !changed && parent )
		{
			changed = parent->isSizeChanged();
		}

		return changed;
	}

	bool OverlayCategory::isPositionChanged()const
	{
		bool changed = m_positionChanged;
		OverlaySPtr parent = getOverlay().getParent();

		if ( !changed && parent )
		{
			changed = parent->isPositionChanged();
		}

		return changed;
	}

	Point2d OverlayCategory::doGetTotalSize()const
	{
		OverlaySPtr parent = getOverlay().getParent();
		Size renderSize = getOverlay().getEngine()->getOverlayCache().getRenderer()->getSize();
		Point2d totalSize( renderSize.getWidth(), renderSize.getHeight() );

		if ( parent )
		{
			Point2d parentSize = parent->getAbsoluteSize();
			totalSize[0] = parentSize[0] * totalSize[0];
			totalSize[1] = parentSize[1] * totalSize[1];
		}

		return totalSize;
	}

	void OverlayCategory::doUpdatePosition()
	{
		OverlayRendererSPtr renderer = getOverlay().getEngine()->getOverlayCache().getRenderer();

		if ( renderer )
		{
			if ( isPositionChanged() || renderer->isSizeChanged() )
			{
				Point2d totalSize = doGetTotalSize();
				bool changed = m_positionChanged;
				Position pos = getPixelPosition();
				Point2d ptPos = getPosition();

				if ( pos.x() )
				{
					changed = !castor::Policy< double >::equals( ptPos[0], pos.x() / totalSize[0] );
					ptPos[0] = pos.x() / totalSize[0];
				}

				if ( pos.y() )
				{
					changed = !castor::Policy< double >::equals( ptPos[1], pos.y() / totalSize[1] );
					ptPos[1] = pos.y() / totalSize[1];
				}

				if ( changed )
				{
					setPosition( ptPos );
				}
			}
		}
	}

	void OverlayCategory::doUpdateSize()
	{
		OverlayRendererSPtr renderer = getOverlay().getEngine()->getOverlayCache().getRenderer();

		if ( renderer )
		{
			if ( isSizeChanged() || renderer->isSizeChanged() )
			{
				Point2d totalSize = doGetTotalSize();
				bool changed = m_sizeChanged;
				Size size = getPixelSize();
				Point2d ptSize = getSize();

				if ( size.getWidth() )
				{
					changed = !castor::Policy< double >::equals( ptSize[0], size.getWidth() / totalSize[0] );
					ptSize[0] = size.getWidth() / totalSize[0];
				}

				if ( size.getHeight() )
				{
					changed = !castor::Policy< double >::equals( ptSize[1], size.getHeight() / totalSize[1] );
					ptSize[1] = size.getHeight() / totalSize[1];
				}

				if ( changed )
				{
					setSize( ptSize );
				}
			}
		}
	}
}

#include "Overlay.hpp"

#include "Engine.hpp"

#include "BorderPanelOverlay.hpp"
#include "PanelOverlay.hpp"
#include "TextOverlay.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	Overlay::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< Overlay >{ tabs }
	{
	}

	bool Overlay::TextWriter::operator()( Overlay const & overlay, TextFile & file )
	{
		return overlay.m_category->createTextWriter( m_tabs )->writeInto( file );
	}

	//*************************************************************************************************

	Overlay::Overlay( Engine & engine, OverlayType type )
		: OwnedBy< Engine >( engine )
		, m_parent()
		, m_scene()
		, m_renderSystem( engine.getRenderSystem() )
		, m_category( engine.getOverlayCache().getFactory().create( type ) )
	{
		m_category->setOverlay( this );
	}

	Overlay::Overlay( Engine & engine, OverlayType type, SceneSPtr scene, OverlaySPtr parent )
		: OwnedBy< Engine >( engine )
		, m_parent( parent )
		, m_scene( scene )
		, m_renderSystem( engine.getRenderSystem() )
		, m_category( engine.getOverlayCache().getFactory().create( type ) )
	{
		m_category->setOverlay( this );
	}

	Overlay::~Overlay()
	{
	}

	void Overlay::addChild( OverlaySPtr overlay )
	{
		int index = 1;

		if ( !m_overlays.empty() )
		{
			index = ( *( m_overlays.end() - 1 ) )->getIndex() + 1;
		}

		overlay->setOrder( index, getLevel() + 1 );
		m_overlays.push_back( overlay );
	}

	uint32_t Overlay::getChildrenCount( int level )const
	{
		uint32_t result{ 0 };

		if ( level == getLevel() + 1 )
		{
			result = int( m_overlays.size() );
		}
		else if ( level > getLevel() )
		{
			for ( auto overlay : m_overlays )
			{
				result += overlay->getChildrenCount( level );
			}
		}

		return result;
	}

	PanelOverlaySPtr Overlay::getPanelOverlay()const
	{
		if ( m_category->getType() != OverlayType::ePanel )
		{
			CU_Exception( "This overlay is not a panel." );
		}

		return std::static_pointer_cast< PanelOverlay >( m_category );
	}

	BorderPanelOverlaySPtr Overlay::getBorderPanelOverlay()const
	{
		if ( m_category->getType() != OverlayType::eBorderPanel )
		{
			CU_Exception( "This overlay is not a border panel." );
		}

		return std::static_pointer_cast< BorderPanelOverlay >( m_category );
	}

	TextOverlaySPtr Overlay::getTextOverlay()const
	{
		if ( m_category->getType() != OverlayType::eText )
		{
			CU_Exception( "This overlay is not a text." );
		}

		return std::static_pointer_cast< TextOverlay >( m_category );
	}

	bool Overlay::isVisible()const
	{
		bool result = m_category->isVisible();

		if ( result && getParent() )
		{
			result = getParent()->isVisible();
		}

		return result;
	}
}

#include "Overlay.hpp"

#include "Engine.hpp"

#include "BorderPanelOverlay.hpp"
#include "PanelOverlay.hpp"
#include "TextOverlay.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	Overlay::TextWriter::TextWriter( String const & p_tabs )
		: castor::TextWriter< Overlay >{ p_tabs }
	{
	}

	bool Overlay::TextWriter::operator()( Overlay const & p_overlay, TextFile & p_file )
	{
		return p_overlay.m_category->createTextWriter( m_tabs )->writeInto( p_file );
	}

	//*************************************************************************************************

	Overlay::Overlay( Engine & engine, OverlayType p_type )
		: OwnedBy< Engine >( engine )
		, m_parent()
		, m_scene()
		, m_renderSystem( engine.getRenderSystem() )
		, m_category( engine.getOverlayCache().getFactory().create( p_type ) )
	{
		m_category->setOverlay( this );
	}

	Overlay::Overlay( Engine & engine, OverlayType p_type, SceneSPtr p_scene, OverlaySPtr p_parent )
		: OwnedBy< Engine >( engine )
		, m_parent( p_parent )
		, m_scene( p_scene )
		, m_renderSystem( engine.getRenderSystem() )
		, m_category( engine.getOverlayCache().getFactory().create( p_type ) )
	{
		m_category->setOverlay( this );
	}

	Overlay::~Overlay()
	{
	}

	void Overlay::render( castor::Size const & size
		, OverlayRenderer & renderer )
	{
		if ( isVisible() )
		{
			m_category->render( renderer );

			for ( auto overlay : m_overlays )
			{
				overlay->render( size, renderer );
			}
		}
	}

	void Overlay::addChild( OverlaySPtr p_overlay )
	{
		int index = 1;

		if ( !m_overlays.empty() )
		{
			index = ( *( m_overlays.end() - 1 ) )->getIndex() + 1;
		}

		p_overlay->setOrder( index, getLevel() + 1 );
		m_overlays.push_back( p_overlay );
	}

	uint32_t Overlay::getChildrenCount( int p_level )const
	{
		uint32_t result{ 0 };

		if ( p_level == getLevel() + 1 )
		{
			result = int( m_overlays.size() );
		}
		else if ( p_level > getLevel() )
		{
			for ( auto overlay : m_overlays )
			{
				result += overlay->getChildrenCount( p_level );
			}
		}

		return result;
	}

	PanelOverlaySPtr Overlay::getPanelOverlay()const
	{
		if ( m_category->getType() != OverlayType::ePanel )
		{
			CASTOR_EXCEPTION( "This overlay is not a panel." );
		}

		return std::static_pointer_cast< PanelOverlay >( m_category );
	}

	BorderPanelOverlaySPtr Overlay::getBorderPanelOverlay()const
	{
		if ( m_category->getType() != OverlayType::eBorderPanel )
		{
			CASTOR_EXCEPTION( "This overlay is not a border panel." );
		}

		return std::static_pointer_cast< BorderPanelOverlay >( m_category );
	}

	TextOverlaySPtr Overlay::getTextOverlay()const
	{
		if ( m_category->getType() != OverlayType::eText )
		{
			CASTOR_EXCEPTION( "This overlay is not a text." );
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

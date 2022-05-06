#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"

#include <RenderGraph/FramePassTimer.hpp>

#include <iomanip>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdtor-name"

//*********************************************************************************************

namespace std
{
	inline std::ostream & operator<<( std::ostream & p_stream, castor::Nanoseconds const & p_duration )
	{
		p_stream << std::setprecision( 3 ) << ( float( p_duration.count() ) / 1000000.0f ) << cuT( " ms" );
		return p_stream;
	}
}

namespace castor3d
{
	//*********************************************************************************************

	template< typename T >
	DebugOverlays::DebugPanelT< T >::DebugPanelT( castor::String const & name
		, castor::String const & label
		, PanelOverlaySPtr panel
		, OverlayCache & cache
		, T const & value )
		: m_cache{ cache }
		, m_v{ value }
	{
		auto baseName = cuT( "DebugPanel-" ) + name;
		m_label = cache.add( baseName + cuT( "-Label" )
			, cache.getEngine()
			, OverlayType::eText
			, nullptr
			, &panel->getOverlay() ).lock()->getTextOverlay();
		m_value = cache.add( baseName + cuT( "-Value" )
			, cache.getEngine()
			, OverlayType::eText
			, nullptr
			, &panel->getOverlay() ).lock()->getTextOverlay();

		m_label->setPixelPosition( castor::Position{ 10, 0 } );
		m_value->setPixelPosition( castor::Position{ 200, 0 } );

		m_label->setPixelSize( castor::Size{ 190, 20 } );
		m_value->setPixelSize( castor::Size{ 110, 20 } );

		m_label->setFont( cuT( "Arial16" ) );
		m_value->setFont( cuT( "Arial16" ) );

		m_label->setVAlign( VAlign::eCenter );
		m_value->setVAlign( VAlign::eCenter );

		auto & engine = cache.getEngine();
		m_label->setMaterial( engine.findMaterial( cuT( "White" ) ).lock().get() );
		m_value->setMaterial( engine.findMaterial( cuT( "White" ) ).lock().get() );

		m_label->setCaption( label );
	}

	template< typename T >
	DebugOverlays::DebugPanelT< T >::~DebugPanelT()
	{
		if ( m_value )
		{
			m_cache.remove( m_value->getOverlay().getName() );
			m_value.reset();
		}

		if ( m_label )
		{
			m_cache.remove( m_label->getOverlay().getName() );
			m_label.reset();
		}
	}

	template< typename T >
	void DebugOverlays::DebugPanelT< T >::updatePosition( int y )
	{
		m_value->setPixelPosition( castor::Position{ 200, y } );
		m_label->setPixelPosition( castor::Position{ 10, y } );
	}

	template< typename T >
	void DebugOverlays::DebugPanelT< T >::update()
	{
		m_value->setCaption( castor::string::toString( m_v ) );
	}

	//*********************************************************************************************

	template< typename T >
	DebugOverlays::DebugPanelsT< T >::DebugPanelsT( castor::String const & title
		, PanelOverlaySPtr panel
		, OverlayCache & cache )
		: m_cache{ cache }
		, m_panel{ panel }
		, m_titlePanel{ m_cache.add( cuT( "DebugPanels-" ) + title + cuT( "-TitlePanel" )
			, cache.getEngine()
			, OverlayType::ePanel
			, nullptr
			, &m_panel->getOverlay() ).lock()->getPanelOverlay() }
		, m_titleText{ m_cache.add( cuT( "DebugPanels-" ) + title + cuT( "-TitleText" )
			, cache.getEngine()
			, OverlayType::eText
			, nullptr
			, &m_titlePanel->getOverlay() ).lock()->getTextOverlay() }
	{
		auto & engine = m_cache.getEngine();

		m_titlePanel->setPixelSize( castor::Size{ 320, 20 } );
		m_titlePanel->setMaterial( engine.findMaterial( cuT( "AlphaDarkBlue" ) ).lock().get() );

		m_titleText->setPixelSize( castor::Size{ 300, 20 } );
		m_titleText->setVAlign( VAlign::eCenter );
		m_titleText->setHAlign( HAlign::eCenter );
		m_titleText->setMaterial( engine.findMaterial( cuT( "White" ) ).lock().get() );
		m_titleText->setFont( cuT( "Arial16" ) );
		m_titleText->setCaption( title );
	}

	template< typename T >
	DebugOverlays::DebugPanelsT< T >::~DebugPanelsT()
	{
		m_panels.clear();

		if ( m_titleText )
		{
			m_cache.remove( m_titleText->getOverlay().getName() );
			m_titleText.reset();
		}

		if ( m_titlePanel )
		{
			m_cache.remove( m_titlePanel->getOverlay().getName() );
			m_titlePanel.reset();
		}
	}

	template< typename T >
	void DebugOverlays::DebugPanelsT< T >::update()
	{
		for ( auto & panel : m_panels )
		{
			panel.update();
		}
	}

	template< typename T >
	int DebugOverlays::DebugPanelsT< T >::updatePosition( int y )
	{
		m_titlePanel->setPixelPosition( castor::Position{ 0, y } );
		m_titleText->setPixelPosition( castor::Position{ 10, 0 } );
		y += 20;

		for ( auto & panel : m_panels )
		{
			panel.updatePosition( y );
			y += 20;
		}

		return y;
	}

	template< typename T >
	void DebugOverlays::DebugPanelsT< T >::add( castor::String const & name
		, castor::String const & label
		, T const & value )
	{
		m_panels.emplace_back( name
			, label
			, m_panel
			, m_cache
			, value );
	}

	//*********************************************************************************************
}

#pragma clang diagnostic pop

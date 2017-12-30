﻿#include "Engine.hpp"
#include "Overlay/PanelOverlay.hpp"
#include "Overlay/TextOverlay.hpp"
#include "Render/RenderPassTimer.hpp"

#include <iomanip>

//*********************************************************************************************

namespace std
{
	inline std::ostream & operator<<( std::ostream & p_stream, castor::Nanoseconds const & p_duration )
	{
		p_stream << std::setprecision( 3 ) << ( p_duration.count() / 1000000.0_r ) << cuT( " ms" );
		return p_stream;
	}
}

//*********************************************************************************************

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	template< typename T >
	DebugOverlays::DebugPanel< T >::DebugPanel( String const & name
		, String const & label
		, PanelOverlaySPtr panel
		, OverlayCache & cache
		, T const & value )
		: m_cache{ cache }
		, m_v{ value }
	{
		auto baseName = cuT( "DebugPanel-" ) + name;
		m_label = cache.add( baseName + cuT( "-Label" )
			, OverlayType::eText
			, nullptr
			, panel->getOverlay().shared_from_this() )->getTextOverlay();
		m_value = cache.add( baseName + cuT( "-Value" )
			, OverlayType::eText
			, nullptr
			, panel->getOverlay().shared_from_this() )->getTextOverlay();

		m_label->setPixelPosition( Position{ 10, 0 } );
		m_value->setPixelPosition( Position{ 200, 0 } );

		m_label->setPixelSize( Size{ 190, 20 } );
		m_value->setPixelSize( Size{ 110, 20 } );

		m_label->setFont( cuT( "Arial20" ) );
		m_value->setFont( cuT( "Arial20" ) );

		m_label->setVAlign( VAlign::eCenter );
		m_value->setVAlign( VAlign::eCenter );

		auto & materials = cache.getEngine()->getMaterialCache();
		m_label->setMaterial( materials.find( cuT( "White" ) ) );
		m_value->setMaterial( materials.find( cuT( "White" ) ) );

		m_label->setCaption( label );
	}

	template< typename T >
	DebugOverlays::DebugPanel< T >::~DebugPanel()
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
	void DebugOverlays::DebugPanel< T >::updatePosition( int y )
	{
		m_value->setPixelPosition( Position{ 200, y } );
		m_label->setPixelPosition( Position{ 10, y } );
	}

	template< typename T >
	void DebugOverlays::DebugPanel< T >::update()
	{
		m_value->setCaption( StringStream{} << std::setprecision( 4 ) << m_v );
	}

	//*********************************************************************************************

	template< typename T >
	DebugOverlays::DebugPanels< T >::DebugPanels( castor::String const & title
		, PanelOverlaySPtr panel
		, OverlayCache & cache )
		: m_cache{ cache }
		, m_panel{ panel }
		, m_titlePanel{ m_cache.add( cuT( "DebugPanels-" ) + title + cuT( "-TitlePanel" )
			, OverlayType::ePanel
			, nullptr
			, m_panel->getOverlay().shared_from_this() )->getPanelOverlay() }
		, m_titleText{ m_cache.add( cuT( "DebugPanels-" ) + title + cuT( "-TitleText" )
			, OverlayType::eText
			, nullptr
			, m_titlePanel->getOverlay().shared_from_this() )->getTextOverlay() }
	{
		auto & materials = m_cache.getEngine()->getMaterialCache();

		m_titlePanel->setPixelSize( Size{ 320, 20 } );
		m_titlePanel->setMaterial( materials.find( cuT( "AlphaDarkBlue" ) ) );

		m_titleText->setPixelSize( Size{ 300, 20 } );
		m_titleText->setVAlign( VAlign::eCenter );
		m_titleText->setHAlign( HAlign::eCenter );
		m_titleText->setMaterial( materials.find( cuT( "White" ) ) );
		m_titleText->setFont( cuT( "Arial20" ) );
		m_titleText->setCaption( title );
	}

	template< typename T >
	DebugOverlays::DebugPanels< T >::~DebugPanels()
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
	void DebugOverlays::DebugPanels< T >::update()
	{
		for ( auto & panel : m_panels )
		{
			panel.update();
		}
	}

	template< typename T >
	int DebugOverlays::DebugPanels< T >::updatePosition( int y )
	{
		m_titlePanel->setPixelPosition( Position{ 0, y } );
		m_titleText->setPixelPosition( Position{ 10, 0 } );
		y += 20;

		for ( auto & panel : m_panels )
		{
			panel.updatePosition( y );
			y += 20;
		}

		return y;
	}

	template< typename T >
	void DebugOverlays::DebugPanels< T >::add( castor::String const & name
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

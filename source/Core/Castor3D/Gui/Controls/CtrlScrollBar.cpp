#include "Castor3D/Gui/Controls/CtrlScrollBar.hpp"

#include "Castor3D/Gui/ControlsManager.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Graphics/Font.hpp>

CU_ImplementSmartPtr( castor3d, ScrollBarCtrl )

namespace castor3d
{
	ScrollBarCtrl::ScrollBarCtrl( SceneRPtr scene
		, castor::String const & name
		, ScrollBarStyleRPtr style
		, ControlRPtr parent )
		: ScrollBarCtrl{ scene
			, name
			, style
			, parent
			, castor::makeRangedValue( 0.0f, 0.0f, 100.0f )
			, castor::Position{}
			, castor::Size{}
			, 0u
			, true }
	{
	}

	ScrollBarCtrl::ScrollBarCtrl( SceneRPtr scene
		, castor::String const & name
		, ScrollBarStyleRPtr style
		, ControlRPtr parent
		, castor::RangedValue< float > const & value
		, castor::Position const & position
		, castor::Size const & size
		, ControlFlagType flags
		, bool visible )
		: Control{ Type
			, scene
			, name
			, style
			, parent
			, position
			, size
			, flags
			, visible }
		, m_value{ value }
	{
		CU_Require( isScrollableControl( *parent ) );
		auto & manager = *getEngine().getControlsManager();
		setBorderSize( castor::Point4ui{} );
		EventHandler::connect( KeyboardEventType::ePushed
			, [this]( KeyboardEvent const & event )
			{
				onKeyDown( event );
			} );

		m_begin = manager.registerControlT( castor::makeUnique< ButtonCtrl >( m_scene
			, cuT( "Begin" )
			, &style->getBeginStyle()
			, this
			, U"-"
			, castor::Position{}
			, castor::Size{} ) );
		m_begin->setVisible( visible );
		m_onBeginClick = m_begin->connect( ButtonEvent::eClicked
			, [this]()
			{
				doScroll( -1 );
			} );

		m_end = manager.registerControlT( castor::makeUnique< ButtonCtrl >( m_scene
			, cuT( "End" )
			, &style->getEndStyle()
			, this
			, U"+"
			, castor::Position{}
			, castor::Size{} ) );
		m_end->setVisible( visible );
		m_onEndClick = m_end->connect( ButtonEvent::eClicked
			, [this]()
			{
				doScroll( 1 );
			} );

		m_bar = manager.registerControlT( castor::makeUnique< PanelCtrl >( m_scene
			, cuT( "Line" )
			, &style->getBarStyle()
			, this
			, castor::Position{}
			, castor::Size{} ) );
		m_bar->setVisible( visible );
		m_bar->connectNC( KeyboardEventType::ePushed
			, [this]( ControlRPtr, KeyboardEvent const & event )
			{
				onNcKeyDown( event );
			} );

		m_thumb = manager.registerControlT( castor::makeUnique< PanelCtrl >( m_scene
			, cuT( "Thumb" )
			, &style->getThumbStyle()
			, m_bar
			, castor::Position{}
			, castor::Size{} ) );
		m_thumb->setBorderSize( { 1u, 1u, 1u, 1u } );
		m_thumb->setVisible( visible );
		m_thumb->setCatchesMouseEvents( true );
		m_thumb->connectNC( MouseEventType::eMove
			, [this]( ControlRPtr, MouseEvent const & event )
			{
				onThumbMouseMove( event );
			} );
		m_thumb->connectNC( MouseEventType::ePushed
			, [this]( ControlRPtr, MouseEvent const & event )
			{
				onThumbMouseButtonDown( event );
			} );
		m_thumb->connectNC( MouseEventType::eReleased
			, [this]( ControlRPtr, MouseEvent const & event )
			{
				onThumbMouseButtonUp( event );
			} );
		m_thumb->connectNC( KeyboardEventType::ePushed
			, [this]( ControlRPtr, KeyboardEvent const & event )
			{
				onNcKeyDown( event );
			} );

		setStyle( style );
	}

	ScrollBarCtrl::~ScrollBarCtrl()noexcept
	{
		auto & manager = *getEngine().getControlsManager();
		manager.unregisterControl( *m_thumb );
		manager.unregisterControl( *m_bar );
	}

	void ScrollBarCtrl::setRange( castor::Range< uint32_t > const & value )
	{
		m_totalRange = value;
		doUpdatePosSize();
	}

	void ScrollBarCtrl::updateThumb( int32_t value )
	{
		auto diff = m_value.range().value( m_totalRange.percent( uint32_t( -std::min( value, 0 ) ) ) ) - m_value.value();

		if ( isVertical() )
		{
			doUpdateThumb( { 0.0f, diff } );
		}
		else
		{
			doUpdateThumb( { diff, 0.0f } );
		}
	}

	void ScrollBarCtrl::doUpdateStyle()
	{
		auto & style = getStyle();
		m_thumb->setStyle( &style.getThumbStyle() );
		m_begin->setStyle( &style.getBeginStyle() );
		m_end->setStyle( &style.getEndStyle() );
		m_bar->setStyle( &style.getBarStyle() );
	}

	void ScrollBarCtrl::doCreate()
	{
		CU_Require( getControlsManager() );
		auto & manager = *getControlsManager();

		manager.create( m_bar );
		manager.create( m_thumb );
		manager.create( m_begin );
		manager.create( m_end );

		doUpdateStyle();
		doUpdatePosSize();

		manager.connectEvents( *this );
	}

	void ScrollBarCtrl::doDestroy()
	{
		CU_Require( getControlsManager() );
		auto & manager = *getControlsManager();
		manager.disconnectEvents( *this );

		manager.destroy( m_bar );
		manager.destroy( m_thumb );
		manager.destroy( m_begin );
		manager.destroy( m_end );
	}

	void ScrollBarCtrl::doSetPosition( castor::Position const & value )
	{
		doUpdatePosSize();
	}

	void ScrollBarCtrl::doSetSize( castor::Size const & value )
	{
		doUpdatePosSize();
	}

	void ScrollBarCtrl::doSetCaption( castor::U32String const & value )
	{
	}

	void ScrollBarCtrl::doSetVisible( bool visible )
	{
	}

	void ScrollBarCtrl::doUpdateFlags()
	{
		doUpdatePosSize();
	}

	void ScrollBarCtrl::doOnMouseMove( MouseEvent const & event )
	{
		if ( m_scrolling )
		{
			doMoveMouse( event.getPosition() );
			m_signals[size_t( ScrollBarEvent::eThumbTrack )]( int32_t( m_value.value() ) );
		}
	}

	void ScrollBarCtrl::doOnMouseButtonUp( MouseEvent const & event )
	{
		if ( event.getButton() == MouseButton::eLeft )
		{
			if ( !m_scrolling )
			{
				m_mouse = event.getPosition() - getAbsolutePosition();
			}

			doMoveMouse( event.getPosition() );
			m_signals[size_t( ScrollBarEvent::eThumbRelease )]( int32_t( m_value.value() ) );
			m_scrolling = false;
		}
	}

	void ScrollBarCtrl::doOnMouseLeave( MouseEvent const & event )
	{
		auto controls = getControlsManager();

		if ( !controls )
		{
			return;
		}

		auto focusedControl = controls->getFocusedControl();

		if ( m_scrolling
			&& focusedControl != this
			&& focusedControl != m_thumb
			&& focusedControl != m_bar )
		{
			doMoveMouse( event.getPosition() );
			m_signals[size_t( ScrollBarEvent::eThumbRelease )]( int32_t( m_value.value() ) );
			m_scrolling = false;
		}
	}

	void ScrollBarCtrl::doUpdateZIndex( uint32_t & index )
	{
	}

	void ScrollBarCtrl::doAdjustZIndex( uint32_t offset )
	{
	}

	void ScrollBarCtrl::onThumbMouseMove( MouseEvent const & event )
	{
		doOnMouseMove( event );
	}

	void ScrollBarCtrl::onThumbMouseButtonDown( MouseEvent const & event )
	{
		if ( event.getButton() == MouseButton::eLeft )
		{
			m_scrolling = true;
			m_mouse = event.getPosition() - getAbsolutePosition();
		}
	}

	void ScrollBarCtrl::onThumbMouseButtonUp( MouseEvent const & event )
	{
		doOnMouseButtonUp( event );
	}

	void ScrollBarCtrl::onKeyDown( KeyboardEvent const & event )
	{
		if ( !m_scrolling )
		{
			if ( event.getKey() == KeyboardKey::eUp )
			{
				doScroll( -1 );
			}
			else if ( event.getKey() == KeyboardKey::eDown )
			{
				doScroll( 1 );
			}
		}
	}

	void ScrollBarCtrl::onNcKeyDown( KeyboardEvent const & event )
	{
		onKeyDown( event );
	}

	void ScrollBarCtrl::doMoveMouse( castor::Position const & mouse )
	{
		castor::Point2i relativePosition = mouse - getAbsolutePosition();
		castor::Point2i delta = relativePosition - m_mouse;
		m_mouse = relativePosition;
		doUpdateThumb( { float( delta[0] ), float( delta[1] ) } );
	}

	void ScrollBarCtrl::doUpdateThumb( castor::Point2f const & delta )
	{
		castor::Point2f realDelta = delta;

		if ( isVertical() )
		{
			realDelta->x = 0.0f;
		}
		else
		{
			realDelta->y = 0.0f;
		}

		if ( auto thumb = m_thumb )
		{
			castor::Point2f position = realDelta + castor::Point2f{ thumb->getPosition().x(), thumb->getPosition().y() };

			if ( auto bar = m_bar )
			{
				auto size = bar->getSize();

				if ( isVertical() )
				{
					m_value = std::min( float( size->y ), std::max( 0.0f, position[1] ) );
					position[1] = m_value.value();
				}
				else
				{
					m_value = std::min( float( size->x ), std::max( 0.0f, position[0] ) );
					position[0] = m_value.value();
				}

				m_scrollPosition = -int32_t( m_totalRange.value( m_value.percent() ) );
			}

			thumb->setPosition( { int32_t( position[0] ), int32_t( position[1] ) } );
		}
	}

	void ScrollBarCtrl::doUpdatePosSize()
	{
		auto size = getClientSize();
		auto offset = getClientOffset();
		auto barSize{ size };
		auto barPosition{ offset };
		auto thumbPosition{ offset };
		auto endPosition{ offset };
		auto percent = m_value.percent();
		uint32_t minSize{};
		castor::Size thumbSize{};

		if ( isVertical() )
		{
			minSize = size->x;
			barSize->x = minSize;
			barSize->y = size->y - minSize * 2u;
			barPosition.y() += minSize;
			endPosition.y() += minSize + barSize->y;

			auto rangeRatio = std::min( 1.0
				, double( barSize->y ) / ( double( m_totalRange.getMax() ) + double( getParent()->getClientSize()->y ) - double( minSize * 2u ) ) );
			thumbSize->x = minSize;
			thumbSize->y = std::max( minSize, uint32_t( double( barSize->y ) * rangeRatio ) );

			m_value.updateRange( castor::makeRange( 0.0f, float( barSize->y - thumbSize->y ) ) );
			m_value = m_value.range().getMax() * percent;
			thumbPosition.y() += int32_t( m_value.value() );
		}
		else
		{
			minSize = size->y;
			barSize->x = size->x - minSize * 2u;
			barSize->y = minSize;
			barPosition.x() += minSize;
			endPosition.x() += minSize + barSize->x;

			auto rangeRatio = std::min( 1.0
				, double( barSize->x ) / ( double( m_totalRange.getMax() ) + double( getParent()->getClientSize()->x ) - double( minSize * 2u ) ) );
			thumbSize->x = std::max( minSize, uint32_t( double( barSize->x ) * rangeRatio ) );
			thumbSize->y = minSize;

			m_value.updateRange( castor::makeRange( 0.0f, float( barSize->x - thumbSize->x ) ) );
			m_value = m_value.range().getMax() * percent;
			thumbPosition.x() += int32_t( m_value.value() );
		}

		if ( auto begin = m_begin )
		{
			begin->setPosition( offset );
			begin->setSize( { minSize, minSize } );
			begin->setVisible( isBackgroundVisible() );
		}

		if ( auto bar = m_bar )
		{
			bar->setPosition( barPosition );
			bar->setSize( barSize );
			bar->setVisible( isBackgroundVisible() );
		}

		if ( auto thumb = m_thumb )
		{
			thumb->setPosition( thumbPosition );
			thumb->setSize( thumbSize );
			thumb->setVisible( isBackgroundVisible() );
		}

		if ( auto end = m_end )
		{
			end->setPosition( endPosition );
			end->setSize( { minSize, minSize } );
			end->setVisible( isBackgroundVisible() );
		}
	}

	void ScrollBarCtrl::doScroll( int32_t v )
	{
		if ( isVertical() )
		{
			doUpdateThumb( { 0.0f, float( v ) } );
		}
		else
		{
			doUpdateThumb( { float( v ), 0.0f } );
		}

		m_signals[size_t( ScrollBarEvent::eThumbRelease )]( int32_t( m_value.value() ) );
	}
}

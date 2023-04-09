#include "Castor3D/Gui/Controls/CtrlScrollable.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Gui/Controls/CtrlLayoutControl.hpp"
#include "Castor3D/Gui/Controls/CtrlScrollBar.hpp"
#include "Castor3D/Gui/Layout/Layout.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Graphics/Font.hpp>

CU_ImplementSmartPtr( castor3d, ScrollableCtrl )

namespace castor3d
{
	//************************************************************************************************

	namespace scrollable
	{
		static void checkScrollBar( ScrollBarFlag flag
			, Control & control
			, castor::String const & prefix
			, ScrollableStyleRPtr style
			, bool ( ScrollableStyle::* has )()const noexcept
			, ScrollBarStyle & ( ScrollableStyle::* get )()const noexcept
			, uint32_t rangeMax
			, castor::Size dim
			, ScrollBarCtrlRPtr & scrollBar )
		{
			if ( castor::checkFlag( control.getFlags(), flag ) )
			{
				if ( !scrollBar )
				{
					CU_Require( style && ( style->*has )() );
					auto & manager = *control.getEngine().getControlsManager();
					auto scene = control.hasScene() ? &control.getScene() : nullptr;
					scrollBar = manager.registerControlT( castor::makeUnique< ScrollBarCtrl >( scene
						, "Scroll/" + prefix
						, &( style->*get )()
						, &control
						, castor::makeRangedValue( 0.0f, 0.0f, float( rangeMax ) )
						, castor::Position{}
						, std::move( dim )
						, ControlFlagType( flag ) ) );
				}

				scrollBar->setVisible( control.isBackgroundVisible() );
				scrollBar->enable();
			}
			else if ( scrollBar )
			{
				scrollBar->setVisible( false );
				scrollBar->disable();
			}
		}
	}

	//************************************************************************************************

	bool isScrollableControl( ControlType type )
	{
		return isLayoutControl( type )
			|| type == ControlType::eEdit;
	}

	bool isScrollableControl( Control const & control )
	{
		return isScrollableControl( control.getType() );
	}

	//************************************************************************************************

	ScrollableCtrl::ScrollableCtrl( Control & target
		, ScrollableStyleRPtr style )
		: m_target{ target }
		, m_scrollableStyle{ style }
	{
	}

	ScrollableCtrl::~ScrollableCtrl()noexcept
	{
		if ( m_corner )
		{
			if ( m_target.hasScene() )
			{
				m_target.getScene().removeOverlay( m_target.getName() + cuT( "/Scroll/Corner" ), true );
			}
			else
			{
				m_target.getEngine().removeOverlay( m_target.getName() + cuT( "/Scroll/Corner" ), true );
			}
		}

		auto & manager = *m_target.getEngine().getControlsManager();

		if ( m_verticalScrollBar )
		{
			manager.unregisterControl( *m_verticalScrollBar );
		}

		if ( m_horizontalScrollBar )
		{
			manager.unregisterControl( *m_horizontalScrollBar );
		}
	}

	castor::Position ScrollableCtrl::getScrollPosition()const
	{
		castor::Position result{};

		if ( m_verticalScrollBar )
		{
			result.y() = m_verticalScrollBar->getScrollPosition();
		}

		if ( m_horizontalScrollBar )
		{
			result.x() = m_horizontalScrollBar->getScrollPosition();
		}

		return result;
	}

	void ScrollableCtrl::createScrollBars()
	{
		if ( m_verticalScrollBar )
		{
			m_target.getControlsManager()->create( m_verticalScrollBar );
			m_onVerticalThumbRelease = m_verticalScrollBar->connect( ScrollBarEvent::eThumbRelease
				, [this]( int32_t position )
				{
					doScrollContent();
				} );
			m_onVerticalThumbTrack = m_verticalScrollBar->connect( ScrollBarEvent::eThumbTrack
				, [this]( int32_t position )
				{
					doScrollContent();
				} );
		}

		if ( m_horizontalScrollBar )
		{
			m_target.getControlsManager()->create( m_horizontalScrollBar );
			m_onHorizontalThumbRelease = m_horizontalScrollBar->connect( ScrollBarEvent::eThumbRelease
				, [this]( int32_t position )
				{
					doScrollContent();
				} );
			m_onHorizontalThumbTrack = m_horizontalScrollBar->connect( ScrollBarEvent::eThumbTrack
				, [this]( int32_t position )
				{
					doScrollContent();
				} );
		}
	}

	void ScrollableCtrl::destroyScrollBars()
	{
		if ( m_horizontalScrollBar )
		{
			m_target.getControlsManager()->destroy( m_horizontalScrollBar );
		}

		if ( m_verticalScrollBar )
		{
			m_target.getControlsManager()->destroy( m_verticalScrollBar );
		}
	}

	void ScrollableCtrl::updateScrollBarsStyle()
	{
		if ( m_verticalScrollBar )
		{
			m_verticalScrollBar->setStyle( &m_scrollableStyle->getVerticalStyle() );
		}

		if ( m_horizontalScrollBar )
		{
			m_horizontalScrollBar->setStyle( &m_scrollableStyle->getHorizontalStyle() );
		}

		if ( auto corner = m_corner )
		{
			corner->setMaterial( m_target.getBackgroundOverlay().getMaterial() );
		}
	}

	void ScrollableCtrl::checkScrollBarFlags()
	{
		scrollable::checkScrollBar( ScrollBarFlag::eVertical
			, m_target
			, "Vertic"
			, m_scrollableStyle
			, &ScrollableStyle::hasVerticalStyle
			, &ScrollableStyle::getVerticalStyle
			, m_target.getClientSize()->y
			, { 20u, m_target.getClientSize()->y }
			, m_verticalScrollBar );
		scrollable::checkScrollBar( ScrollBarFlag::eHorizontal
			, m_target
			, "Horiz"
			, m_scrollableStyle
			, &ScrollableStyle::hasHorizontalStyle
			, &ScrollableStyle::getHorizontalStyle
			, m_target.getClientSize()->x
			, { m_target.getClientSize()->x, 20u }
			, m_horizontalScrollBar );

		if ( m_verticalScrollBar && m_horizontalScrollBar )
		{
			if ( !m_corner )
			{
				auto corner = m_target.hasScene()
					? m_target.getScene().addNewOverlay( m_target.getName() + cuT( "/Scroll/Corner" )
						, m_target.getEngine()
						, OverlayType::ePanel
						, nullptr
						, &m_target.getBackgroundOverlay() )->getPanelOverlay()
					: m_target.getEngine().addNewOverlay( m_target.getName() + cuT( "/Scroll/Corner" )
						, m_target.getEngine()
						, OverlayType::ePanel
						, nullptr
						, &m_target.getBackgroundOverlay() )->getPanelOverlay();
				m_corner = corner;
			}

			if ( auto corner = m_corner )
			{
				corner->setVisible( true );
				corner->setMaterial( m_target.getBackgroundOverlay().getMaterial() );
			}
		}
		else if ( auto corner = m_corner )
		{
			corner->setVisible( false );
		}
	}

	void ScrollableCtrl::updateScrollBars()
	{
		if ( !m_verticalScrollBar
			&& !m_horizontalScrollBar )
		{
			return;
		}

		auto clientOffset = m_target.getClientOffset();
		auto clientSize = m_target.getClientSize();

		if ( m_verticalScrollBar )
		{
			auto vsize = m_verticalScrollBar->getSize();
			m_verticalScrollBar->setPosition( { clientOffset.x() + int32_t( clientSize->x )
				, clientOffset.y() } );
			m_verticalScrollBar->setSize( { vsize->x, clientSize->y } );
		}

		if ( m_horizontalScrollBar )
		{
			auto hsize = m_horizontalScrollBar->getSize();
			m_horizontalScrollBar->setPosition( { clientOffset.x()
				, clientOffset.x() + int32_t( clientSize->y ) } );
			m_horizontalScrollBar->setSize( { clientSize->x, hsize->y } );
		}

		if ( m_verticalScrollBar && m_horizontalScrollBar )
		{
			if ( auto corner = m_corner )
			{
				corner->setPixelPosition( { m_verticalScrollBar->getPosition().x()
					, m_horizontalScrollBar->getPosition().y() } );
				corner->setPixelSize( { m_verticalScrollBar->getSize()->x
					, m_horizontalScrollBar->getSize()->y } );
			}
		}
	}

	castor::Point4ui ScrollableCtrl::updateScrollableClientRect( castor::Point4ui const & clientRect )
	{
		castor::Point4ui result{ clientRect };

		if ( m_verticalScrollBar )
		{
			result->z -= m_verticalScrollBar->getSize()->x;
		}

		if ( m_horizontalScrollBar )
		{
			result->w -= m_horizontalScrollBar->getSize()->y;
		}

		return result;
	}

	void ScrollableCtrl::updateScrollBarsThumb( castor::Position const & pos )
	{
		if ( m_verticalScrollBar )
		{
			m_verticalScrollBar->updateThumb( pos.y() );
		}

		if ( m_horizontalScrollBar )
		{
			m_horizontalScrollBar->updateThumb( pos.x() );
		}
	}

	void ScrollableCtrl::setScrollBarsVisible( bool visible )
	{
		if ( m_verticalScrollBar )
		{
			m_verticalScrollBar->setVisible( visible );
		}

		if ( m_horizontalScrollBar )
		{
			m_horizontalScrollBar->setVisible( visible );
		}

		if ( m_verticalScrollBar
			&& m_horizontalScrollBar )
		{
			if ( auto corner = m_corner )
			{
				corner->setVisible( visible );
			}
		}
	}

	void ScrollableCtrl::registerControl( Control & control )
	{
		if ( !m_verticalScrollBar
			&& !m_horizontalScrollBar )
		{
			return;
		}

		if ( &control != m_verticalScrollBar
			&& &control != m_horizontalScrollBar )
		{
			m_controls.emplace( &control
				, ScrolledControl{ control.onChanged.connect( [this, &control]( Control const & ctrl )
					{
						if ( !m_updating )
						{
							doUpdateControlPosition( control );
							doUpdateScrollableContent();
						}
					} )
					, control.getPosition() } );
			doUpdateControlPosition( control );
			doUpdateScrollableContent();
		}
	}

	void ScrollableCtrl::unregisterControl( Control & control )
	{
		if ( !m_verticalScrollBar
			&& !m_horizontalScrollBar )
		{
			return;
		}

		if ( &control != m_verticalScrollBar
			&& &control != m_horizontalScrollBar )
		{
			auto it = m_controls.find( &control );

			if ( it != m_controls.end() )
			{
				m_controls.erase( it );
				doUpdateScrollableContent();
			}
		}
	}

	void ScrollableCtrl::updateTotalSize( castor::Size const & size )
	{
		if ( m_verticalScrollBar )
		{
			m_verticalScrollBar->setRange( castor::makeRange( 0u
				, uint32_t( std::max( 0
					, int32_t( size->y ) - int32_t( m_target.getClientSize()->y ) ) ) ) );
		}

		if ( m_horizontalScrollBar )
		{
			m_horizontalScrollBar->setRange( castor::makeRange( 0u
				, uint32_t( std::max( 0
					, int32_t( size->x ) - int32_t( m_target.getClientSize()->x ) ) ) ) );
		}
	}

	void ScrollableCtrl::updateScrollZIndex( uint32_t & index )
	{
		if ( auto corner = m_corner )
		{
			corner->setOrder( index++, 0u );
		}
	}

	void ScrollableCtrl::adjustScrollZIndex( uint32_t offset )
	{
		if ( auto corner = m_corner )
		{
			corner->setOrder( corner->getLevel() + offset, 0u );
		}
	}

	void ScrollableCtrl::doUpdateControlPosition( Control & ctrl )
	{
		auto it = m_controls.find( &ctrl );

		if ( it == m_controls.end() )
		{
			CU_Failure( "Control was not found in registered scrolled controls" );
			return;
		}

		it->second.originalPosition = ctrl.getPosition();
	}

	void ScrollableCtrl::doUpdateScrollableContent()
	{
		auto clientSize = m_target.getClientSize();
		int32_t minX{ int32_t( clientSize->x ) };
		int32_t minY{ int32_t( clientSize->y ) };
		int32_t maxX{ -minX };
		int32_t maxY{ -maxX };

		for ( auto & it : m_controls )
		{
			auto control = it.first;
			minX = std::min( minX
				, control->getPosition().x() );
			minY = std::min( minY
				, control->getPosition().y() );
			maxX = std::max( maxX
				, control->getPosition().x() + int32_t( control->getSize()->x ) );
			maxY = std::max( maxY
				, control->getPosition().y() + int32_t( control->getSize()->y ) );
		}

		maxX -= minX;
		maxY -= minY;
		updateTotalSize( { uint32_t( maxX ), uint32_t( maxY ) } );
	}

	void ScrollableCtrl::doScrollContent()
	{
		if ( isLayoutControl( m_target )
			&& static_cast< LayoutControl const & >( m_target ).getLayout() )
		{
			static_cast< LayoutControl const & >( m_target ).getLayout()->markDirty();
			return;
		}

		m_updating = true;
		auto pos{ getScrollPosition() };

		for ( auto & it : m_controls )
		{
			auto position = pos + it.first->getPosition();
			it.first->setPosition( { position->x, position->y } );
		}

		onScrollContent( pos );
		m_updating = false;
	}

	//************************************************************************************************
}

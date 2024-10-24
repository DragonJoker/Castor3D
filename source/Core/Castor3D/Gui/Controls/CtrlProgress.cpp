#include "Castor3D/Gui/Controls/CtrlProgress.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Gui/Controls/CtrlPanel.hpp"
#include "Castor3D/Gui/Controls/CtrlStatic.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Graphics/Font.hpp>

CU_ImplementSmartPtr( castor3d, ProgressCtrl )

namespace castor3d
{
	ProgressCtrl::ProgressCtrl( SceneRPtr scene
		, castor::String const & name
		, ProgressStyle * style
		, ControlRPtr parent )
		: Control{ Type
			, scene
			, name
			, style
			, parent
			, castor::Position{}
			, castor::Size{}
			, ControlFlagType( ProgressFlag::eLeftRight | ProgressFlag::eHasTitle )
			, true }
	{
		setBorderSize( castor::Point4ui{} );
		auto title = m_scene
			? m_scene->addNewOverlay( getName() + cuT( "/Title" )
				, getEngine()
				, OverlayType::eText
				, nullptr
				, &getBackgroundOverlay() )->getTextOverlay()
			: getEngine().addNewOverlay( getName() + cuT( "/Title" )
				, getEngine()
				, OverlayType::eText
				, nullptr
				, &getBackgroundOverlay() )->getTextOverlay();
		m_title = title;
		title->setVAlign( VAlign::eTop );
		title->setPixelSize( getClientSize() );
		title->setVisible( true );

		auto & manager = *getEngine().getControlsManager();
		m_container = manager.registerControlT( castor::makeUnique< PanelCtrl >( m_scene
			, cuT( "Container" )
			, &style->getContainerStyle()
			, this ) );
		m_container->setVisible( true );

		m_progress = manager.registerControlT( castor::makeUnique< PanelCtrl >( m_scene
			, cuT( "Progress" )
			, &style->getProgressStyle()
			, m_container ) );
		m_progress->setVisible( true );

		m_text = manager.registerControlT( castor::makeUnique< StaticCtrl >( m_scene
			, cuT( "Label" )
			, &style->getLabelStyle()
			, m_container ) );
		m_text->setHAlign( HAlign::eCenter );
		m_text->setVAlign( VAlign::eCenter );

		setStyle( style );
		doUpdateFlags();
	}

	ProgressCtrl::~ProgressCtrl()noexcept
	{
		auto & manager = *getEngine().getControlsManager();
		manager.unregisterControl( *m_text );
		manager.unregisterControl( *m_progress );
		manager.unregisterControl( *m_container );

		if ( m_title )
		{
			if ( m_scene )
			{
				m_scene->removeOverlay( getName() + cuT( "/Title" ), true );
			}
			else
			{
				getEngine().removeOverlay( getName() + cuT( "/Title" ), true );
			}
		}
	}

	void ProgressCtrl::setTitle( castor::U32String value )
	{
		if ( hasTitle() )
		{
			if ( auto title = m_title )
			{
				title->setCaption( castor::move( value ) );
			}
		}
	}

	void ProgressCtrl::setRange( castor::Range< int32_t > const & range )
	{
		m_value.updateRange( range );
		doUpdateProgress();
	}

	void ProgressCtrl::setProgress( int32_t value )
	{
		m_value = value;
		doUpdateProgress();
	}

	void ProgressCtrl::setContainerBorderSize( castor::Point4ui const & value )
	{
		if ( m_container )
		{
			m_container->setBorderSize( value );
		}
	}

	void ProgressCtrl::setBarBorderSize( castor::Point4ui const & value )
	{
		if ( m_progress )
		{
			m_progress->setBorderSize( value );
		}
	}

	void ProgressCtrl::setLeftToRight()noexcept
	{
		m_flags &= ~( ProgressFlag::eLeftRight
			| ProgressFlag::eRightLeft
			| ProgressFlag::eTopBottom
			| ProgressFlag::eBottomTop );
		addFlag( ProgressFlag::eLeftRight );
	}

	void ProgressCtrl::setRightToLeft()noexcept
	{
		m_flags &= ~( ProgressFlag::eLeftRight
			| ProgressFlag::eRightLeft
			| ProgressFlag::eTopBottom
			| ProgressFlag::eBottomTop );
		addFlag( ProgressFlag::eRightLeft );
	}

	void ProgressCtrl::setTopToBottom()noexcept
	{
		m_flags &= ~( ProgressFlag::eLeftRight
			| ProgressFlag::eRightLeft
			| ProgressFlag::eTopBottom
			| ProgressFlag::eBottomTop );
		addFlag( ProgressFlag::eTopBottom );
	}

	void ProgressCtrl::setBottomToTop()noexcept
	{
		m_flags &= ~( ProgressFlag::eLeftRight
			| ProgressFlag::eRightLeft
			| ProgressFlag::eTopBottom
			| ProgressFlag::eBottomTop );
		addFlag( ProgressFlag::eBottomTop );
	}

	void ProgressCtrl::showTitle( bool show )noexcept
	{
		if ( show )
		{
			addFlag( ProgressFlag::eHasTitle );
		}
		else
		{
			removeFlag( ProgressFlag::eHasTitle );
		}

		if ( auto title = m_title )
		{
			title->setVisible( show );
		}
	}

	void ProgressCtrl::doCreate()
	{
		CU_Require( getControlsManager() );
		auto & manager = *getControlsManager();
		doUpdatePosSize();
		manager.create( m_text );
		manager.create( m_progress );
		manager.create( m_container );
	}

	void ProgressCtrl::doDestroy()
	{
		CU_Require( getControlsManager() );
		auto & manager = *getControlsManager();

		if ( m_container )
		{
			manager.destroy( m_container );
		}

		if ( m_progress )
		{
			manager.destroy( m_progress );
		}

		if ( m_text )
		{
			manager.destroy( m_text );
		}
	}

	void ProgressCtrl::doUpdateStyle()
	{
		auto & style = getStyle();
		m_container->setStyle( &style.getContainerStyle() );

		if ( auto progress = m_progress )
		{
			progress->setStyle( &style.getProgressStyle() );
		}

		if ( hasTitle() )
		{
			if ( auto title = m_title )
			{
				title->setFont( style.getTitleFontName() );
				title->setMaterial( style.getTitleMaterial() );
			}
		}

		if ( auto text = m_text )
		{
			text->setStyle( &style.getLabelStyle() );
		}
	}

	void ProgressCtrl::doUpdateFlags()
	{
		if ( isVertical() )
		{
			if ( hasTitle() )
			{
				if ( auto title = m_title )
				{
					title->setVisible( false );
				}
			}

			if ( auto text = m_text )
			{
				text->setVisible( false );
			}
		}
		else if ( isRightToLeft() )
		{
			if ( hasTitle() )
			{
				if ( auto title = m_title )
				{
					title->setHAlign( HAlign::eRight );
				}
			}
		}
		else if ( hasTitle() )
		{
			if ( auto title = m_title )
			{
				title->setHAlign( HAlign::eLeft );
			}
		}
	}

	void ProgressCtrl::doUpdateZIndex( uint32_t & index )
	{
		if ( hasTitle() )
		{
			if ( auto title = m_title )
			{
				title->setOrder( index, 0u );
				++index;
			}
		}
	}

	void ProgressCtrl::doAdjustZIndex( uint32_t offset )
	{
		if ( hasTitle() )
		{
			if ( auto title = m_title )
			{
				title->setOrder( title->getLevel() + offset, 0u );
			}
		}
	}

	void ProgressCtrl::doSetBorderSize( castor::Point4ui const & value )
	{
		doUpdatePosSize();
	}

	void ProgressCtrl::doSetPosition( castor::Position const & value )
	{
	}

	void ProgressCtrl::doSetSize( castor::Size const & value )
	{
		doUpdatePosSize();
	}

	void ProgressCtrl::doSetCaption( castor::U32String const & caption )
	{
		if ( auto text = m_text )
		{
			text->setCaption( caption );
		}
	}

	void ProgressCtrl::doSetVisible( bool value )
	{
		if ( hasTitle() )
		{
			if ( auto title = m_title )
			{
				title->setVisible( value && !isVertical() );
			}
		}

		if ( auto container = m_container )
		{
			container->setVisible( value );
		}

		if ( auto progress = m_progress )
		{
			progress->setVisible( value );
		}

		if ( auto text = m_text )
		{
			text->setVisible( value && !isVertical() );
		}
	}

	void ProgressCtrl::doUpdatePosSize()
	{
		auto clientOffset = getClientOffset();
		auto clientSize = getClientSize();
		int32_t top{ clientOffset.y() };
		int32_t left{ clientOffset.x() };

		if ( !isVertical()
			&& hasTitle() )
		{
			if ( auto title = m_title )
			{
				auto titleHeight = ( title->getFontTexture() && title->getFontTexture()->getFont() )
					? title->getFontTexture()->getFont()->getHeight()
					: 0u;
				auto titleOffset = titleHeight / 2u;
				titleHeight += titleOffset;
				title->setPixelPosition( clientOffset );
				title->setPixelSize( { clientSize->x, titleHeight } );
				top += titleHeight;
			}
		}

		if ( auto container = m_container )
		{
			container->setPosition( { left, top } );
			container->setSize( { uint32_t( std::max( 0, int32_t( clientSize->x ) - left ) )
				, uint32_t( std::max( 0, int32_t( clientSize->y ) - top ) ) } );

			if ( !isVertical() )
			{
				if ( auto text = m_text )
				{
					text->setPosition( container->getClientOffset() );
					text->setSize( container->getClientSize() );
				}
			}
		}

		doUpdateProgress();
	}

	void ProgressCtrl::doUpdateProgress()
	{
		if ( m_progress && m_container )
		{
			auto clientSize = m_container->getClientSize();
			auto clientOffset = m_container->getClientOffset();
			uint32_t width{ clientSize->x };
			uint32_t height{ clientSize->y };
			int32_t left{};
			int32_t top{};

			if ( isVertical() )
			{
				height = uint32_t( double( clientSize->y ) * m_value.percent() );

				if ( isBottomToTop() )
				{
					top = int32_t( clientSize->y - height );
				}
			}
			else
			{
				width = uint32_t( double( clientSize->x ) * m_value.percent() );

				if ( isRightToLeft() )
				{
					left = int32_t( clientSize->x - width );
				}
			}

			m_progress->setPosition( { clientOffset.x() + left, clientOffset.y() + top } );
			m_progress->setSize( { width, height } );
		}
	}
}

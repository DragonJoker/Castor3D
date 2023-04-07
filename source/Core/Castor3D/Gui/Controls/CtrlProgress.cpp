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

CU_ImplementCUSmartPtr( castor3d, ProgressCtrl )

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
			, ControlFlagType( ProgressFlag::eLeftRight )
			, true }
		, m_value{ castor::makeRangedValue( 0, 0, 100 ) }
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

		auto text = m_scene
			? m_scene->addNewOverlay( getName() + cuT( "/Text" )
				, getEngine()
				, OverlayType::eText
				, nullptr
				, &m_container->getBackgroundOverlay() )->getTextOverlay()
			: getEngine().addNewOverlay( getName() + cuT( "/Text" )
				, getEngine()
				, OverlayType::eText
				, nullptr
				, &m_container->getBackgroundOverlay() )->getTextOverlay();
		m_text = text;
		text->setPixelSize( getClientSize() );
		text->setVisible( true );
		text->setHAlign( HAlign::eCenter );
		text->setVAlign( VAlign::eCenter );

		setStyle( style );
		doUpdateFlags();
	}

	ProgressCtrl::~ProgressCtrl()noexcept
	{
		if ( auto overlay = m_text )
		{
			if ( m_scene )
			{
				m_scene->removeOverlay( getName() + cuT( "/Text" ), true );
			}
			else
			{
				getEngine().removeOverlay( getName() + cuT( "/Text" ), true );
			}
		}

		auto & manager = *getEngine().getControlsManager();
		manager.unregisterControl( *m_progress );
		manager.unregisterControl( *m_container );

		if ( auto overlay = m_title )
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
		if ( auto title = m_title )
		{
			title->setCaption( value );
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

	void ProgressCtrl::setContainerBorderSize( castor::Point4ui value )
	{
		if ( m_container )
		{
			m_container->setBorderSize( value );
		}
	}

	void ProgressCtrl::setBarBorderSize( castor::Point4ui value )
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

	void ProgressCtrl::doCreate()
	{
		CU_Require( getControlsManager() );
		auto & manager = *getControlsManager();
		doUpdatePosSize();
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
	}

	void ProgressCtrl::doUpdateStyle()
	{
		auto & style = getStyle();
		m_container->setStyle( &style.getContainerStyle() );
		m_progress->setStyle( &style.getProgressStyle() );

		if ( auto title = m_title )
		{
			title->setFont( style.getTitleFontName() );
			title->setMaterial( style.getTitleMaterial() );
		}

		if ( auto text = m_text )
		{
			text->setFont( style.getTextFontName() );
			text->setMaterial( style.getTextMaterial() );
		}
	}

	void ProgressCtrl::doUpdateFlags()
	{
		if ( isVertical() )
		{
			if ( auto title = m_title )
			{
				title->setVisible( false );
			}

			if ( auto text = m_text )
			{
				text->setVisible( false );
			}
		}
		else if ( isRightToLeft() )
		{
			if ( auto title = m_title )
			{
				title->setHAlign( HAlign::eRight );
			}
		}
		else
		{
			if ( auto title = m_title )
			{
				title->setHAlign( HAlign::eLeft );
			}
		}
	}

	void ProgressCtrl::doUpdateZIndex( uint32_t & index )
	{
		if ( auto title = m_title )
		{
			title->setOrder( index++, 0u );
		}

		if ( auto text = m_text )
		{
			text->setOrder( index++, 0u );
		}
	}

	void ProgressCtrl::doAdjustZIndex( uint32_t offset )
	{
		if ( auto title = m_title )
		{
			title->setOrder( title->getLevel() + offset, 0u );
		}

		if ( auto text = m_text )
		{
			text->setOrder( text->getLevel() + offset, 0u );
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
		if ( auto title = m_title )
		{
			title->setVisible( value && !isVertical() );
		}

		if ( m_container )
		{
			m_container->setVisible( value );
		}

		if ( m_progress )
		{
			m_progress->setVisible( value );
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

		if ( !isVertical() )
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

		if ( m_container )
		{
			m_container->setPosition( { left, top } );
			m_container->setSize( { uint32_t( std::max( 0, int32_t( clientSize->x ) - left ) )
				, uint32_t( std::max( 0, int32_t( clientSize->y ) - top ) ) } );

			if ( !isVertical() )
			{
				if ( auto text = m_text )
				{
					text->setPixelPosition( m_container->getClientOffset() );
					text->setPixelSize( m_container->getClientSize() );
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

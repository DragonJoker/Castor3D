#include "Castor3D/Gui/Controls/CtrlButton.hpp"

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

CU_ImplementSmartPtr( castor3d, ButtonCtrl )

namespace castor3d
{
	ButtonCtrl::ButtonCtrl( SceneRPtr scene
		, castor::String const & name
		, ButtonStyleRPtr style
		, ControlRPtr parent )
		: ButtonCtrl{ scene
			, name
			, style
			, parent
			, castor::U32String{}
			, castor::Position{}
			, castor::Size{}
			, 0u
			, true }
	{
	}

	ButtonCtrl::ButtonCtrl( SceneRPtr scene
		, castor::String const & name
		, ButtonStyleRPtr style
		, ControlRPtr parent
		, castor::U32String const & caption
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
		, m_caption{ caption }
		, m_onEnable{ EventHandler::onEnable.connect( [this]( bool v )
			{
				auto const & mystyle = getStyle();

				if ( v )
				{
					m_text->setMaterial( mystyle.getTextMaterial() );
					setBackgroundMaterial( mystyle.getBackgroundMaterial() );
					setBackgroundBorderMaterial( mystyle.getForegroundMaterial() );
				}
				else
				{
					m_text->setMaterial( mystyle.getDisabledTextMaterial() );
					setBackgroundMaterial( mystyle.getDisabledBackgroundMaterial() );
					setBackgroundBorderMaterial( mystyle.getDisabledForegroundMaterial() );
				}
			} ) }
	{
		setBorderSize( castor::Point4ui{ 1, 1, 1, 1 } );
		setHAlign( HAlign::eCenter );
		setVAlign( VAlign::eCenter );

		auto text = m_scene
			? m_scene->addNewOverlay( getName() + cuT( "/Text" )
				, getEngine()
				, OverlayType::eText
				, nullptr
				, &getBackgroundOverlay() )->getTextOverlay()
			: getEngine().addNewOverlay( getName() + cuT( "/Text" )
				, getEngine()
				, OverlayType::eText
				, nullptr
				, &getBackgroundOverlay() )->getTextOverlay();

		text->setPixelPosition( getClientOffset() );
		text->setPixelSize( getClientSize() );
		text->setCaption( m_caption );
		text->setVisible( visible );
		m_text = text;

		setStyle( style );
	}

	ButtonCtrl::~ButtonCtrl()noexcept
	{
		m_onEnable = {};

		if ( m_text )
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
	}

	void ButtonCtrl::setHAlign( HAlign align )
	{
		m_flags &= ~ControlFlagType( StaticFlag::eHAlignCenter
			| StaticFlag::eHAlignRight
			| StaticFlag::eHAlignLeft );

		switch ( align )
		{
		case HAlign::eLeft:
			addFlag( StaticFlag::eHAlignLeft );
			break;
		case HAlign::eCenter:
			addFlag( StaticFlag::eHAlignCenter );
			break;
		case HAlign::eRight:
			addFlag( StaticFlag::eHAlignRight );
			break;
		default:
			break;
		}
	}

	void ButtonCtrl::setVAlign( VAlign align )
	{
		m_flags &= ~ControlFlagType( StaticFlag::eVAlignCenter
			| StaticFlag::eVAlignTop
			| StaticFlag::eVAlignBottom );

		switch ( align )
		{
		case VAlign::eTop:
			addFlag( StaticFlag::eVAlignTop );
			break;
		case VAlign::eCenter:
			addFlag( StaticFlag::eVAlignCenter );
			break;
		case VAlign::eBottom:
			addFlag( StaticFlag::eVAlignBottom );
			break;
		default:
			break;
		}
	}

	HAlign ButtonCtrl::getHAlign()const
	{
		if ( castor::checkFlag( getFlags(), StaticFlag::eHAlignCenter ) )
		{
			return HAlign::eCenter;
		}

		if ( castor::checkFlag( getFlags(), StaticFlag::eHAlignRight ) )
		{
			return HAlign::eRight;
		}

		return HAlign::eLeft;
	}

	VAlign ButtonCtrl::getVAlign()const
	{
		if ( castor::checkFlag( getFlags(), StaticFlag::eVAlignCenter ) )
		{
			return VAlign::eCenter;
		}

		if ( castor::checkFlag( getFlags(), StaticFlag::eVAlignBottom ) )
		{
			return VAlign::eBottom;
		}

		return VAlign::eTop;
	}

	void ButtonCtrl::doUpdateStyle()
	{
		auto const & style = getStyle();

		if ( auto text = m_text )
		{
			text->setFont( style.getFontName() );
		}
	}

	void ButtonCtrl::doCreate()
	{
		setBackgroundBorderPosition( BorderPosition::eInternal );

		if ( auto text = m_text )
		{
			auto const & style = getStyle();
			text->setMaterial( style.getTextMaterial() );

			if ( !text->getFontTexture() || !text->getFontTexture()->getFont() )
			{
				text->setFont( style.getFontName() );
			}
		}

		getControlsManager()->connectEvents( *this );
	}

	void ButtonCtrl::doDestroy()
	{
		getControlsManager()->disconnectEvents( *this );
	}

	void ButtonCtrl::doSetPosition( castor::Position const & value )
	{
		if ( auto text = m_text )
		{
			text->setPixelPosition( getClientOffset() );
		}
	}

	void ButtonCtrl::doSetSize( castor::Size const & value )
	{
		if ( auto text = m_text )
		{
			text->setPixelSize( getClientSize() );
		}
	}

	void ButtonCtrl::doSetBorderSize( castor::Point4ui const & value )
	{
		if ( auto text = m_text )
		{
			text->setPixelPosition( getClientOffset() );
			text->setPixelSize( getClientSize() );
		}
	}

	void ButtonCtrl::doSetCaption( castor::U32String const & value )
	{
		m_caption = value;

		if ( auto text = m_text )
		{
			text->setCaption( value );
		}
	}

	void ButtonCtrl::doSetVisible( bool visible )
	{
		if ( auto text = m_text )
		{
			text->setVisible( visible );
		}
	}

	void ButtonCtrl::doUpdateFlags()
	{
		if ( auto text = m_text )
		{
			text->setHAlign( getHAlign() );
			text->setVAlign( getVAlign() );
		}
	}

	void ButtonCtrl::doOnMouseEnter( MouseEvent const & event )
	{
		auto const & style = getStyle();
		m_text->setMaterial( style.getHighlightedTextMaterial() );
		setBackgroundMaterial( style.getHighlightedBackgroundMaterial() );
		setBackgroundBorderMaterial( style.getHighlightedForegroundMaterial() );
	}

	void ButtonCtrl::doOnMouseButtonDown( MouseEvent const & event )
	{
		if ( event.getButton() == MouseButton::eLeft )
		{
			auto const & style = getStyle();
			m_text->setMaterial( style.getPushedTextMaterial() );
			setBackgroundMaterial( style.getPushedBackgroundMaterial() );
			setBackgroundBorderMaterial( style.getPushedForegroundMaterial() );
		}
	}

	void ButtonCtrl::doOnMouseButtonUp( MouseEvent const & event )
	{
		if ( event.getButton() == MouseButton::eLeft )
		{
			auto const & style = getStyle();
			m_text->setMaterial( style.getHighlightedTextMaterial() );
			setBackgroundMaterial( style.getHighlightedBackgroundMaterial() );
			setBackgroundBorderMaterial( style.getHighlightedForegroundMaterial() );

			m_signals[size_t( ButtonEvent::eClicked )]();
		}
	}

	void ButtonCtrl::doOnMouseLeave( MouseEvent const & event )
	{
		auto const & style = getStyle();
		m_text->setMaterial( style.getTextMaterial() );
		setBackgroundMaterial( style.getBackgroundMaterial() );
		setBackgroundBorderMaterial( style.getForegroundMaterial() );
	}

	void ButtonCtrl::doUpdateZIndex( uint32_t & index )
	{
		if ( auto text = m_text )
		{
			text->setOrder( index++, 0u );
		}
	}

	void ButtonCtrl::doAdjustZIndex( uint32_t offset )
	{
		if ( auto text = m_text )
		{
			text->setOrder( text->getLevel() + offset, 0u );
		}
	}
}

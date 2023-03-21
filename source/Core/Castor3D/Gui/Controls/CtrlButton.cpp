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

CU_ImplementCUSmartPtr( castor3d, ButtonCtrl )

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
			, castor::String{}
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
		, castor::String const & caption
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
				auto & mystyle = getStyle();

				if ( v )
				{
					m_text.lock()->setMaterial( mystyle.getTextMaterial() );
					setBackgroundMaterial( mystyle.getBackgroundMaterial() );
					setBackgroundBorderMaterial( mystyle.getForegroundMaterial() );
				}
				else
				{
					m_text.lock()->setMaterial( mystyle.getDisabledTextMaterial() );
					setBackgroundMaterial( mystyle.getDisabledBackgroundMaterial() );
					setBackgroundBorderMaterial( mystyle.getDisabledForegroundMaterial() );
				}
			} ) }
	{
		setBorderSize( castor::Point4ui{ 1, 1, 1, 1 } );
		setHAlign( HAlign::eCenter );
		setVAlign( VAlign::eCenter );

		auto text = m_scene
			? m_scene->addNewOverlay( cuT( "T_CtrlButton_[" ) + getName() + cuT( "]" )
				, getEngine()
				, OverlayType::eText
				, nullptr
				, &getBackgroundOverlay() ).lock()->getTextOverlay()
			: getEngine().addNewOverlay( cuT( "T_CtrlButton_[" ) + getName() + cuT( "]" )
				, getEngine()
				, OverlayType::eText
				, nullptr
				, &getBackgroundOverlay() ).lock()->getTextOverlay();
		text->setPixelSize( getSize() );
		text->setCaption( castor::string::toU32String( m_caption ) );
		text->setVisible( visible );
		m_text = text;

		setStyle( style );
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
		auto & style = getStyle();

		if ( auto text = m_text.lock() )
		{
			text->setFont( style.getFontName() );
		}
	}

	void ButtonCtrl::doCreate()
	{
		setBackgroundBorderPosition( BorderPosition::eInternal );

		auto & style = getStyle();
		auto text = m_text.lock();
		text->setMaterial( style.getTextMaterial() );

		if ( !text->getFontTexture() || !text->getFontTexture()->getFont() )
		{
			text->setFont( style.getFontName() );
		}

		getControlsManager()->connectEvents( *this );
	}

	void ButtonCtrl::doDestroy()
	{
		getControlsManager()->disconnectEvents( *this );
	}

	void ButtonCtrl::doSetPosition( castor::Position const & value )
	{
		if ( auto text = m_text.lock() )
		{
			text->setPixelPosition( castor::Position{} );
			text.reset();
		}
	}

	void ButtonCtrl::doSetSize( castor::Size const & value )
	{
		if ( auto text = m_text.lock() )
		{
			text->setPixelSize( value );
		}
	}

	void ButtonCtrl::doSetCaption( castor::String const & value )
	{
		m_caption = value;

		if ( auto text = m_text.lock() )
		{
			text->setCaption( castor::string::toU32String( value ) );
		}
	}

	void ButtonCtrl::doSetVisible( bool visible )
	{
		if ( auto text = m_text.lock() )
		{
			text->setVisible( visible );
		}
	}

	void ButtonCtrl::doUpdateFlags()
	{
		if ( auto text = m_text.lock() )
		{
			text->setHAlign( getHAlign() );
			text->setVAlign( getVAlign() );
		}
	}

	void ButtonCtrl::doOnMouseEnter( MouseEvent const & event )
	{
		auto & style = getStyle();
		m_text.lock()->setMaterial( style.getHighlightedTextMaterial() );
		setBackgroundMaterial( style.getHighlightedBackgroundMaterial() );
		setBackgroundBorderMaterial( style.getHighlightedForegroundMaterial() );
	}

	void ButtonCtrl::doOnMouseButtonDown( MouseEvent const & event )
	{
		if ( event.getButton() == MouseButton::eLeft )
		{
			auto & style = getStyle();
			m_text.lock()->setMaterial( style.getPushedTextMaterial() );
			setBackgroundMaterial( style.getPushedBackgroundMaterial() );
			setBackgroundBorderMaterial( style.getPushedForegroundMaterial() );
		}
	}

	void ButtonCtrl::doOnMouseButtonUp( MouseEvent const & event )
	{
		if ( event.getButton() == MouseButton::eLeft )
		{
			auto & style = getStyle();
			m_text.lock()->setMaterial( style.getHighlightedTextMaterial() );
			setBackgroundMaterial( style.getHighlightedBackgroundMaterial() );
			setBackgroundBorderMaterial( style.getHighlightedForegroundMaterial() );

			m_signals[size_t( ButtonEvent::eClicked )]();
		}
	}

	void ButtonCtrl::doOnMouseLeave( MouseEvent const & event )
	{
		auto & style = getStyle();
		m_text.lock()->setMaterial( style.getTextMaterial() );
		setBackgroundMaterial( style.getBackgroundMaterial() );
		setBackgroundBorderMaterial( style.getForegroundMaterial() );
	}

	void ButtonCtrl::doUpdateZIndex( uint32_t & index )
	{
		if ( auto text = m_text.lock() )
		{
			text->setOrder( index++, 0u );
		}
	}
}

#include "CastorGui/Controls/CtrlButton.hpp"

#include "CastorGui/ControlsManager.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Cache/OverlayCache.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Overlay/Overlay.hpp>

#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Overlay/TextOverlay.hpp>

#include <CastorUtils/Graphics/Font.hpp>

namespace CastorGui
{
	ButtonCtrl::ButtonCtrl( castor::String const & name
			, ButtonStyleRPtr style
		, ControlRPtr parent
		, uint32_t id )
		: ButtonCtrl{ name
			, style
			, parent
			, id
			, castor::String{}
			, castor::Position{}
			, castor::Size{}
			, 0u
			, true }
	{
	}

	ButtonCtrl::ButtonCtrl( castor::String const & name
		, ButtonStyleRPtr style
		, ControlRPtr parent
		, uint32_t id
		, castor::String const & caption
		, castor::Position const & position
		, castor::Size const & size
		, uint32_t flags
		, bool visible )
		: Control{ Type
			, name
			, style
			, parent
			, id
			, position
			, size
			, flags
			, visible }
		, m_caption{ caption }
	{
		setBackgroundBorders( castor::Rectangle{ 1, 1, 1, 1 } );
		EventHandler::connect( castor3d::MouseEventType::eEnter
			, [this]( castor3d::MouseEvent const & event )
			{
				onMouseEnter( event );
			} );
		EventHandler::connect( castor3d::MouseEventType::eLeave
			, [this]( castor3d::MouseEvent const & event )
			{
				onMouseLeave( event );
			} );
		EventHandler::connect( castor3d::MouseEventType::ePushed
			, [this]( castor3d::MouseEvent const & event )
			{
				onMouseButtonDown( event );
			} );
		EventHandler::connect( castor3d::MouseEventType::eReleased
			, [this]( castor3d::MouseEvent const & event )
			{
				onMouseButtonUp( event );
			} );

		auto text = getEngine().getOverlayCache().add( cuT( "T_CtrlButton_" ) + castor::string::toString( getId() )
			, getEngine()
			, castor3d::OverlayType::eText
			, nullptr
			, &getBackground()->getOverlay() ).lock()->getTextOverlay();
		text->setPixelSize( getSize() );
		text->setHAlign( castor3d::HAlign::eCenter );
		text->setVAlign( castor3d::VAlign::eCenter );
		text->setCaption( m_caption );
		text->setVisible( visible );
		m_text = text;

		doUpdateStyle();
	}

	void ButtonCtrl::setHAlign( castor3d::HAlign align )
	{
		if ( auto text = m_text.lock() )
		{
			text->setHAlign( align );
		}
	}

	void ButtonCtrl::setVAlign( castor3d::VAlign align )
	{
		if ( auto text = m_text.lock() )
		{
			text->setVAlign( align );
		}
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
		auto bg = getBackground();

		if ( !bg )
		{
			CU_Exception( "No background set" );
		}

		bg->setBorderPosition( castor3d::BorderPosition::eInternal );

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
			text->setCaption( value );
		}
	}

	void ButtonCtrl::doSetVisible( bool visible )
	{
		if ( auto text = m_text.lock() )
		{
			text->setVisible( visible );
		}
	}

	void ButtonCtrl::onMouseEnter( castor3d::MouseEvent const & event )
	{
		auto & style = getStyle();
		m_text.lock()->setMaterial( style.getHighlightedTextMaterial() );

		if ( auto panel = getBackground() )
		{
			panel->setMaterial( style.getHighlightedBackgroundMaterial() );
			panel->setBorderMaterial( style.getHighlightedForegroundMaterial() );
		}
	}

	void ButtonCtrl::onMouseLeave( castor3d::MouseEvent const & event )
	{
		auto & style = getStyle();
		m_text.lock()->setMaterial( style.getTextMaterial() );

		if ( auto panel = getBackground() )
		{
			panel->setMaterial( style.getBackgroundMaterial() );
			panel->setBorderMaterial( style.getForegroundMaterial() );
			panel.reset();
		}
	}

	void ButtonCtrl::onMouseButtonDown( castor3d::MouseEvent const & event )
	{
		if ( event.getButton() == castor3d::MouseButton::eLeft )
		{
			auto & style = getStyle();
			m_text.lock()->setMaterial( style.getPushedTextMaterial() );

			if ( auto panel = getBackground() )
			{
				panel->setMaterial( style.getPushedBackgroundMaterial() );
				panel->setBorderMaterial( style.getPushedForegroundMaterial() );
				panel.reset();
			}
		}
	}

	void ButtonCtrl::onMouseButtonUp( castor3d::MouseEvent const & event )
	{
		if ( event.getButton() == castor3d::MouseButton::eLeft )
		{
			auto & style = getStyle();
			m_text.lock()->setMaterial( style.getHighlightedTextMaterial() );

			if ( auto panel = getBackground() )
			{
				panel->setMaterial( style.getHighlightedBackgroundMaterial() );
				panel->setBorderMaterial( style.getHighlightedForegroundMaterial() );
				panel.reset();
			}

			m_signals[size_t( ButtonEvent::eClicked )]();
		}
	}
}

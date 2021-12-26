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

using namespace castor;
using namespace castor3d;

namespace CastorGui
{
	ButtonCtrl::ButtonCtrl( String const & p_name
		, Engine & engine
		, ControlRPtr p_parent
		, uint32_t p_id )
		: ButtonCtrl( p_name
			, engine
			, p_parent
			, p_id
			, cuT( "" )
			, Position()
			, Size()
			, 0
			, true )
	{
	}

	ButtonCtrl::ButtonCtrl( String const & p_name
		, Engine & engine
		, ControlRPtr p_parent
		, uint32_t p_id
		, String const & p_caption
		, Position const & p_position
		, Size const & p_size
		, uint32_t p_flags
		, bool p_visible )
		: Control( ControlType::eButton
			, p_name
			, engine
			, p_parent
			, p_id
			, p_position
			, p_size
			, p_flags
			, p_visible )
		, m_caption( p_caption )
	{
		setBackgroundBorders( castor::Rectangle( 1, 1, 1, 1 ) );
		EventHandler::connect( MouseEventType::eEnter, [this]( MouseEvent const & p_event )
		{
			onMouseEnter( p_event );
		} );
		EventHandler::connect( MouseEventType::eLeave, [this]( MouseEvent const & p_event )
		{
			onMouseLeave( p_event );
		} );
		EventHandler::connect( MouseEventType::ePushed, [this]( MouseEvent const & p_event )
		{
			onMouseButtonDown( p_event );
		} );
		EventHandler::connect( MouseEventType::eReleased, [this]( MouseEvent const & p_event )
		{
			onMouseButtonUp( p_event );
		} );

		TextOverlaySPtr text = getEngine().getOverlayCache().add( cuT( "T_CtrlButton_" ) + string::toString( getId() )
			, getEngine()
			, OverlayType::eText
			, nullptr
			, &getBackground()->getOverlay() ).lock()->getTextOverlay();
		text->setPixelSize( getSize() );
		text->setHAlign( HAlign::eCenter );
		text->setVAlign( VAlign::eCenter );
		text->setCaption( m_caption );
		text->setVisible( p_visible );
		m_text = text;
	}

	void ButtonCtrl::setTextMaterial( MaterialRPtr p_material )
	{
		m_textMaterial = p_material;
	}

	void ButtonCtrl::setHighlightedBackgroundMaterial( MaterialRPtr p_material )
	{
		m_highlightedBackgroundMaterial = p_material;
	}

	void ButtonCtrl::setHighlightedForegroundMaterial( MaterialRPtr p_material )
	{
		m_highlightedForegroundMaterial = p_material;
	}

	void ButtonCtrl::setHighlightedTextMaterial( MaterialRPtr p_material )
	{
		m_highlightedTextMaterial = p_material;
	}

	void ButtonCtrl::setPushedBackgroundMaterial( MaterialRPtr p_material )
	{
		m_pushedBackgroundMaterial = p_material;
	}

	void ButtonCtrl::setPushedForegroundMaterial( MaterialRPtr p_material )
	{
		m_pushedForegroundMaterial = p_material;
	}

	void ButtonCtrl::setPushedTextMaterial( MaterialRPtr p_material )
	{
		m_pushedTextMaterial = p_material;
	}

	void ButtonCtrl::setFont( castor::String const & p_font )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setFont( p_font );
		}
	}

	void ButtonCtrl::setHAlign( HAlign p_align )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setHAlign( p_align );
		}
	}

	void ButtonCtrl::setVAlign( VAlign p_align )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setVAlign( p_align );
		}
	}

	void ButtonCtrl::doCreate()
	{
		auto bg = getBackground();

		if ( !bg )
		{
			CU_Exception( "No background set" );
		}

		bg->setBorderPosition( BorderPosition::eInternal );

		if ( !getBackgroundMaterial() )
		{
			setBackgroundMaterial( getEngine().getMaterialCache().find( cuT( "Black" ) ).lock().get() );
		}

		if ( !getForegroundMaterial() )
		{
			setForegroundMaterial( getEngine().getMaterialCache().find( cuT( "White" ) ).lock().get() );
		}

		if ( !m_textMaterial )
		{
			m_textMaterial = getForegroundMaterial();
		}

		if ( !m_highlightedBackgroundMaterial )
		{
			m_highlightedBackgroundMaterial = doCreateMaterial( getBackgroundMaterial(), 0.1f );
		}

		if ( !m_highlightedForegroundMaterial )
		{
			m_highlightedForegroundMaterial = doCreateMaterial( getForegroundMaterial(), -0.1f );
		}

		if ( !m_highlightedTextMaterial )
		{
			m_highlightedTextMaterial = doCreateMaterial( getTextMaterial(), -0.1f );
		}

		if ( !m_pushedBackgroundMaterial )
		{
			m_pushedBackgroundMaterial = doCreateMaterial( getBackgroundMaterial(), 0.1f );
		}

		if ( !m_pushedForegroundMaterial )
		{
			m_pushedForegroundMaterial = doCreateMaterial( getForegroundMaterial(), -0.1f );
		}

		if ( !m_pushedTextMaterial )
		{
			m_pushedTextMaterial = doCreateMaterial( getTextMaterial(), -0.1f );
		}

		TextOverlaySPtr text = m_text.lock();
		text->setMaterial( m_textMaterial );

		if ( !text->getFontTexture() || !text->getFontTexture()->getFont() )
		{
			text->setFont( getControlsManager()->getDefaultFont().lock()->getName() );
		}

		getControlsManager()->connectEvents( *this );
	}

	void ButtonCtrl::doDestroy()
	{
		getControlsManager()->disconnectEvents( *this );
	}

	void ButtonCtrl::doSetPosition( Position const & p_value )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setPixelPosition( Position() );
			text.reset();
		}
	}

	void ButtonCtrl::doSetSize( Size const & p_value )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setPixelSize( p_value );
		}
	}

	void ButtonCtrl::doSetBackgroundMaterial( MaterialRPtr p_material )
	{
		m_highlightedBackgroundMaterial = doCreateMaterial( p_material, 0.1f );
	}

	void ButtonCtrl::doSetForegroundMaterial( MaterialRPtr p_material )
	{
		m_highlightedForegroundMaterial = doCreateMaterial( p_material, -0.1f );
	}

	void ButtonCtrl::doSetCaption( String const & p_value )
	{
		m_caption = p_value;
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setCaption( p_value );
		}
	}

	void ButtonCtrl::doSetVisible( bool p_visible )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setVisible( p_visible );
		}
	}

	void ButtonCtrl::onMouseEnter( MouseEvent const & p_event )
	{
		m_text.lock()->setMaterial( m_highlightedTextMaterial );
		BorderPanelOverlaySPtr panel = getBackground();

		if ( panel )
		{
			panel->setMaterial( m_highlightedBackgroundMaterial );
			panel->setBorderMaterial( m_highlightedForegroundMaterial );
			panel.reset();
		}
	}

	void ButtonCtrl::onMouseLeave( MouseEvent const & p_event )
	{
		m_text.lock()->setMaterial( m_textMaterial );
		BorderPanelOverlaySPtr panel = getBackground();

		if ( panel )
		{
			panel->setMaterial( m_backgroundMaterial );
			panel->setBorderMaterial( m_foregroundMaterial );
			panel.reset();
		}
	}

	void ButtonCtrl::onMouseButtonDown( MouseEvent const & p_event )
	{
		if ( p_event.getButton() == MouseButton::eLeft )
		{
			m_text.lock()->setMaterial( m_pushedTextMaterial );
			BorderPanelOverlaySPtr panel = getBackground();

			if ( panel )
			{
				panel->setMaterial( m_pushedBackgroundMaterial );
				panel->setBorderMaterial( m_pushedForegroundMaterial );
				panel.reset();
			}
		}
	}

	void ButtonCtrl::onMouseButtonUp( MouseEvent const & p_event )
	{
		if ( p_event.getButton() == MouseButton::eLeft )
		{
			m_text.lock()->setMaterial( m_highlightedTextMaterial );
			BorderPanelOverlaySPtr panel = getBackground();

			if ( panel )
			{
				panel->setMaterial( m_highlightedBackgroundMaterial );
				panel->setBorderMaterial( m_highlightedForegroundMaterial );
				panel.reset();
			}

			m_signals[size_t( ButtonEvent::eClicked )]();
		}
	}

	MaterialRPtr ButtonCtrl::doCreateMaterial( MaterialRPtr p_material, float p_offset )
	{
		RgbColour colour = getMaterialColour( *p_material->getPass( 0u ) );
		colour.red() = float( colour.red() ) + p_offset;
		colour.green() = float( colour.green() ) + p_offset;
		colour.blue() = float( colour.blue() ) + p_offset;
		return createMaterial( getEngine(), p_material->getName() + cuT( "_MO" ), colour );
	}
}

#include "CtrlButton.hpp"

#include "ControlsManager.hpp"

#include <BorderPanelOverlay.hpp>
#include <InitialiseEvent.hpp>
#include <Material.hpp>
#include <OverlayManager.hpp>
#include <Pass.hpp>
#include <TextOverlay.hpp>

using namespace Castor;
using namespace Castor3D;

namespace CastorGui
{
	ButtonCtrl::ButtonCtrl( ControlRPtr p_parent, uint32_t p_id )
		: ButtonCtrl( p_parent, p_id, cuT( "" ), Position(), Size(), 0, true )
	{
	}

	ButtonCtrl::ButtonCtrl( ControlRPtr p_parent, uint32_t p_id, String const & p_caption, Position const & p_position, Size const & p_size, uint32_t p_style, bool p_visible )
		: Control( eCONTROL_TYPE_BUTTON, p_parent, p_id, p_position, p_size, p_style, p_visible )
		, m_caption( p_caption )
	{
		SetBackgroundBorders( Rectangle( 1, 1, 1, 1 ) );
		EventHandler::Connect( eMOUSE_EVENT_MOUSE_ENTER, std::bind( &ButtonCtrl::OnMouseEnter, this, std::placeholders::_1 ) );
		EventHandler::Connect( eMOUSE_EVENT_MOUSE_LEAVE, std::bind( &ButtonCtrl::OnMouseLeave, this, std::placeholders::_1 ) );
		EventHandler::Connect( eMOUSE_EVENT_MOUSE_BUTTON_RELEASED, std::bind( &ButtonCtrl::OnMouseLButtonUp, this, std::placeholders::_1 ) );
	}

	ButtonCtrl::~ButtonCtrl()
	{
	}

	void ButtonCtrl::SetCaption( String const & p_value )
	{
		m_caption =  p_value;
		TextOverlaySPtr l_text = m_text.lock();

		if ( l_text )
		{
			l_text->SetCaption( p_value );
			l_text.reset();
		}
	}

	void ButtonCtrl::SetMouseOverBackgroundMaterial( MaterialSPtr p_material )
	{
		m_mouseOverBackgroundMaterial = p_material;
	}

	void ButtonCtrl::SetMouseOverForegroundMaterial( MaterialSPtr p_material )
	{
		m_mouseOverForegroundMaterial = p_material;
	}

	void ButtonCtrl::DoCreate()
	{
		GetBackground()->SetBorderPosition( eBORDER_POSITION_INTERNAL );
		TextOverlaySPtr l_text = GetEngine()->GetOverlayManager().CreateText( cuT( "T_CtrlButton_" ) + string::to_string( GetId() ), Position(), GetSize(), GetForegroundMaterial(), GetControlsManager()->GetDefaultFont(), GetBackground()->GetOverlay().shared_from_this() );
		l_text->SetHAlign( eHALIGN_CENTER );
		l_text->SetVAlign( eVALIGN_CENTER );
		l_text->SetCaption( m_caption );
		l_text->SetVisible( DoIsVisible() );
		m_text = l_text;

		if ( !GetBackgroundMaterial() )
		{
			SetBackgroundMaterial( GetEngine()->GetMaterialManager().find( cuT( "Black" ) ) );
		}

		if ( m_mouseOverBackgroundMaterial.expired() )
		{
			m_mouseOverBackgroundMaterial = DoCreateMaterial( GetBackgroundMaterial(), 0.1f );
		}

		if ( m_mouseOverForegroundMaterial.expired() )
		{
			m_mouseOverForegroundMaterial = DoCreateMaterial( GetForegroundMaterial(), -0.1f );
		}
	}

	void ButtonCtrl::DoDestroy()
	{
	}

	void ButtonCtrl::DoSetPosition( Position const & p_value )
	{
		TextOverlaySPtr l_text = m_text.lock();

		if ( l_text )
		{
			l_text->SetPixelPosition( Position() );
			l_text.reset();
		}
	}

	void ButtonCtrl::DoSetSize( Size const & p_value )
	{
		TextOverlaySPtr l_text = m_text.lock();

		if ( l_text )
		{
			l_text->SetPixelSize( p_value );
		}
	}

	void ButtonCtrl::DoSetBackgroundMaterial( MaterialSPtr p_material )
	{
		if ( GetEngine() )
		{
			m_mouseOverBackgroundMaterial = DoCreateMaterial( p_material, 0.1f );
		}
	}

	void ButtonCtrl::DoSetForegroundMaterial( MaterialSPtr p_material )
	{
		if ( GetEngine() )
		{
			m_mouseOverForegroundMaterial = DoCreateMaterial( p_material, -0.1f );
		}

		TextOverlaySPtr l_text = m_text.lock();

		if ( l_text )
		{
			l_text->SetMaterial( p_material );
			l_text.reset();
		}
	}

	void ButtonCtrl::DoSetVisible( bool p_visible )
	{
		TextOverlaySPtr l_text = m_text.lock();

		if ( l_text )
		{
			l_text->SetVisible( p_visible );
		}
	}

	void ButtonCtrl::OnMouseEnter( MouseEvent const & p_event )
	{
		m_text.lock()->SetMaterial( m_mouseOverForegroundMaterial.lock() );
		BorderPanelOverlaySPtr l_panel = GetBackground();

		if ( l_panel )
		{
			l_panel->SetMaterial( m_mouseOverBackgroundMaterial.lock() );
			l_panel->SetBorderMaterial( m_mouseOverForegroundMaterial.lock() );
			l_panel.reset();
		}
	}

	void ButtonCtrl::OnMouseLeave( MouseEvent const & p_event )
	{
		m_text.lock()->SetMaterial( m_foregroundMaterial.lock() );
		BorderPanelOverlaySPtr l_panel = GetBackground();

		if ( l_panel )
		{
			l_panel->SetMaterial( m_backgroundMaterial.lock() );
			l_panel->SetBorderMaterial( m_foregroundMaterial.lock() );
			l_panel.reset();
		}
	}

	void ButtonCtrl::OnMouseLButtonUp( MouseEvent const & p_event )
	{
		if ( p_event.GetButton() == eMOUSE_BUTTON_LEFT )
		{
			m_signals[eBUTTON_EVENT_CLICKED]();
		}
	}

	MaterialSPtr ButtonCtrl::DoCreateMaterial( MaterialSPtr p_material, float p_offset )
	{
		Colour l_colour = p_material->GetPass( 0 )->GetAmbient();
		l_colour.red() = float( l_colour.red() ) + p_offset;
		l_colour.green() = float( l_colour.green() ) + p_offset;
		l_colour.blue() = float( l_colour.blue() ) + p_offset;
		l_colour.alpha() = float( l_colour.alpha() ) + p_offset;
		return CreateMaterial( GetEngine(), p_material->GetName() + cuT( "_MO" ), l_colour );
	}
}

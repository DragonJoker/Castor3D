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
	ButtonCtrl::ButtonCtrl( ControlSPtr p_parent, uint32_t p_id )
		: Control( eCONTROL_TYPE_BUTTON, p_parent, p_id )
	{
		SetBackgroundBorders( Rectangle( 1, 1, 1, 1 ) );
		EventHandler::Connect( eMOUSE_EVENT_MOUSE_ENTER, std::bind( &ButtonCtrl::OnMouseEnter, this, std::placeholders::_1 ) );
		EventHandler::Connect( eMOUSE_EVENT_MOUSE_LEAVE, std::bind( &ButtonCtrl::OnMouseLeave, this, std::placeholders::_1 ) );
		EventHandler::Connect( eMOUSE_EVENT_MOUSE_BUTTON_RELEASED, std::bind( &ButtonCtrl::OnMouseLButtonUp, this, std::placeholders::_1 ) );
	}

	ButtonCtrl::ButtonCtrl( ControlSPtr p_parent, uint32_t p_id, String const & p_caption, Position const & p_position, Size const & p_size, uint32_t p_style, bool p_visible )
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

	void ButtonCtrl::SetMouseOverBackgroundTexture( Castor3D::TextureBaseSPtr p_texture )
	{
		m_mouseOverBackgroundTexture = p_texture;
	}

	void ButtonCtrl::SetMouseOverForegroundTexture( Castor3D::TextureBaseSPtr p_texture )
	{
		m_mouseOverForegroundTexture = p_texture;
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
		Colour l_colour = p_material->GetPass( 0 )->GetAmbient();
		m_mouseOverBackgroundColour.red() = std::min( 1.0f, l_colour.red() + 0.1f );
		m_mouseOverBackgroundColour.green() = std::min( 1.0f, l_colour.green() + 0.1f );
		m_mouseOverBackgroundColour.blue() = std::min( 1.0f, l_colour.blue() + 0.1f );
		m_mouseOverBackgroundColour.alpha() = std::min( 1.0f, l_colour.alpha() + 0.1f );
	}

	void ButtonCtrl::DoSetForegroundMaterial( MaterialSPtr p_material )
	{
		Colour l_colour = p_material->GetPass( 0 )->GetAmbient();
		m_mouseOverForegroundColour.red() = std::max( 0.0f, l_colour.red() - 0.1f );
		m_mouseOverForegroundColour.green() = std::max( 0.0f, l_colour.green() - 0.1f );
		m_mouseOverForegroundColour.blue() = std::max( 0.0f, l_colour.blue() - 0.1f );
		m_mouseOverForegroundColour.alpha() = std::max( 0.0f, l_colour.alpha() - 0.1f );
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
		m_text.lock()->GetMaterial()->GetPass( 0 )->SetAmbient( m_mouseOverForegroundColour );
		BorderPanelOverlaySPtr l_panel = GetBackground();

		if ( l_panel )
		{
			l_panel->GetMaterial()->GetPass( 0 )->SetAmbient( m_mouseOverBackgroundColour );
			l_panel->GetBorderMaterial()->GetPass( 0 )->SetAmbient( m_mouseOverForegroundColour );

			if ( m_mouseOverBackgroundTexture )
			{
				m_backgroundTexture = l_panel->GetMaterial()->GetPass( 0 )->GetTextureUnit( 0 ) ->GetTexture();
				l_panel->GetMaterial()->GetPass( 0 )->GetTextureUnit( 0 ) ->SetTexture( m_mouseOverBackgroundTexture );

				if ( m_mouseOverForegroundTexture )
				{
					m_foregroundTexture = l_panel->GetBorderMaterial()->GetPass( 0 )->GetTextureUnit( 0 ) ->GetTexture();
					l_panel->GetBorderMaterial()->GetPass( 0 )->GetTextureUnit( 0 ) ->SetTexture( m_mouseOverForegroundTexture );
				}
				else
				{
					m_foregroundTexture = l_panel->GetBorderMaterial()->GetPass( 0 )->GetTextureUnit( 0 ) ->GetTexture();
					l_panel->GetBorderMaterial()->GetPass( 0 )->GetTextureUnit( 0 ) ->SetTexture( m_mouseOverBackgroundTexture );
				}
			}

			l_panel.reset();
		}
	}

	void ButtonCtrl::OnMouseLeave( MouseEvent const & p_event )
	{
		m_text.lock()->GetMaterial()->GetPass( 0 )->SetAmbient( m_foregroundColour );
		BorderPanelOverlaySPtr l_panel = GetBackground();

		if ( l_panel )
		{
			l_panel->GetMaterial()->GetPass( 0 )->SetAmbient( m_backgroundColour );
			l_panel->GetBorderMaterial()->GetPass( 0 )->SetAmbient( m_foregroundColour );

			if ( m_backgroundTexture )
			{
				l_panel->GetMaterial()->GetPass( 0 )->GetTextureUnit( 0 ) ->SetTexture( m_backgroundTexture );

				if ( m_foregroundTexture )
				{
					l_panel->GetBorderMaterial()->GetPass( 0 )->GetTextureUnit( 0 ) ->SetTexture( m_foregroundTexture );
				}
				else
				{
					l_panel->GetBorderMaterial()->GetPass( 0 )->GetTextureUnit( 0 ) ->SetTexture( m_backgroundTexture );
				}
				
				m_backgroundTexture.reset();
				m_foregroundTexture.reset();
			}

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
}

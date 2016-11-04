#include "CtrlButton.hpp"

#include "ControlsManager.hpp"

#include <Engine.hpp>
#include <Material/Material.hpp>
#include <Overlay/Overlay.hpp>

#include <Event/Frame/InitialiseEvent.hpp>
#include <Material/Pass.hpp>
#include <Overlay/BorderPanelOverlay.hpp>
#include <Overlay/TextOverlay.hpp>

#include <Graphics/Font.hpp>

using namespace Castor;
using namespace Castor3D;

namespace CastorGui
{
	ButtonCtrl::ButtonCtrl( Engine * p_engine, ControlRPtr p_parent, uint32_t p_id )
		: ButtonCtrl( p_engine, p_parent, p_id, cuT( "" ), Position(), Size(), 0, true )
	{
	}

	ButtonCtrl::ButtonCtrl( Engine * p_engine, ControlRPtr p_parent, uint32_t p_id, String const & p_caption, Position const & p_position, Size const & p_size, uint32_t p_style, bool p_visible )
		: Control( ControlType::eButton, p_engine, p_parent, p_id, p_position, p_size, p_style, p_visible )
		, m_caption( p_caption )
	{
		SetBackgroundBorders( Rectangle( 1, 1, 1, 1 ) );
		EventHandler::Connect( MouseEventType::eEnter, [this]( MouseEvent const & p_event )
		{
			OnMouseEnter( p_event );
		} );
		EventHandler::Connect( MouseEventType::eLeave, [this]( MouseEvent const & p_event )
		{
			OnMouseLeave( p_event );
		} );
		EventHandler::Connect( MouseEventType::ePushed, [this]( MouseEvent const & p_event )
		{
			OnMouseButtonDown( p_event );
		} );
		EventHandler::Connect( MouseEventType::eReleased, [this]( MouseEvent const & p_event )
		{
			OnMouseButtonUp( p_event );
		} );

		TextOverlaySPtr l_text = GetEngine()->GetOverlayCache().Add( cuT( "T_CtrlButton_" ) + string::to_string( GetId() ), OverlayType::eText, nullptr, GetBackground()->GetOverlay().shared_from_this() )->GetTextOverlay();
		l_text->SetPixelSize( GetSize() );
		l_text->SetHAlign( HAlign::eCenter );
		l_text->SetVAlign( VAlign::eCenter );
		l_text->SetCaption( m_caption );
		l_text->SetVisible( DoIsVisible() );
		m_text = l_text;
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
		}
	}

	void ButtonCtrl::SetTextMaterial( MaterialSPtr p_material )
	{
		m_textMaterial = p_material;
	}

	void ButtonCtrl::SetHighlightedBackgroundMaterial( MaterialSPtr p_material )
	{
		m_highlightedBackgroundMaterial = p_material;
	}

	void ButtonCtrl::SetHighlightedForegroundMaterial( MaterialSPtr p_material )
	{
		m_highlightedForegroundMaterial = p_material;
	}

	void ButtonCtrl::SetHighlightedTextMaterial( MaterialSPtr p_material )
	{
		m_highlightedTextMaterial = p_material;
	}

	void ButtonCtrl::SetPushedBackgroundMaterial( MaterialSPtr p_material )
	{
		m_pushedBackgroundMaterial = p_material;
	}

	void ButtonCtrl::SetPushedForegroundMaterial( MaterialSPtr p_material )
	{
		m_pushedForegroundMaterial = p_material;
	}

	void ButtonCtrl::SetPushedTextMaterial( MaterialSPtr p_material )
	{
		m_pushedTextMaterial = p_material;
	}

	void ButtonCtrl::SetFont( Castor::String const & p_font )
	{
		TextOverlaySPtr l_text = m_text.lock();

		if ( l_text )
		{
			l_text->SetFont( p_font );
		}
	}

	void ButtonCtrl::DoCreate()
	{
		GetBackground()->SetBorderPosition( BorderPosition::eInternal );

		if ( !GetBackgroundMaterial() )
		{
			SetBackgroundMaterial( GetEngine()->GetMaterialCache().Find( cuT( "Black" ) ) );
		}

		if ( !GetForegroundMaterial() )
		{
			SetForegroundMaterial( GetEngine()->GetMaterialCache().Find( cuT( "White" ) ) );
		}

		if ( m_textMaterial.expired() )
		{
			m_textMaterial = GetForegroundMaterial();
		}

		if ( m_highlightedBackgroundMaterial.expired() )
		{
			m_highlightedBackgroundMaterial = DoCreateMaterial( GetBackgroundMaterial(), 0.1f );
		}

		if ( m_highlightedForegroundMaterial.expired() )
		{
			m_highlightedForegroundMaterial = DoCreateMaterial( GetForegroundMaterial(), -0.1f );
		}

		if ( m_highlightedTextMaterial.expired() )
		{
			m_highlightedTextMaterial = m_highlightedBackgroundMaterial.lock();
		}

		if ( m_pushedBackgroundMaterial.expired() )
		{
			m_pushedBackgroundMaterial = DoCreateMaterial( GetBackgroundMaterial(), 0.1f );
		}

		if ( m_pushedForegroundMaterial.expired() )
		{
			m_pushedForegroundMaterial = DoCreateMaterial( GetForegroundMaterial(), -0.1f );
		}

		if ( m_pushedTextMaterial.expired() )
		{
			m_pushedTextMaterial = m_pushedBackgroundMaterial.lock();
		}

		TextOverlaySPtr l_text = m_text.lock();
		l_text->SetMaterial( m_textMaterial.lock() );

		if ( !l_text->GetFontTexture() || !l_text->GetFontTexture()->GetFont() )
		{
			l_text->SetFont( GetControlsManager()->GetDefaultFont()->GetName() );
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
			m_highlightedBackgroundMaterial = DoCreateMaterial( p_material, 0.1f );
		}
	}

	void ButtonCtrl::DoSetForegroundMaterial( MaterialSPtr p_material )
	{
		if ( GetEngine() )
		{
			m_highlightedForegroundMaterial = DoCreateMaterial( p_material, -0.1f );
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
		m_text.lock()->SetMaterial( m_highlightedTextMaterial.lock() );
		BorderPanelOverlaySPtr l_panel = GetBackground();

		if ( l_panel )
		{
			l_panel->SetMaterial( m_highlightedBackgroundMaterial.lock() );
			l_panel->SetBorderMaterial( m_highlightedForegroundMaterial.lock() );
			l_panel.reset();
		}
	}

	void ButtonCtrl::OnMouseLeave( MouseEvent const & p_event )
	{
		m_text.lock()->SetMaterial( m_textMaterial.lock() );
		BorderPanelOverlaySPtr l_panel = GetBackground();

		if ( l_panel )
		{
			l_panel->SetMaterial( m_backgroundMaterial.lock() );
			l_panel->SetBorderMaterial( m_foregroundMaterial.lock() );
			l_panel.reset();
		}
	}

	void ButtonCtrl::OnMouseButtonDown( MouseEvent const & p_event )
	{
		if ( p_event.GetButton() == MouseButton::eLeft )
		{
			m_text.lock()->SetMaterial( m_pushedTextMaterial.lock() );
			BorderPanelOverlaySPtr l_panel = GetBackground();

			if ( l_panel )
			{
				l_panel->SetMaterial( m_pushedBackgroundMaterial.lock() );
				l_panel->SetBorderMaterial( m_pushedForegroundMaterial.lock() );
				l_panel.reset();
			}
		}
	}

	void ButtonCtrl::OnMouseButtonUp( MouseEvent const & p_event )
	{
		if ( p_event.GetButton() == MouseButton::eLeft )
		{
			m_text.lock()->SetMaterial( m_highlightedTextMaterial.lock() );
			BorderPanelOverlaySPtr l_panel = GetBackground();

			if ( l_panel )
			{
				l_panel->SetMaterial( m_highlightedBackgroundMaterial.lock() );
				l_panel->SetBorderMaterial( m_highlightedForegroundMaterial.lock() );
				l_panel.reset();
			}

			m_signals[size_t( ButtonEvent::eClicked )]();
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

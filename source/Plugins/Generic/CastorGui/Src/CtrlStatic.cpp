#include "CtrlStatic.hpp"

#include "ControlsManager.hpp"

#include <Engine.hpp>
#include <OverlayManager.hpp>

#include <Overlay/BorderPanelOverlay.hpp>
#include <Overlay/TextOverlay.hpp>

#include <Font.hpp>

using namespace Castor;
using namespace Castor3D;

namespace CastorGui
{
	int StaticCtrl::m_count = 0xFF000000;

	StaticCtrl::StaticCtrl( Engine * p_engine, ControlRPtr p_parent, uint32_t p_id )
		: StaticCtrl( p_engine, p_parent, String(), Position(), Size(), 0, true )
	{
	}

	StaticCtrl::StaticCtrl( Engine * p_engine, ControlRPtr p_parent, String const & p_caption, Position const & p_position, Size const & p_size, uint32_t p_style, bool p_visible )
		: Control( eCONTROL_TYPE_STATIC, p_engine, p_parent, m_count++, p_position, p_size, p_style, p_visible )
		, m_caption( p_caption )
	{
		SetBackgroundBorders( Rectangle() );

		TextOverlaySPtr l_text = GetEngine()->GetOverlayCache().Create( cuT( "T_CtrlStatic_" ) + string::to_string( GetId() ), eOVERLAY_TYPE_TEXT, GetBackground()->GetOverlay().shared_from_this(), nullptr )->GetTextOverlay();
		l_text->SetPixelSize( GetSize() );
		m_text = l_text;
		l_text->SetCaption( m_caption );
		l_text->SetVisible( DoIsVisible() );
		l_text->SetVAlign( eVALIGN_CENTER );
		DoUpdateStyle();
	}

	StaticCtrl::~StaticCtrl()
	{
	}

	void StaticCtrl::SetCaption( String const & p_value )
	{
		m_caption = p_value;
		TextOverlaySPtr l_text = m_text.lock();

		if ( l_text )
		{
			l_text->SetCaption( p_value );
			l_text.reset();
		}
	}

	void StaticCtrl::SetFont( Castor::String const & p_font )
	{
		TextOverlaySPtr l_text = m_text.lock();

		if ( l_text )
		{
			l_text->SetFont( p_font );
		}
	}

	void StaticCtrl::DoCreate()
	{
		if ( m_foregroundMaterial.expired() )
		{
			m_foregroundMaterial = CreateMaterial( GetEngine(), cuT( "CtrlStatic_FG_" ) + string::to_string( GetId() ), Colour::from_components( 1.0, 1.0, 1.0, 1.0 ) );
		}

		TextOverlaySPtr l_text = m_text.lock();
		l_text->SetMaterial( GetForegroundMaterial() );

		if ( !l_text->GetFontTexture() || !l_text->GetFontTexture()->GetFont() )
		{
			l_text->SetFont( GetControlsManager()->GetDefaultFont()->GetName() );
		}
	}

	void StaticCtrl::DoDestroy()
	{
	}

	void StaticCtrl::DoSetPosition( Position const & p_value )
	{
		TextOverlaySPtr l_text = m_text.lock();

		if ( l_text )
		{
			l_text->SetPixelPosition( Position() );
		}
	}

	void StaticCtrl::DoSetSize( Size const & p_value )
	{
		TextOverlaySPtr l_text = m_text.lock();

		if ( l_text )
		{
			l_text->SetPixelSize( p_value );
		}
	}

	void StaticCtrl::DoSetBackgroundMaterial( MaterialSPtr p_material )
	{
	}

	void StaticCtrl::DoSetForegroundMaterial( MaterialSPtr p_material )
	{
		TextOverlaySPtr l_text = m_text.lock();

		if ( l_text )
		{
			l_text->SetMaterial( p_material );
		}
	}

	void StaticCtrl::DoSetVisible( bool p_visible )
	{
		TextOverlaySPtr l_text = m_text.lock();

		if ( l_text )
		{
			l_text->SetVisible( p_visible );
		}
	}

	void StaticCtrl::DoUpdateStyle()
	{
		TextOverlaySPtr l_text = m_text.lock();

		if ( l_text )
		{
			if ( GetStyle() & eSTATIC_STYLE_HALIGN_CENTER )
			{
				l_text->SetHAlign( eHALIGN_CENTER );
			}
			else if ( GetStyle() & eSTATIC_STYLE_HALIGN_RIGHT )
			{
				l_text->SetHAlign( eHALIGN_RIGHT );
			}
			else
			{
				l_text->SetHAlign( eHALIGN_LEFT );
			}

			if ( GetStyle() & eSTATIC_STYLE_VALIGN_CENTER )
			{
				l_text->SetVAlign( eVALIGN_CENTER );
			}
			else if ( GetStyle() & eSTATIC_STYLE_VALIGN_BOTTOM )
			{
				l_text->SetVAlign( eVALIGN_BOTTOM );
			}
			else
			{
				l_text->SetVAlign( eVALIGN_TOP );
			}
		}
	}
}

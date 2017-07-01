#include "CtrlStatic.hpp"

#include "ControlsManager.hpp"

#include <Engine.hpp>
#include <Overlay/Overlay.hpp>

#include <Overlay/BorderPanelOverlay.hpp>
#include <Overlay/TextOverlay.hpp>

#include <Graphics/Font.hpp>

using namespace Castor;
using namespace Castor3D;

namespace CastorGui
{
	int StaticCtrl::m_count = 0xFF000000;

	StaticCtrl::StaticCtrl( String const & p_name
		, Engine & p_engine
		, ControlRPtr p_parent
		, uint32_t p_id )
		: StaticCtrl( p_name
			, p_engine
			, p_parent
			, String()
			, Position()
			, Size()
			, 0
			, true )
	{
	}

	StaticCtrl::StaticCtrl( String const & p_name
		, Engine & p_engine
		, ControlRPtr p_parent
		, String const & p_caption
		, Position const & p_position
		, Size const & p_size
		, uint32_t p_style
		, bool p_visible )
		: Control( ControlType::eStatic
			, p_name
			, p_engine
			, p_parent
			, m_count++
			, p_position
			, p_size
			, p_style
			, p_visible )
		, m_caption( p_caption )
	{
		SetBackgroundBorders( Rectangle() );

		TextOverlaySPtr l_text = GetEngine().GetOverlayCache().Add( cuT( "T_CtrlStatic_" ) + string::to_string( GetId() )
			, OverlayType::eText
			, nullptr
			, GetBackground()->GetOverlay().shared_from_this() )->GetTextOverlay();
		l_text->SetPixelSize( GetSize() );
		m_text = l_text;
		l_text->SetCaption( m_caption );
		l_text->SetVisible( DoIsVisible() );
		l_text->SetVAlign( VAlign::eCenter );
		DoUpdateStyle();
	}

	StaticCtrl::~StaticCtrl()
	{
	}

	void StaticCtrl::SetFont( String const & p_font )
	{
		TextOverlaySPtr l_text = m_text.lock();

		if ( l_text )
		{
			l_text->SetFont( p_font );
		}
	}

	void StaticCtrl::SetHAlign( HAlign p_align )
	{
		TextOverlaySPtr l_text = m_text.lock();

		if ( l_text )
		{
			l_text->SetHAlign( p_align );
		}
	}

	void StaticCtrl::SetVAlign( VAlign p_align )
	{
		TextOverlaySPtr l_text = m_text.lock();

		if ( l_text )
		{
			l_text->SetVAlign( p_align );
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

	void StaticCtrl::DoSetCaption( String const & p_value )
	{
		m_caption = p_value;
		TextOverlaySPtr l_text = m_text.lock();

		if ( l_text )
		{
			l_text->SetCaption( p_value );
			l_text.reset();
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
			if ( CheckFlag( GetStyle(), StaticStyle::eHAlignCenter ) )
			{
				l_text->SetHAlign( HAlign::eCenter );
			}
			else if ( CheckFlag( GetStyle(), StaticStyle::eHAlignRight ) )
			{
				l_text->SetHAlign( HAlign::eRight );
			}
			else
			{
				l_text->SetHAlign( HAlign::eLeft );
			}

			if ( CheckFlag( GetStyle(), StaticStyle::eVAlignCenter ) )
			{
				l_text->SetVAlign( VAlign::eCenter );
			}
			else if ( CheckFlag( GetStyle(), StaticStyle::eVAlignBottom ) )
			{
				l_text->SetVAlign( VAlign::eBottom );
			}
			else
			{
				l_text->SetVAlign( VAlign::eTop );
			}
		}
	}
}

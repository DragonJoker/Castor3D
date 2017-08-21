#include "CtrlStatic.hpp"

#include "ControlsManager.hpp"

#include <Engine.hpp>
#include <Overlay/Overlay.hpp>

#include <Overlay/BorderPanelOverlay.hpp>
#include <Overlay/TextOverlay.hpp>

#include <Graphics/Font.hpp>

using namespace castor;
using namespace castor3d;

namespace CastorGui
{
	int StaticCtrl::m_count = 0xFF000000;

	StaticCtrl::StaticCtrl( String const & p_name
		, Engine & engine
		, ControlRPtr p_parent
		, uint32_t p_id )
		: StaticCtrl( p_name
			, engine
			, p_parent
			, String()
			, Position()
			, Size()
			, 0
			, true )
	{
	}

	StaticCtrl::StaticCtrl( String const & p_name
		, Engine & engine
		, ControlRPtr p_parent
		, String const & p_caption
		, Position const & p_position
		, Size const & p_size
		, uint32_t p_style
		, bool p_visible )
		: Control( ControlType::eStatic
			, p_name
			, engine
			, p_parent
			, m_count++
			, p_position
			, p_size
			, p_style
			, p_visible )
		, m_caption( p_caption )
	{
		setBackgroundBorders( Rectangle() );

		TextOverlaySPtr text = getEngine().getOverlayCache().add( cuT( "T_CtrlStatic_" ) + string::toString( getId() )
			, OverlayType::eText
			, nullptr
			, getBackground()->getOverlay().shared_from_this() )->getTextOverlay();
		text->setPixelSize( getSize() );
		m_text = text;
		text->setCaption( m_caption );
		text->setVisible( p_visible );
		text->setVAlign( VAlign::eCenter );
		doUpdateStyle();
	}

	StaticCtrl::~StaticCtrl()
	{
	}

	void StaticCtrl::setFont( String const & p_font )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setFont( p_font );
		}
	}

	void StaticCtrl::setHAlign( HAlign p_align )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setHAlign( p_align );
		}
	}

	void StaticCtrl::setVAlign( VAlign p_align )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setVAlign( p_align );
		}
	}

	void StaticCtrl::doCreate()
	{
		if ( m_foregroundMaterial.expired() )
		{
			m_foregroundMaterial = CreateMaterial( getEngine(), cuT( "CtrlStatic_FG_" ) + string::toString( getId() ), Colour::fromComponents( 1.0, 1.0, 1.0, 1.0 ) );
		}

		TextOverlaySPtr text = m_text.lock();
		text->setMaterial( getForegroundMaterial() );

		if ( !text->getFontTexture() || !text->getFontTexture()->getFont() )
		{
			text->setFont( getControlsManager()->getDefaultFont()->getName() );
		}
	}

	void StaticCtrl::doDestroy()
	{
	}

	void StaticCtrl::doSetPosition( Position const & p_value )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setPixelPosition( Position() );
		}
	}

	void StaticCtrl::doSetSize( Size const & p_value )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setPixelSize( p_value );
		}
	}

	void StaticCtrl::doSetBackgroundMaterial( MaterialSPtr p_material )
	{
	}

	void StaticCtrl::doSetForegroundMaterial( MaterialSPtr p_material )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setMaterial( p_material );
		}
	}

	void StaticCtrl::doSetCaption( String const & p_value )
	{
		m_caption = p_value;
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setCaption( p_value );
			text.reset();
		}
	}

	void StaticCtrl::doSetVisible( bool p_visible )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setVisible( p_visible );
		}
	}

	void StaticCtrl::doUpdateStyle()
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			if ( checkFlag( getStyle(), StaticStyle::eHAlignCenter ) )
			{
				text->setHAlign( HAlign::eCenter );
			}
			else if ( checkFlag( getStyle(), StaticStyle::eHAlignRight ) )
			{
				text->setHAlign( HAlign::eRight );
			}
			else
			{
				text->setHAlign( HAlign::eLeft );
			}

			if ( checkFlag( getStyle(), StaticStyle::eVAlignCenter ) )
			{
				text->setVAlign( VAlign::eCenter );
			}
			else if ( checkFlag( getStyle(), StaticStyle::eVAlignBottom ) )
			{
				text->setVAlign( VAlign::eBottom );
			}
			else
			{
				text->setVAlign( VAlign::eTop );
			}
		}
	}
}

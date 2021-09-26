#include "CastorGui/CtrlStatic.hpp"

#include "CastorGui/ControlsManager.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/OverlayCache.hpp>
#include <Castor3D/Overlay/Overlay.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Overlay/TextOverlay.hpp>

#include <CastorUtils/Graphics/Font.hpp>

using namespace castor;
using namespace castor3d;

namespace CastorGui
{
	uint32_t StaticCtrl::m_count = 0xFF000000u;

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
		setBackgroundBorders( castor::Rectangle() );

		TextOverlaySPtr text = getEngine().getOverlayCache().add( cuT( "T_CtrlStatic_" ) + string::toString( getId() )
			, getEngine()
			, OverlayType::eText
			, nullptr
			, &getBackground()->getOverlay() ).lock()->getTextOverlay();
		text->setPixelSize( getSize() );
		m_text = text;
		text->setCaption( m_caption );
		text->setVisible( p_visible );
		text->setVAlign( VAlign::eCenter );
		doUpdateStyle();
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
		if ( !m_foregroundMaterial )
		{
			m_foregroundMaterial = createMaterial( getEngine(), cuT( "CtrlStatic_FG_" ) + string::toString( getId() ), RgbColour::fromComponents( 1.0, 1.0, 1.0 ) );
		}

		if ( !m_textMaterial )
		{
			m_textMaterial = m_foregroundMaterial;
		}

		TextOverlaySPtr text = m_text.lock();
		text->setMaterial( getTextMaterial() );

		if ( !text->getFontTexture() || !text->getFontTexture()->getFont() )
		{
			text->setFont( getControlsManager()->getDefaultFont().lock()->getName() );
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

	void StaticCtrl::setTextMaterial( castor3d::MaterialRPtr value )
	{
		m_textMaterial = value;
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setMaterial( value );
		}
	}

	void StaticCtrl::doSetBackgroundMaterial( MaterialRPtr p_material )
	{
	}

	void StaticCtrl::doSetForegroundMaterial( MaterialRPtr p_material )
	{
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

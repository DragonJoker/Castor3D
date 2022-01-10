#include "CastorGui/Controls/CtrlStatic.hpp"

#include "CastorGui/ControlsManager.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/OverlayCache.hpp>
#include <Castor3D/Overlay/Overlay.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Overlay/TextOverlay.hpp>

#include <CastorUtils/Graphics/Font.hpp>

namespace CastorGui
{
	uint32_t StaticCtrl::m_count = 0xFF000000u;

	StaticCtrl::StaticCtrl( castor::String const & name
		, StaticStyle * style
		, ControlRPtr parent
		, uint32_t id )
		: StaticCtrl{ name
			, style
			, parent
			, castor::String{}
			, castor::Position{}
			, castor::Size{}
			, 0
			, true }
	{
	}

	StaticCtrl::StaticCtrl( castor::String const & name
		, StaticStyle * style
		, ControlRPtr parent
		, castor::String const & caption
		, castor::Position const & position
		, castor::Size const & size
		, uint32_t flags
		, bool visible )
		: Control{ Type
			, name
			, style
			, parent
			, m_count++
			, position
			, size
			, flags
			, visible }
		, m_caption{ caption }
	{
		setBackgroundBorders( castor::Rectangle{} );
		auto text = getEngine().getOverlayCache().add( cuT( "T_CtrlStatic_" ) + castor::string::toString( getId() )
			, getEngine()
			, castor3d::OverlayType::eText
			, nullptr
			, &getBackground()->getOverlay() ).lock()->getTextOverlay();
		m_text = text;
		text->setPixelSize( getSize() );
		text->setCaption( m_caption );
		text->setVisible( visible );
		text->setVAlign( castor3d::VAlign::eCenter );
		doUpdateStyle();
		doUpdateFlags();
	}

	void StaticCtrl::setHAlign( castor3d::HAlign align )
	{
		if ( auto text = m_text.lock() )
		{
			text->setHAlign( align );
		}
	}

	void StaticCtrl::setVAlign( castor3d::VAlign align )
	{
		if ( auto text = m_text.lock() )
		{
			text->setVAlign( align );
		}
	}

	void StaticCtrl::doUpdateStyle()
	{
		auto & style = getStyle();

		if ( auto text = m_text.lock() )
		{
			text->setFont( style.getFontName() );
			text->setMaterial( style.getTextMaterial() );
		}
	}

	void StaticCtrl::doCreate()
	{
	}

	void StaticCtrl::doDestroy()
	{
	}

	void StaticCtrl::doSetPosition( castor::Position const & value )
	{
		if ( auto text = m_text.lock() )
		{
			text->setPixelPosition( castor::Position{} );
		}
	}

	void StaticCtrl::doSetSize( castor::Size const & value )
	{
		if ( auto text = m_text.lock() )
		{
			text->setPixelSize( value );
		}
	}

	void StaticCtrl::doSetCaption( castor::String const & value )
	{
		m_caption = value;

		if ( auto text = m_text.lock() )
		{
			text->setCaption( value );
		}
	}

	void StaticCtrl::doSetVisible( bool visible )
	{
		if ( auto text = m_text.lock() )
		{
			text->setVisible( visible );
		}
	}

	void StaticCtrl::doUpdateFlags()
	{
		if ( auto text = m_text.lock() )
		{
			if ( castor::checkFlag( getFlags(), StaticFlag::eHAlignCenter ) )
			{
				text->setHAlign( castor3d::HAlign::eCenter );
			}
			else if ( castor::checkFlag( getFlags(), StaticFlag::eHAlignRight ) )
			{
				text->setHAlign( castor3d::HAlign::eRight );
			}
			else
			{
				text->setHAlign( castor3d::HAlign::eLeft );
			}

			if ( castor::checkFlag( getFlags(), StaticFlag::eVAlignCenter ) )
			{
				text->setVAlign( castor3d::VAlign::eCenter );
			}
			else if ( castor::checkFlag( getFlags(), StaticFlag::eVAlignBottom ) )
			{
				text->setVAlign( castor3d::VAlign::eBottom );
			}
			else
			{
				text->setVAlign( castor3d::VAlign::eTop );
			}
		}
	}
}

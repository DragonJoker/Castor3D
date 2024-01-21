#include "Castor3D/Gui/Controls/CtrlStatic.hpp"

#include "Castor3D/Gui/ControlsManager.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Graphics/Font.hpp>

CU_ImplementSmartPtr( castor3d, StaticCtrl )

namespace castor3d
{
	StaticCtrl::StaticCtrl( SceneRPtr scene
		, castor::String const & name
		, StaticStyle * style
		, ControlRPtr parent )
		: StaticCtrl{ scene
			, name
			, style
			, parent
			, castor::U32String{}
			, castor::Position{}
			, castor::Size{}
			, 0
			, true }
	{
	}

	StaticCtrl::StaticCtrl( SceneRPtr scene
		, castor::String const & name
		, StaticStyle * style
		, ControlRPtr parent
		, castor::U32String const & caption
		, castor::Position const & position
		, castor::Size const & size
		, ControlFlagType flags
		, bool visible )
		: Control{ Type
			, scene
			, name
			, style
			, parent
			, position
			, size
			, flags
			, visible }
		, m_caption{ caption }
	{
		setBorderSize( castor::Point4ui{} );
		auto text = m_scene
			? m_scene->addNewOverlay( getName() + cuT( "/Text" )
				, getEngine()
				, OverlayType::eText
				, nullptr
				, &getBackgroundOverlay() )->getTextOverlay()
			: getEngine().addNewOverlay( getName() + cuT( "/Text" )
				, getEngine()
				, OverlayType::eText
				, nullptr
				, &getBackgroundOverlay() )->getTextOverlay();
		m_text = text;
		text->setPixelSize( getClientSize() );
		text->setCaption( m_caption );
		text->setVisible( visible );
		text->setVAlign( VAlign::eCenter );

		setStyle( style );
		doUpdateFlags();
	}

	StaticCtrl::~StaticCtrl()noexcept
	{
		if ( m_text )
		{
			if ( m_scene )
			{
				m_scene->removeOverlay( getName() + cuT( "/Text" ), true );
			}
			else
			{
				getEngine().removeOverlay( getName() + cuT( "/Text" ), true );
			}
		}
	}

	void StaticCtrl::setHAlign( HAlign align )
	{
		m_flags &= ~ControlFlagType( StaticFlag::eHAlignCenter
			| StaticFlag::eHAlignRight
			| StaticFlag::eHAlignLeft );

		switch ( align )
		{
		case HAlign::eLeft:
			addFlag( StaticFlag::eHAlignLeft );
			break;
		case HAlign::eCenter:
			addFlag( StaticFlag::eHAlignCenter );
			break;
		case HAlign::eRight:
			addFlag( StaticFlag::eHAlignRight );
			break;
		default:
			break;
		}
	}

	void StaticCtrl::setVAlign( VAlign align )
	{
		m_flags &= ~ControlFlagType( StaticFlag::eVAlignCenter
			| StaticFlag::eVAlignTop
			| StaticFlag::eVAlignBottom );

		switch ( align )
		{
		case VAlign::eTop:
			addFlag( StaticFlag::eVAlignTop );
			break;
		case VAlign::eCenter:
			addFlag( StaticFlag::eVAlignCenter );
			break;
		case VAlign::eBottom:
			addFlag( StaticFlag::eVAlignBottom );
			break;
		default:
			break;
		}
	}

	HAlign StaticCtrl::getHAlign()const
	{
		if ( castor::checkFlag( getFlags(), StaticFlag::eHAlignCenter ) )
		{
			return HAlign::eCenter;
		}

		if ( castor::checkFlag( getFlags(), StaticFlag::eHAlignRight ) )
		{
			return HAlign::eRight;
		}

		return HAlign::eLeft;
	}

	VAlign StaticCtrl::getVAlign()const
	{
		if ( castor::checkFlag( getFlags(), StaticFlag::eVAlignCenter ) )
		{
			return VAlign::eCenter;
		}

		if ( castor::checkFlag( getFlags(), StaticFlag::eVAlignBottom ) )
		{
			return VAlign::eBottom;
		}

		return VAlign::eTop;
	}

	void StaticCtrl::doUpdateStyle()
	{
		auto const & style = getStyle();

		if ( auto text = m_text )
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
		if ( auto text = m_text )
		{
			text->setPixelPosition( getClientOffset() );
		}
	}

	void StaticCtrl::doSetSize( castor::Size const & value )
	{
		if ( auto text = m_text )
		{
			text->setPixelSize( getClientSize() );
		}
	}

	void StaticCtrl::doSetBorderSize( castor::Point4ui const & value )
	{
		if ( auto text = m_text )
		{
			text->setPixelPosition( getClientOffset() );
			text->setPixelSize( getClientSize() );
		}
	}

	void StaticCtrl::doSetCaption( castor::U32String const & value )
	{
		m_caption = value;

		if ( auto text = m_text )
		{
			text->setCaption( value );
		}
	}

	void StaticCtrl::doSetVisible( bool visible )
	{
		if ( auto text = m_text )
		{
			text->setVisible( visible );
		}
	}

	void StaticCtrl::doUpdateFlags()
	{
		if ( auto text = m_text )
		{
			text->setHAlign( getHAlign() );
			text->setVAlign( getVAlign() );
		}
	}

	void StaticCtrl::doUpdateZIndex( uint32_t & index )
	{
		if ( auto text = m_text )
		{
			text->setOrder( index, 0u );
			++index;
		}
	}

	void StaticCtrl::doAdjustZIndex( uint32_t offset )
	{
		if ( auto text = m_text )
		{
			text->setOrder( text->getLevel() + offset, 0u );
		}
	}
}

#include "Castor3D/Gui/Controls/CtrlStatic.hpp"

#include "Castor3D/Gui/ControlsManager.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Graphics/Font.hpp>

CU_ImplementSmartPtr( c3d, StaticCtrl )

namespace c3d
{
	StaticCtrl::StaticCtrl( SceneRPtr scene
		, String const & name
		, StaticStyle * style
		, ControlRPtr parent )
		: StaticCtrl{ scene
			, name
			, style
			, parent
			, U32String{}
			, Position{}
			, Size{}
			, 0
			, true }
	{
	}

	StaticCtrl::StaticCtrl( SceneRPtr scene
		, String const & name
		, StaticStyle * style
		, ControlRPtr parent
		, U32String const & caption
		, Position const & position
		, Size const & size
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
		setBorderSize( Point4ui{} );
		auto text = m_scene
			? m_scene->addNewOverlay( getName() + cuT( "/Text" )
				, getEngine()
				, OverlayType::eText
				, &getBackgroundOverlay() )->getTextOverlay()
			: getEngine().addNewOverlay( getName() + cuT( "/Text" )
				, getEngine()
				, OverlayType::eText
				, &getBackgroundOverlay() )->getTextOverlay();
		m_text = text;
		text->setPixelSize( getClientSize() );
		text->setCaption( m_caption );
		text->setVisible( visible );
		text->setVAlign( VAlign::eCenter );

		setStyle( style );
		doUpdateTextAlign();
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
		if ( checkFlag( getFlags(), StaticFlag::eHAlignCenter ) )
		{
			return HAlign::eCenter;
		}

		if ( checkFlag( getFlags(), StaticFlag::eHAlignRight ) )
		{
			return HAlign::eRight;
		}

		return HAlign::eLeft;
	}

	VAlign StaticCtrl::getVAlign()const
	{
		if ( checkFlag( getFlags(), StaticFlag::eVAlignCenter ) )
		{
			return VAlign::eCenter;
		}

		if ( checkFlag( getFlags(), StaticFlag::eVAlignBottom ) )
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

	void StaticCtrl::doSetPosition( Position const & value )
	{
		if ( auto text = m_text )
		{
			text->setPixelPosition( getClientOffset() );
		}
	}

	void StaticCtrl::doSetSize( Size const & value )
	{
		if ( auto text = m_text )
		{
			text->setPixelSize( getClientSize() );
		}
	}

	void StaticCtrl::doSetBorderSize( Point4ui const & value )
	{
		if ( auto text = m_text )
		{
			text->setPixelPosition( getClientOffset() );
			text->setPixelSize( getClientSize() );
		}
	}

	void StaticCtrl::doSetCaption( U32String const & value )
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
		doUpdateTextAlign();
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

	void StaticCtrl::doUpdateTextAlign()
	{
		if ( auto text = m_text )
		{
			text->setHAlign( getHAlign() );
			text->setVAlign( getVAlign() );
		}
	}
}

#include "Castor3D/Gui/Controls/CtrlStatic.hpp"

#include "Castor3D/Gui/ControlsManager.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Graphics/Font.hpp>

CU_ImplementCUSmartPtr( castor3d, StaticCtrl )

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
			, castor::String{}
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
		, castor::String const & caption
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
		setBackgroundBorderSize( castor::Point4ui{} );
		auto text = m_scene
			? m_scene->addNewOverlay( cuT( "T_CtrlStatic_[" ) + getName() + cuT( "]" )
				, getEngine()
				, OverlayType::eText
				, nullptr
				, &getBackgroundOverlay() ).lock()->getTextOverlay()
			: getEngine().addNewOverlay( cuT( "T_CtrlStatic_[" ) + getName() + cuT( "]" )
				, getEngine()
				, OverlayType::eText
				, nullptr
				, &getBackgroundOverlay() ).lock()->getTextOverlay();
		m_text = text;
		text->setPixelSize( getSize() );
		text->setCaption( castor::string::toU32String( m_caption ) );
		text->setVisible( visible );
		text->setVAlign( VAlign::eCenter );
		doUpdateStyle();
		doUpdateFlags();
	}

	void StaticCtrl::setHAlign( HAlign align )
	{
		if ( auto text = m_text.lock() )
		{
			text->setHAlign( align );
		}
	}

	void StaticCtrl::setVAlign( VAlign align )
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
			text->setCaption( castor::string::toU32String( value ) );
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
				text->setHAlign( HAlign::eCenter );
			}
			else if ( castor::checkFlag( getFlags(), StaticFlag::eHAlignRight ) )
			{
				text->setHAlign( HAlign::eRight );
			}
			else
			{
				text->setHAlign( HAlign::eLeft );
			}

			if ( castor::checkFlag( getFlags(), StaticFlag::eVAlignCenter ) )
			{
				text->setVAlign( VAlign::eCenter );
			}
			else if ( castor::checkFlag( getFlags(), StaticFlag::eVAlignBottom ) )
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

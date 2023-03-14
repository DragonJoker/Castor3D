#include "Castor3D/Gui/Controls/CtrlButton.hpp"

#include "Castor3D/Gui/ControlsManager.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Graphics/Font.hpp>

CU_ImplementCUSmartPtr( castor3d, ButtonCtrl )

namespace castor3d
{
	ButtonCtrl::ButtonCtrl( SceneRPtr scene
		, castor::String const & name
		, ButtonStyleRPtr style
		, ControlRPtr parent )
		: ButtonCtrl{ scene
			, name
			, style
			, parent
			, castor::String{}
			, castor::Position{}
			, castor::Size{}
			, 0u
			, true }
	{
	}

	ButtonCtrl::ButtonCtrl( SceneRPtr scene
		, castor::String const & name
		, ButtonStyleRPtr style
		, ControlRPtr parent
		, castor::String const & caption
		, castor::Position const & position
		, castor::Size const & size
		, uint64_t flags
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
		, m_onEnable{ EventHandler::onEnable.connect( [this]( bool penabled )
			{
				auto & mystyle = getStyle();

				if ( penabled )
				{
					m_text.lock()->setMaterial( mystyle.getTextMaterial() );

					if ( auto panel = getBackground() )
					{
						panel->setMaterial( mystyle.getBackgroundMaterial() );
						panel->setBorderMaterial( mystyle.getForegroundMaterial() );
					}
				}
				else
				{
					m_text.lock()->setMaterial( mystyle.getDisabledTextMaterial() );

					if ( auto panel = getBackground() )
					{
						panel->setMaterial( mystyle.getDisabledBackgroundMaterial() );
						panel->setBorderMaterial( mystyle.getDisabledForegroundMaterial() );
					}
				}
			} ) }
	{
		setBackgroundBorders( castor::Point4ui{ 1, 1, 1, 1 } );
		EventHandler::connect( MouseEventType::eEnter
			, [this]( MouseEvent const & event )
			{
				onMouseEnter( event );
			} );
		EventHandler::connect( MouseEventType::eLeave
			, [this]( MouseEvent const & event )
			{
				onMouseLeave( event );
			} );
		EventHandler::connect( MouseEventType::ePushed
			, [this]( MouseEvent const & event )
			{
				onMouseButtonDown( event );
			} );
		EventHandler::connect( MouseEventType::eReleased
			, [this]( MouseEvent const & event )
			{
				onMouseButtonUp( event );
			} );

		auto text = m_scene
			? m_scene->addNewOverlay( cuT( "T_CtrlButton_[" ) + getName() + cuT( "]" )
				, getEngine()
				, OverlayType::eText
				, nullptr
				, &getBackground()->getOverlay() ).lock()->getTextOverlay()
			: getEngine().addNewOverlay( cuT( "T_CtrlButton_[" ) + getName() + cuT( "]" )
				, getEngine()
				, OverlayType::eText
				, nullptr
				, &getBackground()->getOverlay() ).lock()->getTextOverlay();
		text->setPixelSize( getSize() );
		text->setHAlign( HAlign::eCenter );
		text->setVAlign( VAlign::eCenter );
		text->setCaption( castor::string::toU32String( m_caption ) );
		text->setVisible( visible );
		m_text = text;

		doUpdateStyle();
	}

	void ButtonCtrl::setHAlign( HAlign align )
	{
		if ( auto text = m_text.lock() )
		{
			text->setHAlign( align );
		}
	}

	void ButtonCtrl::setVAlign( VAlign align )
	{
		if ( auto text = m_text.lock() )
		{
			text->setVAlign( align );
		}
	}

	void ButtonCtrl::doUpdateStyle()
	{
		auto & style = getStyle();

		if ( auto text = m_text.lock() )
		{
			text->setFont( style.getFontName() );
		}
	}

	void ButtonCtrl::doCreate()
	{
		auto bg = getBackground();

		if ( !bg )
		{
			CU_Exception( "No background set" );
		}

		bg->setBorderPosition( BorderPosition::eInternal );

		auto & style = getStyle();
		auto text = m_text.lock();
		text->setMaterial( style.getTextMaterial() );

		if ( !text->getFontTexture() || !text->getFontTexture()->getFont() )
		{
			text->setFont( style.getFontName() );
		}

		getControlsManager()->connectEvents( *this );
	}

	void ButtonCtrl::doDestroy()
	{
		getControlsManager()->disconnectEvents( *this );
	}

	void ButtonCtrl::doSetPosition( castor::Position const & value )
	{
		if ( auto text = m_text.lock() )
		{
			text->setPixelPosition( castor::Position{} );
			text.reset();
		}
	}

	void ButtonCtrl::doSetSize( castor::Size const & value )
	{
		if ( auto text = m_text.lock() )
		{
			text->setPixelSize( value );
		}
	}

	void ButtonCtrl::doSetCaption( castor::String const & value )
	{
		m_caption = value;

		if ( auto text = m_text.lock() )
		{
			text->setCaption( castor::string::toU32String( value ) );
		}
	}

	void ButtonCtrl::doSetVisible( bool visible )
	{
		if ( auto text = m_text.lock() )
		{
			text->setVisible( visible );
		}
	}

	void ButtonCtrl::onMouseEnter( MouseEvent const & event )
	{
		auto & style = getStyle();
		m_text.lock()->setMaterial( style.getHighlightedTextMaterial() );

		if ( auto panel = getBackground() )
		{
			panel->setMaterial( style.getHighlightedBackgroundMaterial() );
			panel->setBorderMaterial( style.getHighlightedForegroundMaterial() );
		}
	}

	void ButtonCtrl::onMouseLeave( MouseEvent const & event )
	{
		auto & style = getStyle();
		m_text.lock()->setMaterial( style.getTextMaterial() );

		if ( auto panel = getBackground() )
		{
			panel->setMaterial( style.getBackgroundMaterial() );
			panel->setBorderMaterial( style.getForegroundMaterial() );
		}
	}

	void ButtonCtrl::onMouseButtonDown( MouseEvent const & event )
	{
		if ( event.getButton() == MouseButton::eLeft )
		{
			auto & style = getStyle();
			m_text.lock()->setMaterial( style.getPushedTextMaterial() );

			if ( auto panel = getBackground() )
			{
				panel->setMaterial( style.getPushedBackgroundMaterial() );
				panel->setBorderMaterial( style.getPushedForegroundMaterial() );
				panel.reset();
			}
		}
	}

	void ButtonCtrl::onMouseButtonUp( MouseEvent const & event )
	{
		if ( event.getButton() == MouseButton::eLeft )
		{
			auto & style = getStyle();
			m_text.lock()->setMaterial( style.getHighlightedTextMaterial() );

			if ( auto panel = getBackground() )
			{
				panel->setMaterial( style.getHighlightedBackgroundMaterial() );
				panel->setBorderMaterial( style.getHighlightedForegroundMaterial() );
				panel.reset();
			}

			m_signals[size_t( ButtonEvent::eClicked )]();
		}
	}
}

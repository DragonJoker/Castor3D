#include "CastorGui/Controls/CtrlControl.hpp"

#include "CastorGui/ControlsManager.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/OverlayCache.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Overlay/Overlay.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Overlay/TextOverlay.hpp>

namespace CastorGui
{
	Control::Control( ControlType type
		, castor::String const & name
		, ControlStyleRPtr style
		, ControlRPtr parent
		, uint32_t id
		, castor::Position const & position
		, castor::Size const & size
		, uint32_t flags
		, bool visible )
		: castor3d::NonClientEventHandler< Control >{ type != ControlType::eStatic }
		, castor::Named{ name }
		, m_parent{ parent }
		, m_id{ id }
		, m_type{ type }
		, m_style{ style }
		, m_flags{ flags }
		, m_position{ position }
		, m_size{ size }
	{
		castor3d::OverlayRPtr parentOv{};

		if ( m_parent )
		{
			auto bg = m_parent->getBackground();

			if ( !bg )
			{
				CU_Exception( "No background set" );
			}

			parentOv = &bg->getOverlay();
		}

		auto overlay = getEngine().getOverlayCache().add( name
			, getEngine()
			, castor3d::OverlayType::eBorderPanel
			, nullptr
			, parentOv ).lock();

		if ( !overlay )
		{
			CU_Exception( "No background set" );
		}

		overlay->setPixelPosition( getPosition() );
		overlay->setPixelSize( getSize() );
		auto panel = overlay->getBorderPanelOverlay();
		panel->setBorderPosition( castor3d::BorderPosition::eInternal );
		panel->setVisible( visible );
		m_background = panel;
	}

	void Control::setStyle( ControlStyleRPtr value )
	{
		m_style = value;

		if ( auto background = getBackground() )
		{
			background->setMaterial( m_style->getBackgroundMaterial() );
			background->setBorderMaterial( m_style->getForegroundMaterial() );
		}

		doUpdateStyle();
	}

	void Control::create( ControlsManagerSPtr ctrlManager )
	{
		m_ctrlManager = ctrlManager;
		ControlRPtr parent = getParent();

		if ( parent )
		{
			parent->m_children.push_back( std::static_pointer_cast< Control >( shared_from_this() ) );
		}

		if ( auto background = getBackground() )
		{
			background->setMaterial( m_style->getBackgroundMaterial() );
			background->setBorderMaterial( m_style->getForegroundMaterial() );
			background->setBorderPixelSize( m_borders );
		}

		doCreate();
	}

	void Control::destroy()
	{
		doDestroy();
	}

	void Control::setBackgroundBorders( castor::Rectangle const & value )
	{
		m_borders = value;

		if ( auto background = getBackground() )
		{
			background->setBorderPixelSize( m_borders );
		}
	}

	void Control::setPosition( castor::Position const & value )
	{
		m_position = value;

		if ( auto background = getBackground() )
		{
			background->setPixelPosition( m_position );
		}

		doSetPosition( m_position );
	}

	castor::Position Control::getAbsolutePosition()const
	{
		ControlRPtr parent = getParent();
		auto result = m_position;

		if ( parent )
		{
			result += parent->getAbsolutePosition();
		}

		return result;
	}

	void Control::setSize( castor::Size const & value )
	{
		m_size = value;

		if ( auto background = getBackground() )
		{
			background->setPixelSize( m_size );
		}

		doSetSize( m_size );
	}

	void Control::setCaption( castor::String const & caption )
	{
		doSetCaption( caption );
	}

	void Control::setVisible( bool value )
	{
		if ( auto background = getBackground() )
		{
			background->setVisible( value );
		}
		else
		{
			CU_Exception( "No background set" );
		}

		doSetVisible( value );
	}

	bool Control::isVisible()const
	{
		bool visible = false;

		if ( auto background = getBackground() )
		{
			visible = background->isVisible();
			ControlRPtr parent = getParent();

			if ( visible && parent )
			{
				visible = parent->isVisible();
			}
		}
		else
		{
			CU_Exception( "No background set" );
		}

		return visible;
	}

	ControlSPtr Control::getChildControl( uint32_t id )
	{
		auto it = std::find_if( std::begin( m_children )
			, std::end( m_children )
			, [&id]( ControlWPtr lookup )
			{
				auto ctrl = lookup.lock();
				return ctrl
					? ( ctrl->getId() == id )
					: false;
			} );

		if ( it == m_children.end() )
		{
			CU_Exception( "This control does not exist in my childs" );
		}

		return it->lock();
	}

	void Control::addFlag( uint32_t flags )
	{
		m_flags |= flags;
		doUpdateFlags();
	}

	void Control::removeFlag( uint32_t flags )
	{
		m_flags &= ~flags;
		doUpdateFlags();
	}

	bool Control::doIsVisible()const
	{
		bool result = false;

		if ( auto background = getBackground() )
		{
			result = background->isVisible();
		}
		else
		{
			CU_Exception( "No background set" );
		}

		return result;
	}
}

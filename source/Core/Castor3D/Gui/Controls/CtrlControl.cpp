#include "Castor3D/Gui/Controls/CtrlControl.hpp"

#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Gui/Layout/Layout.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Scene/Scene.hpp"

namespace castor3d
{
	Control::Control( ControlType type
		, SceneRPtr scene
		, castor::String const & name
		, ControlStyleRPtr style
		, ControlRPtr parent
		, castor::Position const & position
		, castor::Size const & size
		, uint64_t flags
		, bool visible )
		: NonClientEventHandler< Control >{ name, type != ControlType::eStatic }
		, m_scene{ scene }
		, m_parent{ parent }
		, m_id{ std::hash< castor::String >{}( getName() ) }
		, m_type{ type }
		, m_style{ style }
		, m_flags{ flags }
		, m_position{ position }
		, m_size{ size }
	{
		OverlayRPtr parentOv{};

		if ( m_parent )
		{
			auto bg = m_parent->getBackground();

			if ( !bg )
			{
				CU_SrcException( "Control", "No background set" );
			}

			parentOv = &bg->getOverlay();
		}

		auto overlay = m_scene
			? m_scene->addNewOverlay( cuT( "T_Ctrl_[" ) + getName() + cuT( "]" )
				, getEngine()
				, OverlayType::eBorderPanel
				, nullptr
				, parentOv ).lock()
			: getEngine().addNewOverlay( cuT( "T_Ctrl_[" ) + getName() + cuT( "]" )
				, getEngine()
				, OverlayType::eBorderPanel
				, nullptr
				, parentOv ).lock();

		if ( !overlay )
		{
			CU_SrcException( "Control", "Couldn't create background overlay" );
		}

		overlay->setPixelPosition( getPosition() );
		overlay->setPixelSize( getSize() );
		auto panel = overlay->getBorderPanelOverlay();
		panel->setBorderPosition( BorderPosition::eInternal );
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
			background->setAbsoluteBorderSize( m_borders );
		}

		doCreate();
	}

	void Control::destroy()
	{
		doDestroy();
	}

	void Control::setBackgroundBorders( castor::Point4ui const & value )
	{
		m_borders = value;

		if ( auto background = getBackground() )
		{
			background->setAbsoluteBorderSize( m_borders );
		}

		doSetBackgroundBorders( m_borders );
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
			CU_SrcException( "Control", "No background set" );
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
			CU_SrcException( "Control", "No background set" );
		}

		return visible;
	}

	ControlSPtr Control::getChildControl( ControlID id )
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
			return nullptr;
		}

		return it->lock();
	}

	void Control::addFlag( uint64_t flags )
	{
		m_flags |= flags;
		doUpdateFlags();
	}

	void Control::removeFlag( uint64_t flags )
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
			CU_SrcException( "Control", "No background set" );
		}

		return result;
	}
}

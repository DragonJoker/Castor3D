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

CU_ImplementCUSmartPtr( castor3d, Control )

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
		: NonClientEventHandler< Control >{ ( parent ? parent->getName() + "/" + name : name ), type != ControlType::eStatic }
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

		if ( !isAlwaysOnTop() && m_parent )
		{
			parentOv = &m_parent->doGetBackground().getOverlay();
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

		if ( isAlwaysOnTop() )
		{
			overlay->getCategory()->setOrder( 50000u, 0u );
			overlay->setPixelPosition( getAbsolutePosition() );
		}
		else
		{
			overlay->setPixelPosition( getPosition() );
		}

		overlay->setPixelSize( getSize() );
		auto panel = overlay->getBorderPanelOverlay();
		panel->setBorderPosition( BorderPosition::eInternal );
		panel->setVisible( visible );
		m_background = panel;
	}

	void Control::setStyle( ControlStyleRPtr value )
	{
		m_style = value;
		doGetBackground().setMaterial( m_style->getBackgroundMaterial() );
		doGetBackground().setBorderMaterial( m_style->getForegroundMaterial() );
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

		doGetBackground().setMaterial( m_style->getBackgroundMaterial() );
		doGetBackground().setBorderMaterial( m_style->getForegroundMaterial() );
		doGetBackground().setAbsoluteBorderSize( m_borders );

		doCreate();
	}

	void Control::destroy()
	{
		doDestroy();
	}

	Overlay & Control::getBackgroundOverlay()
	{
		return doGetBackground().getOverlay();
	}

	void Control::setBackgroundMaterial( MaterialRPtr value )
	{
		doGetBackground().setMaterial( value );
	}

	void Control::setBackgroundSize( castor::Size const & value )
	{
		doGetBackground().setPixelSize( value );
	}

	void Control::setBackgroundUV( castor::Point4d const & value )
	{
		doGetBackground().setUV( value );
	}

	void Control::setBackgroundBorderPosition( BorderPosition value )
	{
		doGetBackground().setBorderPosition( value );
	}

	void Control::setBackgroundBorderMaterial( MaterialRPtr value )
	{
		doGetBackground().setBorderMaterial( value );
	}

	void Control::setBackgroundBorderSize( castor::Point4ui const & value )
	{
		m_borders = value;
		doGetBackground().setAbsoluteBorderSize( m_borders );
		doSetBackgroundBorders( m_borders );
		onChanged( *this );
	}

	void Control::setBackgroundBorderInnerUV( castor::Point4d const & value )
	{
		doGetBackground().setBorderInnerUV( value );
	}

	void Control::setBackgroundBorderOuterUV( castor::Point4d const & value )
	{
		doGetBackground().setBorderOuterUV( value );
	}

	void Control::setPosition( castor::Position const & value )
	{
		m_position = value;

		if ( isAlwaysOnTop() )
		{
			doGetBackground().setPixelPosition( getAbsolutePosition() );
		}
		else
		{
			doGetBackground().setPixelPosition( getPosition() );
		}

		doSetPosition( m_position );
		onChanged( *this );
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
		doGetBackground().setPixelSize( m_size );
		doSetSize( m_size );
		onChanged( *this );
	}

	void Control::setCaption( castor::String const & caption )
	{
		doSetCaption( caption );
	}

	void Control::setVisible( bool value )
	{
		doGetBackground().setVisible( value );
		doSetVisible( value );
		onChanged( *this );
	}

	bool Control::isVisible()const
	{
		auto visible = doGetBackground().isVisible();
		ControlRPtr parent = getParent();

		if ( visible && parent )
		{
			visible = parent->isVisible();
		}

		return visible;
	}

	ControlSPtr Control::getChildControl( ControlID id )const
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

	uint64_t Control::getZIndex()const
	{
		return doGetBackground().getIndex() + doGetBackground().getLevel() * 1000ull;
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
		return doGetBackground().isVisible();
	}
}

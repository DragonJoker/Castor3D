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
	static int32_t constexpr ResizeBorderSize = 10;

	Control::Control( ControlType type
		, SceneRPtr scene
		, castor::String const & name
		, ControlStyleRPtr style
		, ControlRPtr parent
		, castor::Position const & position
		, castor::Size const & size
		, ControlFlagType flags
		, bool visible )
		: NonClientEventHandler< Control >{ ( parent ? parent->getName() + "/" + name : name ), type != ControlType::eStatic }
		, m_scene{ scene }
		, m_parent{ parent }
		, m_flags{ flags }
		, m_id{ std::hash< castor::String >{}( getName() ) }
		, m_type{ type }
		, m_style{ style }
		, m_position{ position }
		, m_size{ size }
	{
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
		EventHandler::connect( MouseEventType::eMove
			, [this]( MouseEvent const & event )
			{
				onMouseMove( event );
			} );
		EventHandler::connect( MouseEventType::eLeave
			, [this]( MouseEvent const & event )
			{
				onMouseLeave( event );
			} );
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
		setBackgroundMaterial( m_style->getBackgroundMaterial() );
		setBackgroundBorderMaterial( m_style->getForegroundMaterial() );
		doUpdateStyle();
	}

	void Control::setSize( castor::Size const & value )
	{
		m_size = value;
		doGetBackground().setPixelSize( m_size );
		doSetSize( m_size );
		onChanged( *this );
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

	void Control::setUV( castor::Point4d const & value )
	{
		doGetBackground().setUV( value );
	}

	void Control::setBorderSize( castor::Point4ui const & value )
	{
		m_borders = value;
		doGetBackground().setAbsoluteBorderSize( m_borders );
		doSetBackgroundBorders( m_borders );
		onChanged( *this );
	}

	void Control::setBorderInnerUV( castor::Point4d const & value )
	{
		doGetBackground().setBorderInnerUV( value );
	}

	void Control::setBorderOuterUV( castor::Point4d const & value )
	{
		doGetBackground().setBorderOuterUV( value );
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

	bool Control::isVisible()const
	{
		auto visible = doIsVisible();
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

	Overlay & Control::getBackgroundOverlay()
	{
		return doGetBackground().getOverlay();
	}

	void Control::setBackgroundMaterial( MaterialRPtr value )
	{
		if ( !m_style->isBackgroundInvisible() )
		{
			doGetBackground().setMaterial( value );
		}
	}

	void Control::setBackgroundSize( castor::Size const & value )
	{
		doGetBackground().setPixelSize( value );
	}

	void Control::setBackgroundBorderPosition( BorderPosition value )
	{
		doGetBackground().setBorderPosition( value );
	}

	void Control::setBackgroundBorderMaterial( MaterialRPtr value )
	{
		if ( !m_style->isForegroundInvisible() )
		{
			doGetBackground().setBorderMaterial( value );
		}
	}

	void Control::create( ControlsManagerSPtr ctrlManager )
	{
		m_ctrlManager = ctrlManager;
		ControlRPtr parent = getParent();

		if ( parent )
		{
			parent->m_children.push_back( std::static_pointer_cast< Control >( shared_from_this() ) );
		}

		setBackgroundMaterial( m_style->getBackgroundMaterial() );
		setBackgroundBorderMaterial( m_style->getForegroundMaterial() );
		doGetBackground().setAbsoluteBorderSize( m_borders );

		doCreate();
	}

	void Control::destroy()
	{
		doDestroy();
	}

	bool Control::doIsVisible()const
	{
		return doGetBackground().isVisible();
	}

	void Control::adjustZIndex( uint32_t index )
	{
		auto level = doGetBackground().getLevel();
		doGetBackground().setOrder( level + index, 0u );
	}

	void Control::updateZIndex( uint32_t & index
		, std::vector< Control * > & controls
		, std::vector< Control * > & topControls )
	{
		bool hasMovable = std::any_of( m_children.begin()
			, m_children.end()
			, []( auto const & lookup )
			{
				auto control = lookup.lock();
				return control ? control->isMovable() : false;
			} );

		auto realIndex = &index;
		uint32_t zindex{};

		if ( isAlwaysOnTop() )
		{
			zindex = index;
			realIndex = &zindex;
			topControls.push_back( this );
		}
		else
		{
			controls.push_back( this );
		}

		if ( hasMovable )
		{
			doGetBackground().setOrder( ( *realIndex )++, 0u );
			doUpdateZIndex( ( *realIndex ) );

			for ( auto child : m_children )
			{
				if ( auto control = child.lock() )
				{
					control->updateZIndex( ( *realIndex ), controls, topControls );
				}
			}
		}
		else
		{
			doGetBackground().setOrder( ( *realIndex )++, 0u );
			auto findex = *realIndex;
			doUpdateZIndex( findex );
			auto maxIndex = findex;

			for ( auto child : m_children )
			{
				if ( auto control = child.lock() )
				{
					findex = *realIndex;
					control->updateZIndex( findex, controls, topControls );
					maxIndex = std::max( findex, maxIndex );
				}
			}

			*realIndex = maxIndex;
		}
	}

	void Control::onMouseButtonDown( MouseEvent const & event )
	{
		bool processed{};

		if ( event.getButton() == MouseButton::eLeft )
		{
			if ( isMovable() && beginMove( event ) )
			{
				processed = true;
			}
			else if ( isResizable() && beginResize( event ) )
			{
				processed = true;
			}
		}

		if ( !processed )
		{
			doOnMouseButtonDown( event );
		}
	}

	void Control::onMouseButtonUp( MouseEvent const & event )
	{
		bool processed{};

		if ( event.getButton() == MouseButton::eLeft )
		{
			if ( isMovable() && beginMove( event ) )
			{
				endMove( event );
				processed = true;
			}
			else if ( isResizable() && beginResize( event ) )
			{
				endResize( event );
				processed = true;
			}
		}

		if ( !processed )
		{
			doOnMouseButtonUp( event );
		}
	}

	void Control::onMouseMove( MouseEvent const & event )
	{
		doOnMouseMove( event );

		if ( isMovable() && isMoving() )
		{
			move( event );
		}
		else if ( isResizable() && isResizing() )
		{
			resize( event );
		}
	}

	void Control::onMouseLeave( MouseEvent const & event )
	{
		doOnMouseLeave( event );

		if ( isMovable() && isMoving() )
		{
			endMove( event );
		}
		else if ( isResizable() && isResizing() )
		{
			endResize( event );
		}
	}

	bool Control::beginMove( MouseEvent const & event )
	{
		auto size = getSize();
		auto pos = event.getPosition() - getAbsolutePosition();
		auto result = pos->x > ResizeBorderSize
			&& pos->x < int32_t( size->x ) - ResizeBorderSize
			&& pos->y > ResizeBorderSize
			&& pos->y < int32_t( size->y ) - ResizeBorderSize;
			
		if ( result )
		{
			result = getControlsManager()->setMovedControl( this );

			if ( result )
			{
				m_moving = true;
				m_mouseStartPosition = getPosition();
				m_mouseStartMousePosition = event.getPosition();
			}
		}

		return result;
	}

	void Control::move( MouseEvent const & event )
	{
		auto diff = event.getPosition() - m_mouseStartMousePosition;
		auto newPos = m_mouseStartPosition + diff;
		setPosition( { newPos->x, newPos->y } );
	}

	void Control::endMove( MouseEvent const & event )
	{
		m_moving = false;
		getControlsManager()->setMovedControl( nullptr );
	}

	bool Control::beginResize( MouseEvent const & event )
	{
		auto size = getSize();
		auto pos = event.getPosition() - getAbsolutePosition();
		auto result = pos->x <= ResizeBorderSize
			|| pos->x >= int32_t( size->x ) - ResizeBorderSize
			|| pos->y <= ResizeBorderSize
			|| pos->y >= int32_t( size->y ) - ResizeBorderSize;

		if ( result )
		{
			result = getControlsManager()->setResizedControl( this );

			if ( result )
			{
				m_resizing[0] = pos->x <= ResizeBorderSize;
				m_resizing[1] = pos->y <= ResizeBorderSize;
				m_resizing[2] = pos->x >= int32_t( size->x ) - ResizeBorderSize;
				m_resizing[3] = pos->y >= int32_t( size->y ) - ResizeBorderSize;
				m_mouseStartSize = getSize();
				m_mouseStartPosition = getPosition();
				m_mouseStartMousePosition = event.getPosition();
			}
		}

		return result;
	}

	void Control::resize( MouseEvent const & event )
	{
		auto diff = event.getPosition() - m_mouseStartMousePosition;
		castor::Point2i newPos{ m_mouseStartPosition.x(), m_mouseStartPosition.y() };

		if ( m_resizing[0] )
		{
			newPos->x += diff->x;
			diff->x = -diff->x;
		}
		else if ( !m_resizing[2] )
		{
			diff->x = 0;
		}

		if ( m_resizing[1] )
		{
			newPos->y += diff->y;
			diff->y = -diff->y;
		}
		else if ( !m_resizing[3] )
		{
			diff->y = 0;
		}

		auto newSize = m_mouseStartSize + diff;
		setSize( { newSize->x, newSize->y } );
		setPosition( { newPos->x, newPos->y } );
	}

	void Control::endResize( MouseEvent const & event )
	{
		m_resizing[0] = false;
		m_resizing[1] = false;
		m_resizing[2] = false;
		m_resizing[3] = false;
		getControlsManager()->setResizedControl( nullptr );
	}
}

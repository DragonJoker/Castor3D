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
		, m_engine{ style->getEngine() }
		, m_style{ style }
		, m_position{ position }
		, m_size{ size }
		, m_resizeBorderSize{ ResizeBorderSize }
	{
		updateClientRect();
		EventHandler::connect( MouseEventType::eEnter
			, [this]( MouseEvent const & event )
			{
				onMouseEnter( event );
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
			? m_scene->addNewOverlay( getName()
				, getEngine()
				, OverlayType::eBorderPanel
				, nullptr
				, parentOv ).lock()
			: getEngine().addNewOverlay( getName()
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
			overlay->getCategory().setOrder( 50000u, 0u );
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

	Control::~Control()noexcept
	{
		if ( auto overlay = m_background )
		{
			if ( m_scene )
			{
				m_scene->removeOverlay( getName(), true );
			}
			else
			{
				getEngine().removeOverlay( getName(), true );
			}
		}
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
		m_size = doUpdateSize( value );
		updateClientRect();
		doGetBackground().setPixelSize( m_size );
		doSetSize( m_size );
		onChanged( *this );
	}

	void Control::setPosition( castor::Position const & value )
	{
		m_position = doUpdatePosition( value );
		updateClientRect();

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
		m_borders = doUpdateBorderSize( value );
		updateClientRect();
		doGetBackground().setPixelBorderSize( m_borders );
		doSetBorderSize( m_borders );
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

	void Control::setCaption( castor::U32String const & caption )
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
		auto visible = isBackgroundVisible();
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

	std::array< bool, 4u > Control::isInResizeRange( castor::Position const & position )const
	{
		auto size = getSize();
		auto pos = position - getAbsolutePosition();
		bool isOnW = pos->x <= int32_t( m_resizeBorderSize );
		bool isOnE = pos->x + int32_t( m_resizeBorderSize ) >= int32_t( size->x );
		bool isOnN = pos->y <= int32_t( m_resizeBorderSize );
		bool isOnS = pos->y + int32_t( m_resizeBorderSize ) >= int32_t( size->y );
		return { isOnN, isOnW, isOnS, isOnE };
	}

	castor::Point4d const & Control::getBorderInnerUV()const
	{
		return doGetBackground().getBorderInnerUV();
	}

	castor::Point4d const & Control::getBorderOuterUV()const
	{
		return doGetBackground().getBorderOuterUV();
	}

	BorderPosition Control::getBorderPosition()const
	{
		return doGetBackground().getBorderPosition();
	}

	castor::Point4d const & Control::getUV()const
	{
		return doGetBackground().getUV();
	}

	bool Control::isBackgroundVisible()const
	{
		return doGetBackground().isVisible();
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

		if ( ControlRPtr parent = getParent() )
		{
			parent->addChild( shared_from_this() );
		}

		setBackgroundMaterial( m_style->getBackgroundMaterial() );
		setBackgroundBorderMaterial( m_style->getForegroundMaterial() );
		doGetBackground().setPixelBorderSize( m_borders );

		doCreate();
	}

	void Control::destroy()
	{
		doDestroy();
	}

	void Control::addChild( ControlSPtr control )
	{
		m_children.push_back( control );
		doAddChild( control );
	}

	void Control::adjustZIndex( uint32_t offset )
	{
		auto level = doGetBackground().getLevel();
		doGetBackground().setOrder( level + offset, 0u );
		doAdjustZIndex( offset );

		for ( auto child : m_children )
		{
			if ( auto control = child.lock() )
			{
				control->adjustZIndex( offset );
			}
		}
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
			std::vector< Control * > scrollbars;

			for ( auto child : m_children )
			{
				if ( auto control = child.lock() )
				{
					if ( control->getType() == ControlType::eScrollBar )
					{
						scrollbars.push_back( control.get() );
					}
					else
					{
						control->updateZIndex( ( *realIndex ), controls, topControls );
					}
				}
			}

			for ( auto control : scrollbars )
			{
				control->updateZIndex( ( *realIndex ), controls, topControls );
			}
		}
		else
		{
			doGetBackground().setOrder( ( *realIndex )++, 0u );
			auto findex = *realIndex;
			doUpdateZIndex( findex );
			std::vector< Control * > scrollbars;
			auto maxIndex = findex;

			for ( auto child : m_children )
			{
				if ( auto control = child.lock() )
				{
					if ( control->getType() == ControlType::eScrollBar )
					{
						scrollbars.push_back( control.get() );
					}
					else
					{
						findex = *realIndex;
						control->updateZIndex( findex, controls, topControls );
						maxIndex = std::max( findex, maxIndex );
					}
				}
			}

			for ( auto control : scrollbars )
			{
				findex = maxIndex;
				control->updateZIndex( findex, controls, topControls );
				maxIndex = std::max( findex, maxIndex );
			}

			*realIndex = maxIndex;
		}
	}

	void Control::onMouseEnter( MouseEvent const & event )
	{
		doOnMouseEnter( event );
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
			if ( isMovable() && isMoving() )
			{
				getControlsManager()->setMovedControl( nullptr, event );
				processed = true;
			}
			else if ( isResizable() && isResizing() )
			{
				getControlsManager()->setResizedControl( nullptr, event );
				processed = true;
			}
			else
			{
				getControlsManager()->setMovedControl( nullptr, event );
				getControlsManager()->setResizedControl( nullptr, event );
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
			getControlsManager()->setMovedControl( nullptr, event );
		}
		else if ( isResizable() && isResizing() )
		{
			getControlsManager()->setResizedControl( nullptr, event );
		}
	}

	bool Control::beginMove( MouseEvent const & event )
	{
		auto [isOnN, isOnW, isOnS, isOnE] = isInResizeRange( event.getPosition() );
		auto result = !isOnN && !isOnW && !isOnS && !isOnE;
			
		if ( result )
		{
			result = getControlsManager()->setMovedControl( this, event );

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
	}

	bool Control::beginResize( MouseEvent const & event )
	{
		auto size = getSize();
		auto [isOnN, isOnW, isOnS, isOnE] = isInResizeRange( event.getPosition() );
		auto result = isOnN || isOnW || isOnS || isOnE;

		if ( result )
		{
			result = getControlsManager()->setResizedControl( this, event );

			if ( result )
			{
				m_resizingN = isOnN;
				m_resizingW = isOnW;
				m_resizingS = isOnS;
				m_resizingE = isOnE;
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

		if ( m_resizingW )
		{
			newPos->x += diff->x;
			diff->x = -diff->x;
		}
		else if ( !m_resizingE )
		{
			diff->x = 0;
		}

		if ( m_resizingN )
		{
			newPos->y += diff->y;
			diff->y = -diff->y;
		}
		else if ( !m_resizingS )
		{
			diff->y = 0;
		}

		auto bordersWidth = int32_t( getBorderSize()->x + getBorderSize()->z );
		auto bordersHeight = int32_t( getBorderSize()->y + getBorderSize()->w );

		if ( auto background = m_background )
		{
			if ( background->getBorderPosition() == BorderPosition::eMiddle )
			{
				bordersWidth /= 2;
				bordersHeight /= 2;
			}
			else if ( background->getBorderPosition() == BorderPosition::eExternal )
			{
				bordersWidth = 1;
				bordersHeight = 1;
			}
		}

		if ( diff->x <= bordersWidth - int32_t( m_mouseStartSize->x ) )
		{
			diff->x = bordersWidth - int32_t( m_mouseStartSize->x );
		}

		if ( diff->y <= bordersHeight - int32_t( m_mouseStartSize->y ) )
		{
			diff->y = bordersHeight - int32_t( m_mouseStartSize->y );
		}

		auto newSize = m_mouseStartSize + diff;
		setPosition( { newPos->x, newPos->y } );
		diff = newPos - getPosition();
		setSize( { uint32_t( newSize->x + diff->x )
			, uint32_t( newSize->y + diff->y ) } );
	}

	void Control::endResize( MouseEvent const & event )
	{
		m_resizingN = false;
		m_resizingW = false;
		m_resizingS = false;
		m_resizingE = false;
	}

	void Control::updateClientRect()
	{
		auto & borders = getBorderSize();
		auto bordersWidth = borders->x + borders->z;
		auto bordersHeight = borders->y + borders->w;

		if ( auto background = m_background )
		{
			if ( background->getBorderPosition() == BorderPosition::eMiddle )
			{
				bordersWidth /= 2;
				bordersHeight /= 2;
			}
			else if ( background->getBorderPosition() == BorderPosition::eExternal )
			{
				bordersWidth = 0;
				bordersHeight = 0;
			}
		}

		bordersWidth /= 2;
		bordersHeight /= 2;
		auto & size = getSize();
		m_clientRect = doUpdateClientRect( { bordersWidth
			, bordersHeight
			, size->x - bordersWidth
			, size->y - bordersHeight } );
	}
}

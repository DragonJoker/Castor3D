#include "ControlsManager.hpp"

#include "CtrlControl.hpp"
#include "CtrlButton.hpp"
#include "CtrlComboBox.hpp"
#include "CtrlEdit.hpp"
#include "CtrlListBox.hpp"
#include "CtrlSlider.hpp"
#include "CtrlStatic.hpp"

#include <Engine.hpp>
#include <Event/Frame/InitialiseEvent.hpp>
#include <Event/Frame/FunctorEvent.hpp>
#include <Overlay/BorderPanelOverlay.hpp>

using namespace castor;
using namespace castor3d;

namespace CastorGui
{
	ControlsManager::ControlsManager( Engine & engine )
		: UserInputListener{ engine, PLUGIN_NAME }
		, m_changed{ true }
	{
	}

	ControlsManager::~ControlsManager()
	{
	}

	bool ControlsManager::doInitialise()
	{
		return true;
	}

	void ControlsManager::doCleanup()
	{
		auto lock = makeUniqueLock( m_mutexHandlers );

		for ( auto handler : m_handlers )
		{
			std::static_pointer_cast< Control >( handler )->destroy();
		}
	}

	bool ControlsManager::fireMaterialEvent( castor::String const & p_overlay, castor::String const & p_material )
	{
		auto lock = makeUniqueLock( m_mutexControls );
		auto it = std::find_if( std::begin( m_controlsByZIndex )
			, std::end( m_controlsByZIndex )
			, [&p_overlay]( ControlSPtr p_control )
		{
			return p_control->getName() == p_overlay;
		} );
		bool result = false;

		if ( it != std::end( m_controlsByZIndex ) )
		{
			auto material = getEngine()->getMaterialCache().find( p_material );

			if ( material )
			{
				auto control = *it;
				m_frameListener->postEvent( MakeFunctorEvent( EventType::ePreRender
					, [control, material]()
					{
						control->setBackgroundMaterial( material );
					} ) );
				result = true;
			}
		}

		return result;
	}

	bool ControlsManager::fireTextEvent( castor::String const & p_overlay, castor::String const & p_caption )
	{
		auto lock = makeUniqueLock( m_mutexControls );
		auto it = std::find_if( std::begin( m_controlsByZIndex )
			, std::end( m_controlsByZIndex )
			, [&p_overlay]( ControlSPtr p_control )
		{
			return p_control->getName() == p_overlay;
		} );
		bool result = false;

		if ( it != std::end( m_controlsByZIndex ) )
		{
			auto control = *it;
			m_frameListener->postEvent( MakeFunctorEvent( EventType::ePreRender
				, [control, p_caption]()
				{
					control->setCaption( p_caption );
				} ) );
			result = true;
		}

		return result;
	}

	void ControlsManager::create( ControlSPtr p_control )
	{
		addControl( p_control );
		p_control->create( shared_from_this() );
	}

	void ControlsManager::destroy( ControlSPtr p_control )
	{
		p_control->destroy();
		removeControl( p_control->getId() );
	}

	void ControlsManager::addControl( ControlSPtr p_control )
	{
		doAddHandler( p_control );

		auto lock = makeUniqueLock( m_mutexControls );

		if ( m_controlsById.find( p_control->getId() ) != m_controlsById.end() )
		{
			CASTOR_EXCEPTION( "A control with this ID already exists in the manager" );
		}

		m_controlsById.insert( std::make_pair( p_control->getId(), p_control ) );
		m_changed = true;
	}

	void ControlsManager::removeControl( uint32_t p_id )
	{
		auto lock = makeUniqueLock( m_mutexControls );
		doRemoveControl( p_id );
	}

	ControlSPtr ControlsManager::getControl( uint32_t p_id )
	{
		auto lock = makeUniqueLock( m_mutexControls );
		auto it = m_controlsById.find( p_id );

		if ( it == m_controlsById.end() )
		{
			CASTOR_EXCEPTION( "This control does not exist in the manager" );
		}

		return it->second.lock();
	}

	void ControlsManager::connectEvents( ButtonCtrl & p_control )
	{
		m_onButtonClicks.emplace( &p_control, p_control.connect( ButtonEvent::eClicked
			, [this, &p_control]()
			{
				onClickAction( p_control.getName() );
			} ) );
	}

	void ControlsManager::connectEvents( ComboBoxCtrl & p_control )
	{
		m_onComboSelects.emplace( &p_control, p_control.connect( ComboBoxEvent::eSelected
			, [this, &p_control]( int p_index )
			{
				onSelectAction( p_control.getName(), p_index );
			} ) );
	}

	void ControlsManager::connectEvents( EditCtrl & p_control )
	{
		m_onEditUpdates.emplace( &p_control, p_control.connect( EditEvent::eUpdated
			, [this, &p_control]( String const & p_text )
			{
				onTextAction( p_control.getName(), p_text );
			} ) );
	}

	void ControlsManager::connectEvents( ListBoxCtrl & p_control )
	{
		m_onListSelects.emplace( &p_control, p_control.connect( ListBoxEvent::eSelected
			, [this, &p_control]( int p_index )
			{
				onSelectAction( p_control.getName(), p_index );
			} ) );
	}

	void ControlsManager::connectEvents( SliderCtrl & p_control )
	{
		m_onSliderTracks.emplace( &p_control, p_control.connect( SliderEvent::eThumbTrack
			, [this, &p_control]( int p_index )
			{
				onSelectAction( p_control.getName(), p_index );
			} ) );
		m_onSliderReleases.emplace( &p_control, p_control.connect( SliderEvent::eThumbTrack
			, [this, &p_control]( int p_index )
			{
				onSelectAction( p_control.getName(), p_index );
			} ) );
	}

	void ControlsManager::disconnectEvents( ButtonCtrl & p_control )
	{
		auto it = m_onButtonClicks.find( &p_control );

		if ( it != m_onButtonClicks.end() )
		{
			m_onButtonClicks.erase( it );
		}
	}

	void ControlsManager::disconnectEvents( ComboBoxCtrl & p_control )
	{
		auto it = m_onComboSelects.find( &p_control );

		if ( it != m_onComboSelects.end() )
		{
			m_onComboSelects.erase( it );
		}
	}

	void ControlsManager::disconnectEvents( EditCtrl & p_control )
	{
		auto it = m_onEditUpdates.find( &p_control );

		if ( it != m_onEditUpdates.end() )
		{
			m_onEditUpdates.erase( it );
		}
	}

	void ControlsManager::disconnectEvents( ListBoxCtrl & p_control )
	{
		auto it = m_onListSelects.find( &p_control );

		if ( it != m_onListSelects.end() )
		{
			m_onListSelects.erase( it );
		}
	}

	void ControlsManager::disconnectEvents( SliderCtrl & p_control )
	{
		auto it = m_onSliderTracks.find( &p_control );

		if ( it != m_onSliderTracks.end() )
		{
			m_onSliderTracks.erase( it );
		}

		it = m_onSliderReleases.find( &p_control );

		if ( it != m_onSliderReleases.end() )
		{
			m_onSliderReleases.erase( it );
		}
	}

	EventHandlerSPtr ControlsManager::doGetMouseTargetableHandler( Position const & p_position )const
	{
		if ( m_changed )
		{
			doUpdate();
		}

		auto lock = makeUniqueLock( m_mutexControls );
		EventHandlerSPtr result;
		auto it = m_controlsByZIndex.rbegin();

		while ( !result && it != m_controlsByZIndex.rend() )
		{
			ControlSPtr control = *it;

			if ( control->catchesMouseEvents()
					&& control->getAbsolutePosition().x() <= p_position.x()
					&& control->getAbsolutePosition().x() + int32_t( control->getSize().getWidth() ) > p_position.x()
					&& control->getAbsolutePosition().y() <= p_position.y()
					&& control->getAbsolutePosition().y() + int32_t( control->getSize().getHeight() ) > p_position.y()
			   )
			{
				result = control;
			}

			++it;
		}

		return result;
	}

	void ControlsManager::doUpdate()const
	{
		auto lock = makeUniqueLock( m_mutexControls );
		{
			auto handlers = doGetHandlers();
			m_controlsByZIndex.clear();

			for ( auto handler : handlers )
			{
				m_controlsByZIndex.push_back( std::static_pointer_cast< Control >( handler ) );
			}
		}

		std::sort( m_controlsByZIndex.begin(), m_controlsByZIndex.end(), []( ControlSPtr p_a, ControlSPtr p_b )
		{
			uint64_t a = p_a->getBackground()->getIndex() + p_a->getBackground()->getLevel() * 1000;
			uint64_t b = p_b->getBackground()->getIndex() + p_b->getBackground()->getLevel() * 1000;
			return a < b;
		} );
	}

	void ControlsManager::doFlush()
	{
		cleanup();
	}

	void ControlsManager::doRemoveControl( uint32_t p_id )
	{
		EventHandlerSPtr handler;
		{
			auto lock = makeUniqueLock( m_mutexControls );
			auto it = m_controlsById.find( p_id );

			if ( it == m_controlsById.end() )
			{
				CASTOR_EXCEPTION( "This control does not exist in the manager." );
			}

			m_controlsById.erase( it );
			handler = it->second.lock();
		}

		m_changed = true;
		doRemoveHandler( handler );
	}
}

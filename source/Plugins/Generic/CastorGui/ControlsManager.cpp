#include "CastorGui/ControlsManager.hpp"

#include "CastorGui/CtrlControl.hpp"
#include "CastorGui/CtrlButton.hpp"
#include "CastorGui/CtrlComboBox.hpp"
#include "CastorGui/CtrlEdit.hpp"
#include "CastorGui/CtrlListBox.hpp"
#include "CastorGui/CtrlSlider.hpp"
#include "CastorGui/CtrlStatic.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Event/Frame/InitialiseEvent.hpp>
#include <Castor3D/Event/Frame/FunctorEvent.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>

using namespace castor;
using namespace castor3d;

namespace CastorGui
{
	namespace
	{
		using LockType = std::unique_lock< std::mutex >;
	}

	ControlsManager::ControlsManager( Engine & engine )
		: UserInputListener{ engine, PLUGIN_NAME }
		, m_changed{ false }
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
		LockType lock{ makeUniqueLock( m_mutexHandlers ) };

		for ( auto handler : m_handlers )
		{
			std::static_pointer_cast< Control >( handler )->destroy();
		}
	}

	bool ControlsManager::fireMaterialEvent( castor::String const & p_overlay, castor::String const & p_material )
	{
		auto controls = doGetControlsByZIndex();
		auto it = std::find_if( std::begin( controls )
			, std::end( controls )
			, [&p_overlay]( ControlSPtr p_control )
		{
			return p_control->getName() == p_overlay;
		} );
		bool result = false;

		if ( it != std::end( controls ) )
		{
			auto material = getEngine()->getMaterialCache().find( p_material );

			if ( material )
			{
				auto control = *it;
				m_frameListener->postEvent( makeFunctorEvent( EventType::ePreRender
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
		auto controls = doGetControlsByZIndex();
		auto it = std::find_if( std::begin( controls )
			, std::end( controls )
			, [&p_overlay]( ControlSPtr p_control )
		{
			return p_control->getName() == p_overlay;
		} );
		bool result = false;

		if ( it != std::end( controls ) )
		{
			auto control = *it;
			m_frameListener->postEvent( makeFunctorEvent( EventType::ePreRender
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
		LockType lock{ makeUniqueLock( m_mutexControlsById ) };

		if ( m_controlsById.find( p_control->getId() ) != m_controlsById.end() )
		{
			CU_Exception( "A control with this ID already exists in the manager" );
		}

		m_controlsById.insert( std::make_pair( p_control->getId(), p_control ) );
		m_changed = true;
	}

	void ControlsManager::removeControl( uint32_t p_id )
	{
		doRemoveControl( p_id );
	}

	ControlSPtr ControlsManager::getControl( uint32_t p_id )
	{
		auto controls = doGetControlsById();
		auto it = controls.find( p_id );

		if ( it == controls.end() )
		{
			CU_Exception( "This control does not exist in the manager" );
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

		auto controls = doGetControlsByZIndex();
		EventHandlerSPtr result;
		auto it = controls.rbegin();

		while ( !result && it != controls.rend() )
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
		LockType lock{ makeUniqueLock( m_mutexControlsByZIndex ) };
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
			LockType lock{ makeUniqueLock( m_mutexControlsById ) };
			auto it = m_controlsById.find( p_id );

			if ( it == m_controlsById.end() )
			{
				CU_Exception( "This control does not exist in the manager." );
			}

			m_controlsById.erase( it );
			handler = it->second.lock();
		}

		m_changed = true;
		doRemoveHandler( handler );
	}

	std::vector< ControlSPtr > ControlsManager::doGetControlsByZIndex()const
	{
		LockType lock{ makeUniqueLock( m_mutexControlsByZIndex ) };
		std::vector< ControlSPtr > result;

		if ( !m_controlsByZIndex.empty() )
		{
			result = m_controlsByZIndex;
		}

		return result;
	}

	std::map< uint32_t, ControlWPtr > ControlsManager::doGetControlsById()const
	{
		LockType lock{ makeUniqueLock( m_mutexControlsById ) };
		std::map< uint32_t, ControlWPtr > result;

		if ( !m_controlsById.empty() )
		{
			result = m_controlsById;
		}

		return result;
	}
}

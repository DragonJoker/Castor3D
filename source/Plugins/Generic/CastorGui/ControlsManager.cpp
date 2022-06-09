#include "CastorGui/ControlsManager.hpp"

#include "CastorGui/Controls/CtrlControl.hpp"
#include "CastorGui/Controls/CtrlButton.hpp"
#include "CastorGui/Controls/CtrlComboBox.hpp"
#include "CastorGui/Controls/CtrlEdit.hpp"
#include "CastorGui/Controls/CtrlListBox.hpp"
#include "CastorGui/Controls/CtrlSlider.hpp"
#include "CastorGui/Controls/CtrlStatic.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Event/Frame/GpuFunctorEvent.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>

namespace CastorGui
{
	namespace ctrlmgr
	{
		using LockType = std::unique_lock< std::mutex >;
	}

	ControlsManager::ControlsManager( castor3d::Engine & engine )
		: castor3d::UserInputListener{ engine, PLUGIN_NAME }
		, m_changed{ false }
	{
	}

	ThemeRPtr ControlsManager::createTheme( castor::String const & name )
	{
		auto ires = m_themes.emplace( name, nullptr );
		auto it = ires.first;

		if ( ires.second )
		{
			it->second = std::make_unique< Theme >( name, *getEngine() );
		}

		return it->second.get();
	}

	ThemeRPtr ControlsManager::getTheme( castor::String const & name )
	{
		auto it = m_themes.find( name );

		if ( it == m_themes.end() )
		{
			return nullptr;
		}

		return it->second.get();
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
		ctrlmgr::LockType lock{ castor::makeUniqueLock( m_mutexControlsById ) };

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
			, [this, &p_control]( castor::String const & p_text )
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

	bool ControlsManager::doInitialise()
	{
		return true;
	}

	void ControlsManager::doCleanup()
	{
		ctrlmgr::LockType lock{ castor::makeUniqueLock( m_mutexHandlers ) };

		for ( auto handler : m_handlers )
		{
			std::static_pointer_cast< Control >( handler )->destroy();
		}
	}

	castor3d::EventHandler * ControlsManager::doGetMouseTargetableHandler( castor::Position const & p_position )const
	{
		if ( m_changed )
		{
			doUpdate();
		}

		auto controls = doGetControlsByZIndex();
		castor3d::EventHandler * result{};
		auto it = controls.rbegin();

		while ( !result && it != controls.rend() )
		{
			Control * control = *it;

			if ( control
				&& control->catchesMouseEvents()
				&& control->getAbsolutePosition().x() <= p_position.x()
				&& control->getAbsolutePosition().x() + int32_t( control->getSize().getWidth() ) > p_position.x()
				&& control->getAbsolutePosition().y() <= p_position.y()
				&& control->getAbsolutePosition().y() + int32_t( control->getSize().getHeight() ) > p_position.y() )
			{
				result = control;
			}

			++it;
		}

		return result;
	}

	void ControlsManager::doUpdate()const
	{
		ctrlmgr::LockType lock{ castor::makeUniqueLock( m_mutexControlsByZIndex ) };
		{
			auto handlers = doGetHandlers();
			m_controlsByZIndex.clear();

			for ( auto handler : handlers )
			{
				m_controlsByZIndex.push_back( static_cast< Control * >( handler.get() ) );
			}
		}

		std::sort( m_controlsByZIndex.begin()
			, m_controlsByZIndex.end()
			, []( Control * lhs, Control * rhs )
			{
				bool result = false;
				auto lhsBg = lhs ? lhs->getBackground() : nullptr;
				auto rhsBg = rhs ? rhs->getBackground() : nullptr;

				if ( !lhs || !lhsBg )
				{
					result = true;
				}
				else if ( !rhs || !rhsBg )
				{
					result = true;
				}
				else
				{
					uint64_t a = lhsBg->getIndex() + lhsBg->getLevel() * 1000ull;
					uint64_t b = rhsBg->getIndex() + rhsBg->getLevel() * 1000ull;
					result = a < b;
				}

				return result;
			} );
	}

	void ControlsManager::doFlush()
	{
		cleanup();
	}

	void ControlsManager::doRemoveControl( uint32_t p_id )
	{
		castor3d::EventHandler * handler;
		{
			ctrlmgr::LockType lock{ castor::makeUniqueLock( m_mutexControlsById ) };
			auto it = m_controlsById.find( p_id );

			if ( it == m_controlsById.end() )
			{
				CU_Exception( "This control does not exist in the manager." );
			}

			m_controlsById.erase( it );
			handler = it->second.lock().get();
		}

		m_changed = true;
		doRemoveHandler( *handler );
	}

	std::vector< Control * > ControlsManager::doGetControlsByZIndex()const
	{
		ctrlmgr::LockType lock{ castor::makeUniqueLock( m_mutexControlsById ) };
		std::vector< Control * > result;

		if ( !m_controlsByZIndex.empty() )
		{
			result = m_controlsByZIndex;
		}

		return result;
	}

	std::map< uint32_t, ControlWPtr > ControlsManager::doGetControlsById()const
	{
		ctrlmgr::LockType lock{ castor::makeUniqueLock( m_mutexControlsById ) };
		std::map< uint32_t, ControlWPtr > result;

		if ( !m_controlsById.empty() )
		{
			result = m_controlsById;
		}

		return result;
	}
}

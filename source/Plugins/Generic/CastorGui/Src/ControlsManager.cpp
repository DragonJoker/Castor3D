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

using namespace Castor;
using namespace Castor3D;

namespace CastorGui
{
	ControlsManager::ControlsManager( Engine & p_engine )
		: UserInputListener{ p_engine, PLUGIN_NAME }
		, m_changed{ true }
	{
	}

	ControlsManager::~ControlsManager()
	{
	}

	bool ControlsManager::DoInitialise()
	{
		return true;
	}

	void ControlsManager::DoCleanup()
	{
		auto lock = make_unique_lock( m_mutexHandlers );

		for ( auto handler : m_handlers )
		{
			std::static_pointer_cast< Control >( handler )->Destroy();
		}
	}

	bool ControlsManager::FireMaterialEvent( Castor::String const & p_overlay, Castor::String const & p_material )
	{
		auto lock = make_unique_lock( m_mutexControls );
		auto it = std::find_if( std::begin( m_controlsByZIndex )
			, std::end( m_controlsByZIndex )
			, [&p_overlay]( ControlSPtr p_control )
		{
			return p_control->GetName() == p_overlay;
		} );
		bool result = false;

		if ( it != std::end( m_controlsByZIndex ) )
		{
			auto material = GetEngine()->GetMaterialCache().Find( p_material );

			if ( material )
			{
				auto control = *it;
				m_frameListener->PostEvent( MakeFunctorEvent( EventType::ePreRender
					, [control, material]()
					{
						control->SetBackgroundMaterial( material );
					} ) );
				result = true;
			}
		}

		return result;
	}

	bool ControlsManager::FireTextEvent( Castor::String const & p_overlay, Castor::String const & p_caption )
	{
		auto lock = make_unique_lock( m_mutexControls );
		auto it = std::find_if( std::begin( m_controlsByZIndex )
			, std::end( m_controlsByZIndex )
			, [&p_overlay]( ControlSPtr p_control )
		{
			return p_control->GetName() == p_overlay;
		} );
		bool result = false;

		if ( it != std::end( m_controlsByZIndex ) )
		{
			auto control = *it;
			m_frameListener->PostEvent( MakeFunctorEvent( EventType::ePreRender
				, [control, p_caption]()
				{
					control->SetCaption( p_caption );
				} ) );
			result = true;
		}

		return result;
	}

	void ControlsManager::Create( ControlSPtr p_control )
	{
		AddControl( p_control );
		p_control->Create( shared_from_this() );
	}

	void ControlsManager::Destroy( ControlSPtr p_control )
	{
		p_control->Destroy();
		RemoveControl( p_control->GetId() );
	}

	void ControlsManager::AddControl( ControlSPtr p_control )
	{
		DoAddHandler( p_control );

		auto lock = make_unique_lock( m_mutexControls );

		if ( m_controlsById.find( p_control->GetId() ) != m_controlsById.end() )
		{
			CASTOR_EXCEPTION( "A control with this ID already exists in the manager" );
		}

		m_controlsById.insert( std::make_pair( p_control->GetId(), p_control ) );
		m_changed = true;
	}

	void ControlsManager::RemoveControl( uint32_t p_id )
	{
		auto lock = make_unique_lock( m_mutexControls );
		DoRemoveControl( p_id );
	}

	ControlSPtr ControlsManager::GetControl( uint32_t p_id )
	{
		auto lock = make_unique_lock( m_mutexControls );
		auto it = m_controlsById.find( p_id );

		if ( it == m_controlsById.end() )
		{
			CASTOR_EXCEPTION( "This control does not exist in the manager" );
		}

		return it->second.lock();
	}

	void ControlsManager::ConnectEvents( ButtonCtrl & p_control )
	{
		m_onButtonClicks.emplace( &p_control, p_control.Connect( ButtonEvent::eClicked
			, [this, &p_control]()
			{
				OnClickAction( p_control.GetName() );
			} ) );
	}

	void ControlsManager::ConnectEvents( ComboBoxCtrl & p_control )
	{
		m_onComboSelects.emplace( &p_control, p_control.Connect( ComboBoxEvent::eSelected
			, [this, &p_control]( int p_index )
			{
				OnSelectAction( p_control.GetName(), p_index );
			} ) );
	}

	void ControlsManager::ConnectEvents( EditCtrl & p_control )
	{
		m_onEditUpdates.emplace( &p_control, p_control.Connect( EditEvent::eUpdated
			, [this, &p_control]( String const & p_text )
			{
				OnTextAction( p_control.GetName(), p_text );
			} ) );
	}

	void ControlsManager::ConnectEvents( ListBoxCtrl & p_control )
	{
		m_onListSelects.emplace( &p_control, p_control.Connect( ListBoxEvent::eSelected
			, [this, &p_control]( int p_index )
			{
				OnSelectAction( p_control.GetName(), p_index );
			} ) );
	}

	void ControlsManager::ConnectEvents( SliderCtrl & p_control )
	{
		m_onSliderTracks.emplace( &p_control, p_control.Connect( SliderEvent::eThumbTrack
			, [this, &p_control]( int p_index )
			{
				OnSelectAction( p_control.GetName(), p_index );
			} ) );
		m_onSliderReleases.emplace( &p_control, p_control.Connect( SliderEvent::eThumbTrack
			, [this, &p_control]( int p_index )
			{
				OnSelectAction( p_control.GetName(), p_index );
			} ) );
	}

	void ControlsManager::DisconnectEvents( ButtonCtrl & p_control )
	{
		auto it = m_onButtonClicks.find( &p_control );

		if ( it != m_onButtonClicks.end() )
		{
			m_onButtonClicks.erase( it );
		}
	}

	void ControlsManager::DisconnectEvents( ComboBoxCtrl & p_control )
	{
		auto it = m_onComboSelects.find( &p_control );

		if ( it != m_onComboSelects.end() )
		{
			m_onComboSelects.erase( it );
		}
	}

	void ControlsManager::DisconnectEvents( EditCtrl & p_control )
	{
		auto it = m_onEditUpdates.find( &p_control );

		if ( it != m_onEditUpdates.end() )
		{
			m_onEditUpdates.erase( it );
		}
	}

	void ControlsManager::DisconnectEvents( ListBoxCtrl & p_control )
	{
		auto it = m_onListSelects.find( &p_control );

		if ( it != m_onListSelects.end() )
		{
			m_onListSelects.erase( it );
		}
	}

	void ControlsManager::DisconnectEvents( SliderCtrl & p_control )
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

	EventHandlerSPtr ControlsManager::DoGetMouseTargetableHandler( Position const & p_position )const
	{
		if ( m_changed )
		{
			DoUpdate();
		}

		auto lock = make_unique_lock( m_mutexControls );
		EventHandlerSPtr result;
		auto it = m_controlsByZIndex.rbegin();

		while ( !result && it != m_controlsByZIndex.rend() )
		{
			ControlSPtr control = *it;

			if ( control->CatchesMouseEvents()
					&& control->GetAbsolutePosition().x() <= p_position.x()
					&& control->GetAbsolutePosition().x() + int32_t( control->GetSize().width() ) > p_position.x()
					&& control->GetAbsolutePosition().y() <= p_position.y()
					&& control->GetAbsolutePosition().y() + int32_t( control->GetSize().height() ) > p_position.y()
			   )
			{
				result = control;
			}

			++it;
		}

		return result;
	}

	void ControlsManager::DoUpdate()const
	{
		auto lock = make_unique_lock( m_mutexControls );
		{
			auto handlers = DoGetHandlers();

			for ( auto handler : handlers )
			{
				m_controlsByZIndex.push_back( std::static_pointer_cast< Control >( handler ) );
			}
		}

		std::sort( m_controlsByZIndex.begin(), m_controlsByZIndex.end(), []( ControlSPtr p_a, ControlSPtr p_b )
		{
			uint64_t a = p_a->GetBackground()->GetIndex() + p_a->GetBackground()->GetLevel() * 1000;
			uint64_t b = p_b->GetBackground()->GetIndex() + p_b->GetBackground()->GetLevel() * 1000;
			return a < b;
		} );
	}

	void ControlsManager::DoFlush()
	{
		Cleanup();
	}

	void ControlsManager::DoRemoveControl( uint32_t p_id )
	{
		EventHandlerSPtr handler;
		{
			auto lock = make_unique_lock( m_mutexControls );
			auto it = m_controlsById.find( p_id );

			if ( it == m_controlsById.end() )
			{
				CASTOR_EXCEPTION( "This control does not exist in the manager." );
			}

			m_controlsById.erase( it );
			handler = it->second.lock();
		}

		m_changed = true;
		DoRemoveHandler( handler );
	}
}

#include "ControlsManager.hpp"

#include "CtrlControl.hpp"
#include "CtrlButton.hpp"
#include "CtrlComboBox.hpp"
#include "CtrlEdit.hpp"
#include "CtrlListBox.hpp"
#include "CtrlSlider.hpp"
#include "CtrlStatic.hpp"

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
		auto l_lock = make_unique_lock( m_mutexHandlers );

		for ( auto l_handler : m_handlers )
		{
			std::static_pointer_cast< Control >( l_handler )->Destroy();
		}
	}

	bool ControlsManager::FireMaterialEvent( Castor::String const & p_overlay, Castor::String const & p_material )
	{
		auto l_lock = make_unique_lock( m_mutexControls );
		auto l_it = std::find_if( std::begin( m_controlsByZIndex )
			, std::end( m_controlsByZIndex )
			, [&p_overlay]( ControlSPtr p_control )
		{
			return p_control->GetName() == p_overlay;
		} );
		bool l_result = false;

		if ( l_it != std::end( m_controlsByZIndex ) )
		{
			auto l_material = GetEngine()->GetMaterialCache().Find( p_material );

			if ( l_material )
			{
				auto l_control = *l_it;
				m_frameListener->PostEvent( MakeFunctorEvent( EventType::ePreRender
					, [l_control, l_material]()
					{
						l_control->SetBackgroundMaterial( l_material );
					} ) );
				l_result = true;
			}
		}

		return l_result;
	}

	bool ControlsManager::FireTextEvent( Castor::String const & p_overlay, Castor::String const & p_caption )
	{
		auto l_lock = make_unique_lock( m_mutexControls );
		auto l_it = std::find_if( std::begin( m_controlsByZIndex )
			, std::end( m_controlsByZIndex )
			, [&p_overlay]( ControlSPtr p_control )
		{
			return p_control->GetName() == p_overlay;
		} );
		bool l_result = false;

		if ( l_it != std::end( m_controlsByZIndex ) )
		{
			auto l_control = *l_it;
			m_frameListener->PostEvent( MakeFunctorEvent( EventType::ePreRender
				, [l_control, p_caption]()
				{
					l_control->SetCaption( p_caption );
				} ) );
			l_result = true;
		}

		return l_result;
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

		auto l_lock = make_unique_lock( m_mutexControls );

		if ( m_controlsById.find( p_control->GetId() ) != m_controlsById.end() )
		{
			CASTOR_EXCEPTION( "A control with this ID already exists in the manager" );
		}

		m_controlsById.insert( std::make_pair( p_control->GetId(), p_control ) );
		m_changed = true;
	}

	void ControlsManager::RemoveControl( uint32_t p_id )
	{
		auto l_lock = make_unique_lock( m_mutexControls );
		DoRemoveControl( p_id );
	}

	ControlSPtr ControlsManager::GetControl( uint32_t p_id )
	{
		auto l_lock = make_unique_lock( m_mutexControls );
		auto l_it = m_controlsById.find( p_id );

		if ( l_it == m_controlsById.end() )
		{
			CASTOR_EXCEPTION( "This control does not exist in the manager" );
		}

		return l_it->second.lock();
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
		auto l_it = m_onButtonClicks.find( &p_control );

		if ( l_it != m_onButtonClicks.end() )
		{
			m_onButtonClicks.erase( l_it );
		}
	}

	void ControlsManager::DisconnectEvents( ComboBoxCtrl & p_control )
	{
		auto l_it = m_onComboSelects.find( &p_control );

		if ( l_it != m_onComboSelects.end() )
		{
			m_onComboSelects.erase( l_it );
		}
	}

	void ControlsManager::DisconnectEvents( EditCtrl & p_control )
	{
		auto l_it = m_onEditUpdates.find( &p_control );

		if ( l_it != m_onEditUpdates.end() )
		{
			m_onEditUpdates.erase( l_it );
		}
	}

	void ControlsManager::DisconnectEvents( ListBoxCtrl & p_control )
	{
		auto l_it = m_onListSelects.find( &p_control );

		if ( l_it != m_onListSelects.end() )
		{
			m_onListSelects.erase( l_it );
		}
	}

	void ControlsManager::DisconnectEvents( SliderCtrl & p_control )
	{
		auto l_it = m_onSliderTracks.find( &p_control );

		if ( l_it != m_onSliderTracks.end() )
		{
			m_onSliderTracks.erase( l_it );
		}

		l_it = m_onSliderReleases.find( &p_control );

		if ( l_it != m_onSliderReleases.end() )
		{
			m_onSliderReleases.erase( l_it );
		}
	}

	EventHandlerSPtr ControlsManager::DoGetMouseTargetableHandler( Position const & p_position )const
	{
		if ( m_changed )
		{
			DoUpdate();
		}

		auto l_lock = make_unique_lock( m_mutexControls );
		EventHandlerSPtr l_return;
		auto l_it = m_controlsByZIndex.rbegin();

		while ( !l_return && l_it != m_controlsByZIndex.rend() )
		{
			ControlSPtr l_control = *l_it;

			if ( l_control->CatchesMouseEvents()
					&& l_control->GetAbsolutePosition().x() <= p_position.x()
					&& l_control->GetAbsolutePosition().x() + int32_t( l_control->GetSize().width() ) > p_position.x()
					&& l_control->GetAbsolutePosition().y() <= p_position.y()
					&& l_control->GetAbsolutePosition().y() + int32_t( l_control->GetSize().height() ) > p_position.y()
			   )
			{
				l_return = l_control;
			}

			++l_it;
		}

		return l_return;
	}

	void ControlsManager::DoUpdate()const
	{
		auto l_lock = make_unique_lock( m_mutexControls );
		{
			auto l_handlers = DoGetHandlers();

			for ( auto l_handler : l_handlers )
			{
				m_controlsByZIndex.push_back( std::static_pointer_cast< Control >( l_handler ) );
			}
		}

		std::sort( m_controlsByZIndex.begin(), m_controlsByZIndex.end(), []( ControlSPtr p_a, ControlSPtr p_b )
		{
			uint64_t l_a = p_a->GetBackground()->GetIndex() + p_a->GetBackground()->GetLevel() * 1000;
			uint64_t l_b = p_b->GetBackground()->GetIndex() + p_b->GetBackground()->GetLevel() * 1000;
			return l_a < l_b;
		} );
	}

	void ControlsManager::DoFlush()
	{
		Cleanup();
	}

	void ControlsManager::DoRemoveControl( uint32_t p_id )
	{
		EventHandlerSPtr l_handler;
		{
			auto l_lock = make_unique_lock( m_mutexControls );
			auto l_it = m_controlsById.find( p_id );

			if ( l_it == m_controlsById.end() )
			{
				CASTOR_EXCEPTION( "This control does not exist in the manager." );
			}

			m_controlsById.erase( l_it );
			l_handler = l_it->second.lock();
		}

		m_changed = true;
		DoRemoveHandler( l_handler );
	}
}

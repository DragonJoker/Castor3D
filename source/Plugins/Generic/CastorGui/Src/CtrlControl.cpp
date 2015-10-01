#include "CtrlControl.hpp"

#include "ControlsManager.hpp"

#include <BorderPanelOverlay.hpp>
#include <FunctorEvent.hpp>
#include <InitialiseEvent.hpp>
#include <Material.hpp>
#include <OverlayManager.hpp>
#include <Pass.hpp>
#include <TextureUnit.hpp>

using namespace Castor;
using namespace Castor3D;

namespace CastorGui
{
	Control::Control( eCONTROL_TYPE p_type, ControlRPtr p_parent, uint32_t p_id, Position const & p_position, Size const & p_size, uint32_t p_style, bool p_visible )
		: EventHandler< Control >( p_type != eCONTROL_TYPE_STATIC )
		, m_type( p_type )
		, m_id( p_id )
		, m_position( p_position )
		, m_size( p_size )
		, m_style( p_style )
		, m_visible( p_visible )
		, m_borders( 0, 0, 0, 0 )
		, m_cursor( eMOUSE_CURSOR_HAND )
		, m_parent( p_parent )
		, m_engine( NULL )
	{
	}

	Control::~Control()
	{
	}

	void Control::Create( ControlsManagerSPtr p_ctrlManager, Engine * p_engine )
	{
		m_engine = p_engine;
		m_ctrlManager = p_ctrlManager;
		OverlaySPtr l_parentOv;
		ControlRPtr l_parent = GetParent();

		if ( l_parent )
		{
			l_parent->m_childs.push_back( std::static_pointer_cast< Control >( shared_from_this() ) );
			l_parentOv = l_parent->GetBackground()->GetOverlay().shared_from_this();
		}

		BorderPanelOverlaySPtr l_panel;
		l_panel = GetEngine()->GetOverlayManager().CreateBorderPanel( cuT( "BP_CtrlControl_" ) + string::to_string( GetId() ), GetPosition(), GetSize(), GetBackgroundMaterial(), m_borders, GetForegroundMaterial(), l_parentOv );
		l_panel->SetBorderPosition( eBORDER_POSITION_INTERNAL );
		l_panel->SetVisible( m_visible );
		m_background = l_panel;
		DoCreate();
	}

	void Control::Destroy()
	{
		DoDestroy();
	}

	void Control::SetBackgroundBorders( Rectangle const & p_value )
	{
		m_borders = p_value;
		BorderPanelOverlaySPtr l_panel = GetBackground();

		if ( l_panel )
		{
			l_panel->SetBorderPixelSize( p_value );
		}
	}

	void Control::SetPosition( Position const & p_value )
	{
		m_position = p_value;
		BorderPanelOverlaySPtr l_panel = GetBackground();

		if ( l_panel )
		{
			l_panel->SetPixelPosition( p_value );
			l_panel.reset();
		}

		DoSetPosition( p_value );
	}

	Position Control::GetAbsolutePosition()const
	{
		ControlRPtr l_parent = GetParent();
		Position l_return = m_position;

		if ( l_parent )
		{
			l_return += l_parent->GetAbsolutePosition();
		}

		return l_return;
	}

	void Control::SetSize( Size const & p_value )
	{
		m_size = p_value;
		BorderPanelOverlaySPtr l_panel = GetBackground();

		if ( l_panel )
		{
			l_panel->SetPixelSize( p_value );
			l_panel.reset();
		}

		DoSetSize( p_value );
	}

	void Control::SetBackgroundMaterial( MaterialSPtr p_value )
	{
		m_backgroundMaterial = p_value;
		BorderPanelOverlaySPtr l_panel = GetBackground();

		if ( l_panel )
		{
			l_panel->SetMaterial( p_value );
			l_panel.reset();
		}

		DoSetBackgroundMaterial( p_value );
	}

	void Control::SetForegroundMaterial( MaterialSPtr p_value )
	{
		m_foregroundMaterial = p_value;
		BorderPanelOverlaySPtr l_panel = GetBackground();

		if ( l_panel )
		{
			l_panel->SetBorderMaterial( p_value );
			l_panel.reset();
		}

		DoSetForegroundMaterial( p_value );
	}

	void Control::SetVisible( bool p_value )
	{
		m_visible = p_value;
		BorderPanelOverlaySPtr l_panel = GetBackground();

		if ( l_panel )
		{
			l_panel->SetVisible( p_value );
			l_panel.reset();
		}

		DoSetVisible( p_value );
	}

	bool Control::IsVisible()const
	{
		bool l_visible = m_visible;
		ControlRPtr l_parent = GetParent();

		if ( l_visible && l_parent )
		{
			l_visible = l_parent->IsVisible();
		}

		return l_visible;
	}

	ControlSPtr Control::GetChildControl( uint32_t p_id )
	{
		auto l_it = std::find_if( std::begin( m_childs ), std::end( m_childs ), [&p_id]( ControlWPtr p_ctrl )
		{
			return p_ctrl.expired() ? false : p_ctrl.lock()->GetId() == p_id;
		} );

		if ( l_it == m_childs.end() )
		{
			CASTOR_EXCEPTION( "This control does not exist in my childs" );
		}

		return l_it->lock();
	}
}

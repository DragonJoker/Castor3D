#include "CtrlControl.hpp"

#include "ControlsManager.hpp"

#include <Engine.hpp>
#include <Material/Material.hpp>
#include <Overlay/Overlay.hpp>

#include <Event/Frame/FunctorEvent.hpp>
#include <Event/Frame/InitialiseEvent.hpp>
#include <Material/Pass.hpp>
#include <Overlay/BorderPanelOverlay.hpp>
#include <Overlay/TextOverlay.hpp>
#include <Texture/TextureUnit.hpp>

using namespace Castor;
using namespace Castor3D;

namespace CastorGui
{
	Control::Control( ControlType p_type
		, String const & p_name
		, Engine & p_engine
		, ControlRPtr p_parent
		, uint32_t p_id
		, Position const & p_position
		, Size const & p_size
		, uint32_t p_style
		, bool p_visible )
		: NonClientEventHandler< Control >( p_type != ControlType::eStatic )
		, Named( p_name )
		, m_type( p_type )
		, m_id( p_id )
		, m_position( p_position )
		, m_size( p_size )
		, m_style( p_style )
		, m_visible( p_visible )
		, m_borders( 0, 0, 0, 0 )
		, m_cursor( MouseCursor::eHand )
		, m_parent( p_parent )
		, m_engine( p_engine )
	{
		OverlaySPtr l_parentOv;
		ControlRPtr l_parent = GetParent();

		if ( l_parent )
		{
			l_parentOv = l_parent->GetBackground()->GetOverlay().shared_from_this();
		}

		OverlaySPtr l_overlay = GetEngine().GetOverlayCache().Add( p_name, OverlayType::eBorderPanel, nullptr, l_parentOv );
		l_overlay->SetPixelPosition( GetPosition() );
		l_overlay->SetPixelSize( GetSize() );
		BorderPanelOverlaySPtr l_panel = l_overlay->GetBorderPanelOverlay();
		l_panel->SetBorderPosition( BorderPosition::eInternal );
		l_panel->SetVisible( m_visible );
		m_background = l_panel;
	}

	Control::~Control()
	{
	}

	void Control::Create( ControlsManagerSPtr p_ctrlManager )
	{
		m_ctrlManager = p_ctrlManager;
		ControlRPtr l_parent = GetParent();

		if ( l_parent )
		{
			l_parent->m_children.push_back( std::static_pointer_cast< Control >( shared_from_this() ) );
		}

		BorderPanelOverlaySPtr l_panel = GetBackground();
		l_panel->SetMaterial( GetBackgroundMaterial() );
		l_panel->SetBorderMaterial( GetForegroundMaterial() );
		l_panel->SetBorderPixelSize( m_borders );
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
			l_panel->SetBorderPixelSize( m_borders );
		}
	}

	void Control::SetPosition( Position const & p_value )
	{
		m_position = p_value;
		BorderPanelOverlaySPtr l_panel = GetBackground();

		if ( l_panel )
		{
			l_panel->SetPixelPosition( m_position );
			l_panel.reset();
		}

		DoSetPosition( m_position );
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
			l_panel->SetPixelSize( m_size );
			l_panel.reset();
		}

		DoSetSize( m_size );
	}

	void Control::SetBackgroundMaterial( MaterialSPtr p_value )
	{
		REQUIRE( p_value );
		m_backgroundMaterial = p_value;
		BorderPanelOverlaySPtr l_panel = GetBackground();

		if ( l_panel )
		{
			l_panel->SetMaterial( GetBackgroundMaterial() );
			l_panel.reset();
		}

		DoSetBackgroundMaterial( GetBackgroundMaterial() );
	}

	void Control::SetForegroundMaterial( MaterialSPtr p_value )
	{
		m_foregroundMaterial = p_value;
		BorderPanelOverlaySPtr l_panel = GetBackground();

		if ( l_panel )
		{
			l_panel->SetBorderMaterial( GetForegroundMaterial() );
			l_panel.reset();
		}

		DoSetForegroundMaterial( GetForegroundMaterial() );
	}

	void Control::SetCaption( Castor::String const & p_caption )
	{
		DoSetCaption( p_caption );
	}

	void Control::SetVisible( bool p_value )
	{
		m_visible = p_value;
		BorderPanelOverlaySPtr l_panel = GetBackground();

		if ( l_panel )
		{
			l_panel->SetVisible( m_visible );
			l_panel.reset();
		}

		DoSetVisible( m_visible );
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
		auto l_it = std::find_if( std::begin( m_children ), std::end( m_children ), [&p_id]( ControlWPtr p_ctrl )
		{
			return p_ctrl.expired() ? false : p_ctrl.lock()->GetId() == p_id;
		} );

		if ( l_it == m_children.end() )
		{
			CASTOR_EXCEPTION( "This control does not exist in my childs" );
		}

		return l_it->lock();
	}

	void Control::AddStyle( uint32_t p_style )
	{
		m_style |= p_style;
		DoUpdateStyle();
	}

	void Control::RemoveStyle( uint32_t p_style )
	{
		m_style &= ~p_style;
		DoUpdateStyle();
	}
}

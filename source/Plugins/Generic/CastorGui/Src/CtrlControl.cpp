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
		, Engine & engine
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
		, m_engine( engine )
	{
		OverlaySPtr parentOv;
		ControlRPtr parent = GetParent();

		if ( parent )
		{
			parentOv = parent->GetBackground()->GetOverlay().shared_from_this();
		}

		OverlaySPtr overlay = GetEngine().GetOverlayCache().Add( p_name, OverlayType::eBorderPanel, nullptr, parentOv );
		overlay->SetPixelPosition( GetPosition() );
		overlay->SetPixelSize( GetSize() );
		BorderPanelOverlaySPtr panel = overlay->GetBorderPanelOverlay();
		panel->SetBorderPosition( BorderPosition::eInternal );
		panel->SetVisible( m_visible );
		m_background = panel;
	}

	Control::~Control()
	{
	}

	void Control::Create( ControlsManagerSPtr p_ctrlManager )
	{
		m_ctrlManager = p_ctrlManager;
		ControlRPtr parent = GetParent();

		if ( parent )
		{
			parent->m_children.push_back( std::static_pointer_cast< Control >( shared_from_this() ) );
		}

		BorderPanelOverlaySPtr panel = GetBackground();
		panel->SetMaterial( GetBackgroundMaterial() );
		panel->SetBorderMaterial( GetForegroundMaterial() );
		panel->SetBorderPixelSize( m_borders );
		DoCreate();
	}

	void Control::Destroy()
	{
		DoDestroy();
	}

	void Control::SetBackgroundBorders( Rectangle const & p_value )
	{
		m_borders = p_value;
		BorderPanelOverlaySPtr panel = GetBackground();

		if ( panel )
		{
			panel->SetBorderPixelSize( m_borders );
		}
	}

	void Control::SetPosition( Position const & p_value )
	{
		m_position = p_value;
		BorderPanelOverlaySPtr panel = GetBackground();

		if ( panel )
		{
			panel->SetPixelPosition( m_position );
			panel.reset();
		}

		DoSetPosition( m_position );
	}

	Position Control::GetAbsolutePosition()const
	{
		ControlRPtr parent = GetParent();
		Position result = m_position;

		if ( parent )
		{
			result += parent->GetAbsolutePosition();
		}

		return result;
	}

	void Control::SetSize( Size const & p_value )
	{
		m_size = p_value;
		BorderPanelOverlaySPtr panel = GetBackground();

		if ( panel )
		{
			panel->SetPixelSize( m_size );
			panel.reset();
		}

		DoSetSize( m_size );
	}

	void Control::SetBackgroundMaterial( MaterialSPtr p_value )
	{
		REQUIRE( p_value );
		m_backgroundMaterial = p_value;
		BorderPanelOverlaySPtr panel = GetBackground();

		if ( panel )
		{
			panel->SetMaterial( GetBackgroundMaterial() );
			panel.reset();
		}

		DoSetBackgroundMaterial( GetBackgroundMaterial() );
	}

	void Control::SetForegroundMaterial( MaterialSPtr p_value )
	{
		m_foregroundMaterial = p_value;
		BorderPanelOverlaySPtr panel = GetBackground();

		if ( panel )
		{
			panel->SetBorderMaterial( GetForegroundMaterial() );
			panel.reset();
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
		BorderPanelOverlaySPtr panel = GetBackground();

		if ( panel )
		{
			panel->SetVisible( m_visible );
			panel.reset();
		}

		DoSetVisible( m_visible );
	}

	bool Control::IsVisible()const
	{
		bool visible = m_visible;
		ControlRPtr parent = GetParent();

		if ( visible && parent )
		{
			visible = parent->IsVisible();
		}

		return visible;
	}

	ControlSPtr Control::GetChildControl( uint32_t p_id )
	{
		auto it = std::find_if( std::begin( m_children ), std::end( m_children ), [&p_id]( ControlWPtr p_ctrl )
		{
			return p_ctrl.expired() ? false : p_ctrl.lock()->GetId() == p_id;
		} );

		if ( it == m_children.end() )
		{
			CASTOR_EXCEPTION( "This control does not exist in my childs" );
		}

		return it->lock();
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

#include "Overlay.hpp"

#include "Engine.hpp"

#include "BorderPanelOverlay.hpp"
#include "PanelOverlay.hpp"
#include "TextOverlay.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	Overlay::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< Overlay >{ p_tabs }
	{
	}

	bool Overlay::TextWriter::operator()( Overlay const & p_overlay, TextFile & p_file )
	{
		return p_overlay.m_category->CreateTextWriter( m_tabs )->WriteInto( p_file );
	}

	//*************************************************************************************************

	Overlay::Overlay( Engine & engine, OverlayType p_type )
		: OwnedBy< Engine >( engine )
		, m_parent()
		, m_scene()
		, m_renderSystem( engine.GetRenderSystem() )
		, m_category( engine.GetOverlayCache().GetFactory().Create( p_type ) )
	{
		m_category->SetOverlay( this );
	}

	Overlay::Overlay( Engine & engine, OverlayType p_type, SceneSPtr p_scene, OverlaySPtr p_parent )
		: OwnedBy< Engine >( engine )
		, m_parent( p_parent )
		, m_scene( p_scene )
		, m_renderSystem( engine.GetRenderSystem() )
		, m_category( engine.GetOverlayCache().GetFactory().Create( p_type ) )
	{
		m_category->SetOverlay( this );
	}

	Overlay::~Overlay()
	{
	}

	void Overlay::Render( Castor::Size const & p_size )
	{
		if ( IsVisible() )
		{
			m_category->Render();

			for ( auto overlay : m_overlays )
			{
				overlay->Render( p_size );
			}
		}
	}

	void Overlay::AddChild( OverlaySPtr p_overlay )
	{
		int index = 1;

		if ( !m_overlays.empty() )
		{
			index = ( *( m_overlays.end() - 1 ) )->GetIndex() + 1;
		}

		p_overlay->SetOrder( index, GetLevel() + 1 );
		m_overlays.push_back( p_overlay );
	}

	uint32_t Overlay::GetChildrenCount( int p_level )const
	{
		uint32_t result{ 0 };

		if ( p_level == GetLevel() + 1 )
		{
			result = int( m_overlays.size() );
		}
		else if ( p_level > GetLevel() )
		{
			for ( auto overlay : m_overlays )
			{
				result += overlay->GetChildrenCount( p_level );
			}
		}

		return result;
	}

	PanelOverlaySPtr Overlay::GetPanelOverlay()const
	{
		if ( m_category->GetType() != OverlayType::ePanel )
		{
			CASTOR_EXCEPTION( "This overlay is not a panel." );
		}

		return std::static_pointer_cast< PanelOverlay >( m_category );
	}

	BorderPanelOverlaySPtr Overlay::GetBorderPanelOverlay()const
	{
		if ( m_category->GetType() != OverlayType::eBorderPanel )
		{
			CASTOR_EXCEPTION( "This overlay is not a border panel." );
		}

		return std::static_pointer_cast< BorderPanelOverlay >( m_category );
	}

	TextOverlaySPtr Overlay::GetTextOverlay()const
	{
		if ( m_category->GetType() != OverlayType::eText )
		{
			CASTOR_EXCEPTION( "This overlay is not a text." );
		}

		return std::static_pointer_cast< TextOverlay >( m_category );
	}

	bool Overlay::IsVisible()const
	{
		bool result = m_category->IsVisible();

		if ( result && GetParent() )
		{
			result = GetParent()->IsVisible();
		}

		return result;
	}
}
